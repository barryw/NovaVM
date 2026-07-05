// SPDX-License-Identifier: GPL-2.0
// novacap.c - NovaVM post-OSD HDMI/audio capture as V4L2 + ALSA.

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/videodev2.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-vmalloc.h>
#include <sound/core.h>
#include <sound/initval.h>
#include <sound/pcm.h>

#define NCAP_MAGIC          0x4e434150u
#define NCAP_CONTROL        0x04
#define NCAP_FRAME_SEQ      0x08
#define NCAP_AUDIO_SEQ      0x0c
#define NCAP_VIDEO_DROPS    0x10
#define NCAP_AUDIO_DROPS    0x14
#define NCAP_LAST_FRAME     0x18
#define NCAP_WIDTH          0x1c
#define NCAP_HEIGHT         0x20
#define NCAP_FRAME_STRIDE   0x24
#define NCAP_FRAME_COUNT    0x28
#define NCAP_VIDEO_BASE     0x2c
#define NCAP_AUDIO_BASE     0x30
#define NCAP_AUDIO_BYTES    0x34
#define NCAP_AUDIO_OFFSET   0x38
#define NOVACAP_AUDIO_FRAME_BYTES 4u

struct novacap_buffer {
    struct vb2_v4l2_buffer vb;
    struct list_head list;
};

struct novacap {
    struct device *dev;
    void __iomem *regs;
    void __iomem *mem;
    resource_size_t mem_size;

    u32 width;
    u32 height;
    u32 frame_stride;
    u32 frame_count;
    u32 video_base;
    u32 audio_base;
    u32 audio_bytes;
    u32 audio_mem_offset;

    struct v4l2_device v4l2_dev;
    struct video_device vdev;
    struct vb2_queue queue;
    struct mutex vlock;
    spinlock_t qlock;
    struct list_head queued;
    struct task_struct *video_thread;
    bool video_streaming;
    u32 video_sequence;
    u32 seen_frame_seq;

    struct snd_card *card;
    struct snd_pcm *pcm;
    struct snd_pcm_substream *substream;
    struct task_struct *audio_thread;
    bool audio_running;
    size_t audio_hw_ptr;
    u32 audio_read_offset;
};

static inline u32 ncap_read(struct novacap *cap, u32 offset)
{
    return readl(cap->regs + offset);
}

static inline void ncap_write(struct novacap *cap, u32 offset, u32 value)
{
    writel(value, cap->regs + offset);
}

static struct novacap_buffer *to_novacap_buffer(struct vb2_buffer *vb)
{
    return container_of(to_vb2_v4l2_buffer(vb), struct novacap_buffer, vb);
}

static int novacap_queue_setup(struct vb2_queue *vq, unsigned int *nbufs,
                               unsigned int *nplanes, unsigned int sizes[],
                               struct device *alloc_devs[])
{
    struct novacap *cap = vb2_get_drv_priv(vq);
    unsigned int size = cap->width * cap->height * 4;

    if (*nplanes)
        return sizes[0] < size ? -EINVAL : 0;

    *nplanes = 1;
    sizes[0] = size;
    if (*nbufs < 2)
        *nbufs = 2;
    return 0;
}

static int novacap_buf_prepare(struct vb2_buffer *vb)
{
    struct novacap *cap = vb2_get_drv_priv(vb->vb2_queue);
    unsigned int size = cap->width * cap->height * 4;

    if (vb2_plane_size(vb, 0) < size)
        return -EINVAL;
    vb2_set_plane_payload(vb, 0, size);
    return 0;
}

static void novacap_buf_queue(struct vb2_buffer *vb)
{
    struct novacap *cap = vb2_get_drv_priv(vb->vb2_queue);
    struct novacap_buffer *buf = to_novacap_buffer(vb);
    unsigned long flags;

    spin_lock_irqsave(&cap->qlock, flags);
    list_add_tail(&buf->list, &cap->queued);
    spin_unlock_irqrestore(&cap->qlock, flags);
}

static void novacap_copy_frame(struct novacap *cap, void *dst, u32 frame)
{
    u32 row;
    void __iomem *src = cap->mem + frame * cap->frame_stride;
    u32 row_bytes = cap->width * 4;

    for (row = 0; row < cap->height; row++)
        memcpy_fromio(dst + row * row_bytes, src + row * row_bytes, row_bytes);
}

static int novacap_video_worker(void *data)
{
    struct novacap *cap = data;

    while (!kthread_should_stop()) {
        struct novacap_buffer *buf = NULL;
        unsigned long flags;
        u32 frame_seq;
        u32 frame;
        void *dst;

        if (!cap->video_streaming) {
            msleep(5);
            continue;
        }

        frame_seq = ncap_read(cap, NCAP_FRAME_SEQ);
        if (frame_seq == cap->seen_frame_seq) {
            usleep_range(1000, 2000);
            continue;
        }

        spin_lock_irqsave(&cap->qlock, flags);
        if (!list_empty(&cap->queued)) {
            buf = list_first_entry(&cap->queued, struct novacap_buffer, list);
            list_del(&buf->list);
        }
        spin_unlock_irqrestore(&cap->qlock, flags);

        if (!buf) {
            usleep_range(1000, 2000);
            continue;
        }

        dst = vb2_plane_vaddr(&buf->vb.vb2_buf, 0);
        frame = ncap_read(cap, NCAP_LAST_FRAME);
        if (frame >= cap->frame_count)
            frame = 0;
        novacap_copy_frame(cap, dst, frame);

        buf->vb.sequence = cap->video_sequence++;
        buf->vb.field = V4L2_FIELD_NONE;
        buf->vb.vb2_buf.timestamp = ktime_get_ns();
        cap->seen_frame_seq = frame_seq;
        vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
    }

    return 0;
}

static int novacap_start_streaming(struct vb2_queue *vq, unsigned int count)
{
    struct novacap *cap = vb2_get_drv_priv(vq);

    cap->seen_frame_seq = ncap_read(cap, NCAP_FRAME_SEQ);
    cap->video_sequence = 0;
    cap->video_streaming = true;
    ncap_write(cap, NCAP_CONTROL, 1);

    cap->video_thread = kthread_run(novacap_video_worker, cap, "novacap-video");
    if (IS_ERR(cap->video_thread)) {
        int ret = PTR_ERR(cap->video_thread);
        cap->video_thread = NULL;
        cap->video_streaming = false;
        return ret;
    }

    return 0;
}

static void novacap_stop_streaming(struct vb2_queue *vq)
{
    struct novacap *cap = vb2_get_drv_priv(vq);
    struct novacap_buffer *buf;
    unsigned long flags;

    cap->video_streaming = false;
    if (cap->video_thread) {
        kthread_stop(cap->video_thread);
        cap->video_thread = NULL;
    }

    spin_lock_irqsave(&cap->qlock, flags);
    while (!list_empty(&cap->queued)) {
        buf = list_first_entry(&cap->queued, struct novacap_buffer, list);
        list_del(&buf->list);
        vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
    }
    spin_unlock_irqrestore(&cap->qlock, flags);
}

static const struct vb2_ops novacap_vb2_ops = {
    .queue_setup = novacap_queue_setup,
    .buf_prepare = novacap_buf_prepare,
    .buf_queue = novacap_buf_queue,
    .start_streaming = novacap_start_streaming,
    .stop_streaming = novacap_stop_streaming,
    .wait_prepare = vb2_ops_wait_prepare,
    .wait_finish = vb2_ops_wait_finish,
};

static int novacap_querycap(struct file *file, void *priv,
                            struct v4l2_capability *capability)
{
    strscpy(capability->driver, "novacap", sizeof(capability->driver));
    strscpy(capability->card, "NovaVM HDMI Capture", sizeof(capability->card));
    strscpy(capability->bus_info, "platform:novavm-capture", sizeof(capability->bus_info));
    return 0;
}

static int novacap_enum_fmt(struct file *file, void *priv,
                            struct v4l2_fmtdesc *f)
{
    if (f->index)
        return -EINVAL;
    f->pixelformat = V4L2_PIX_FMT_XBGR32;
    return 0;
}

static int novacap_g_fmt(struct file *file, void *priv, struct v4l2_format *f)
{
    struct novacap *cap = video_drvdata(file);

    f->fmt.pix.width = cap->width;
    f->fmt.pix.height = cap->height;
    f->fmt.pix.pixelformat = V4L2_PIX_FMT_XBGR32;
    f->fmt.pix.field = V4L2_FIELD_NONE;
    f->fmt.pix.bytesperline = cap->width * 4;
    f->fmt.pix.sizeimage = cap->width * cap->height * 4;
    f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
    return 0;
}

static int novacap_s_fmt(struct file *file, void *priv, struct v4l2_format *f)
{
    return novacap_g_fmt(file, priv, f);
}

static int novacap_enum_input(struct file *file, void *priv, struct v4l2_input *input)
{
    if (input->index)
        return -EINVAL;
    strscpy(input->name, "HDMI output", sizeof(input->name));
    input->type = V4L2_INPUT_TYPE_CAMERA;
    return 0;
}

static int novacap_g_input(struct file *file, void *priv, unsigned int *i)
{
    *i = 0;
    return 0;
}

static int novacap_s_input(struct file *file, void *priv, unsigned int i)
{
    return i ? -EINVAL : 0;
}

static const struct v4l2_ioctl_ops novacap_ioctl_ops = {
    .vidioc_querycap = novacap_querycap,
    .vidioc_enum_fmt_vid_cap = novacap_enum_fmt,
    .vidioc_g_fmt_vid_cap = novacap_g_fmt,
    .vidioc_s_fmt_vid_cap = novacap_s_fmt,
    .vidioc_try_fmt_vid_cap = novacap_s_fmt,
    .vidioc_enum_input = novacap_enum_input,
    .vidioc_g_input = novacap_g_input,
    .vidioc_s_input = novacap_s_input,
    .vidioc_reqbufs = vb2_ioctl_reqbufs,
    .vidioc_create_bufs = vb2_ioctl_create_bufs,
    .vidioc_querybuf = vb2_ioctl_querybuf,
    .vidioc_qbuf = vb2_ioctl_qbuf,
    .vidioc_dqbuf = vb2_ioctl_dqbuf,
    .vidioc_expbuf = vb2_ioctl_expbuf,
    .vidioc_streamon = vb2_ioctl_streamon,
    .vidioc_streamoff = vb2_ioctl_streamoff,
};

static const struct v4l2_file_operations novacap_fops = {
    .owner = THIS_MODULE,
    .open = v4l2_fh_open,
    .release = vb2_fop_release,
    .read = vb2_fop_read,
    .poll = vb2_fop_poll,
    .unlocked_ioctl = video_ioctl2,
    .mmap = vb2_fop_mmap,
};

static const struct snd_pcm_hardware novacap_pcm_hw = {
    .info = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED |
            SNDRV_PCM_INFO_BLOCK_TRANSFER | SNDRV_PCM_INFO_MMAP_VALID,
    .formats = SNDRV_PCM_FMTBIT_S16_LE,
    .rates = SNDRV_PCM_RATE_48000,
    .rate_min = 48000,
    .rate_max = 48000,
    .channels_min = 2,
    .channels_max = 2,
    .buffer_bytes_max = 262144,
    .period_bytes_min = 1024,
    .period_bytes_max = 65536,
    .periods_min = 2,
    .periods_max = 128,
};

static void novacap_copy_audio(struct novacap *cap, void *dst, size_t bytes)
{
    size_t first;
    void __iomem *src = cap->mem + cap->audio_mem_offset + cap->audio_read_offset;

    if (cap->audio_read_offset + bytes <= cap->audio_bytes) {
        memcpy_fromio(dst, src, bytes);
    } else {
        first = cap->audio_bytes - cap->audio_read_offset;
        memcpy_fromio(dst, src, first);
        memcpy_fromio(dst + first, cap->mem + cap->audio_mem_offset, bytes - first);
    }

    cap->audio_read_offset += bytes;
    if (cap->audio_read_offset >= cap->audio_bytes)
        cap->audio_read_offset -= cap->audio_bytes;
}

static int novacap_audio_worker(void *data)
{
    struct novacap *cap = data;

    while (!kthread_should_stop()) {
        struct snd_pcm_substream *substream = cap->substream;
        struct snd_pcm_runtime *runtime;
        size_t period_bytes;
        size_t buffer_bytes;
        unsigned int usec;
        void *dst;

        if (!cap->audio_running || !substream || !substream->runtime) {
            msleep(5);
            continue;
        }

        runtime = substream->runtime;
        period_bytes = runtime->period_size * NOVACAP_AUDIO_FRAME_BYTES;
        buffer_bytes = runtime->buffer_size * NOVACAP_AUDIO_FRAME_BYTES;
        if (!runtime->dma_area || !period_bytes || !buffer_bytes) {
            msleep(5);
            continue;
        }

        if (cap->audio_hw_ptr + period_bytes > buffer_bytes) {
            size_t first = buffer_bytes - cap->audio_hw_ptr;
            dst = runtime->dma_area + cap->audio_hw_ptr;
            novacap_copy_audio(cap, dst, first);
            novacap_copy_audio(cap, runtime->dma_area, period_bytes - first);
        } else {
            dst = runtime->dma_area + cap->audio_hw_ptr;
            novacap_copy_audio(cap, dst, period_bytes);
        }

        cap->audio_hw_ptr += period_bytes;
        if (cap->audio_hw_ptr >= buffer_bytes)
            cap->audio_hw_ptr -= buffer_bytes;
        snd_pcm_period_elapsed(substream);
        usec = (unsigned int)div_u64((u64)runtime->period_size * 1000000ull, runtime->rate);
        usleep_range(usec, usec + 1000);
    }

    return 0;
}

static int novacap_pcm_open(struct snd_pcm_substream *substream)
{
    struct novacap *cap = snd_pcm_substream_chip(substream);

    substream->runtime->hw = novacap_pcm_hw;
    cap->substream = substream;
    return 0;
}

static int novacap_pcm_close(struct snd_pcm_substream *substream)
{
    struct novacap *cap = snd_pcm_substream_chip(substream);

    cap->audio_running = false;
    if (cap->audio_thread) {
        kthread_stop(cap->audio_thread);
        cap->audio_thread = NULL;
    }
    cap->substream = NULL;
    return 0;
}

static int novacap_pcm_hw_params(struct snd_pcm_substream *substream,
                                 struct snd_pcm_hw_params *params)
{
    return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(params));
}

static int novacap_pcm_hw_free(struct snd_pcm_substream *substream)
{
    return snd_pcm_lib_free_pages(substream);
}

static int novacap_pcm_prepare(struct snd_pcm_substream *substream)
{
    struct novacap *cap = snd_pcm_substream_chip(substream);

    cap->audio_hw_ptr = 0;
    cap->audio_read_offset = ncap_read(cap, NCAP_AUDIO_OFFSET);
    if (cap->audio_read_offset >= cap->audio_bytes)
        cap->audio_read_offset = 0;
    if (substream->runtime->dma_area)
        memset(substream->runtime->dma_area, 0, substream->runtime->buffer_size * NOVACAP_AUDIO_FRAME_BYTES);
    return 0;
}

static int novacap_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
    struct novacap *cap = snd_pcm_substream_chip(substream);

    switch (cmd) {
    case SNDRV_PCM_TRIGGER_START:
    case SNDRV_PCM_TRIGGER_RESUME:
        cap->audio_running = true;
        if (!cap->audio_thread) {
            cap->audio_thread = kthread_run(novacap_audio_worker, cap, "novacap-audio");
            if (IS_ERR(cap->audio_thread)) {
                int ret = PTR_ERR(cap->audio_thread);
                cap->audio_thread = NULL;
                cap->audio_running = false;
                return ret;
            }
        }
        return 0;
    case SNDRV_PCM_TRIGGER_STOP:
    case SNDRV_PCM_TRIGGER_SUSPEND:
        cap->audio_running = false;
        if (cap->audio_thread) {
            kthread_stop(cap->audio_thread);
            cap->audio_thread = NULL;
        }
        return 0;
    default:
        return -EINVAL;
    }
}

static snd_pcm_uframes_t novacap_pcm_pointer(struct snd_pcm_substream *substream)
{
    struct novacap *cap = snd_pcm_substream_chip(substream);

    return cap->audio_hw_ptr / NOVACAP_AUDIO_FRAME_BYTES;
}

static const struct snd_pcm_ops novacap_pcm_ops = {
    .open = novacap_pcm_open,
    .close = novacap_pcm_close,
    .hw_params = novacap_pcm_hw_params,
    .hw_free = novacap_pcm_hw_free,
    .prepare = novacap_pcm_prepare,
    .trigger = novacap_pcm_trigger,
    .pointer = novacap_pcm_pointer,
};

static int novacap_register_video(struct novacap *cap)
{
    struct vb2_queue *q = &cap->queue;
    int ret;

    ret = v4l2_device_register(cap->dev, &cap->v4l2_dev);
    if (ret)
        return ret;

    q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_READ;
    q->drv_priv = cap;
    q->buf_struct_size = sizeof(struct novacap_buffer);
    q->ops = &novacap_vb2_ops;
    q->mem_ops = &vb2_vmalloc_memops;
    q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
    q->lock = &cap->vlock;
    q->dev = cap->dev;

    ret = vb2_queue_init(q);
    if (ret)
        goto fail_v4l2;

    strscpy(cap->vdev.name, "NovaVM HDMI Capture", sizeof(cap->vdev.name));
    cap->vdev.v4l2_dev = &cap->v4l2_dev;
    cap->vdev.fops = &novacap_fops;
    cap->vdev.ioctl_ops = &novacap_ioctl_ops;
    cap->vdev.release = video_device_release_empty;
    cap->vdev.lock = &cap->vlock;
    cap->vdev.queue = q;
    cap->vdev.device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
    video_set_drvdata(&cap->vdev, cap);

    ret = video_register_device(&cap->vdev, VFL_TYPE_VIDEO, -1);
    if (ret)
        goto fail_v4l2;

    return 0;

fail_v4l2:
    v4l2_device_unregister(&cap->v4l2_dev);
    return ret;
}

static int novacap_register_audio(struct novacap *cap)
{
    int ret;

    ret = snd_card_new(cap->dev, -1, "NovaVM", THIS_MODULE, 0, &cap->card);
    if (ret)
        return ret;

    ret = snd_pcm_new(cap->card, "NovaVM Capture", 0, 0, 1, &cap->pcm);
    if (ret)
        goto fail_card;

    cap->pcm->private_data = cap;
    strscpy(cap->pcm->name, "NovaVM HDMI Capture", sizeof(cap->pcm->name));
    snd_pcm_set_ops(cap->pcm, SNDRV_PCM_STREAM_CAPTURE, &novacap_pcm_ops);
    snd_pcm_lib_preallocate_pages_for_all(cap->pcm, SNDRV_DMA_TYPE_VMALLOC, NULL, 65536, 262144);

    strscpy(cap->card->driver, "NovaVM", sizeof(cap->card->driver));
    strscpy(cap->card->shortname, "NovaVM", sizeof(cap->card->shortname));
    strscpy(cap->card->longname, "NovaVM HDMI audio capture", sizeof(cap->card->longname));

    ret = snd_card_register(cap->card);
    if (ret)
        goto fail_card;

    return 0;

fail_card:
    snd_card_free(cap->card);
    cap->card = NULL;
    return ret;
}

static int novacap_probe(struct platform_device *pdev)
{
    struct novacap *cap;
    struct resource mem_res;
    struct device_node *mem_np;
    u32 magic;
    int ret;

    cap = devm_kzalloc(&pdev->dev, sizeof(*cap), GFP_KERNEL);
    if (!cap)
        return -ENOMEM;

    cap->dev = &pdev->dev;
    mutex_init(&cap->vlock);
    spin_lock_init(&cap->qlock);
    INIT_LIST_HEAD(&cap->queued);

    cap->regs = devm_platform_ioremap_resource(pdev, 0);
    if (IS_ERR(cap->regs))
        return PTR_ERR(cap->regs);

    mem_np = of_parse_phandle(pdev->dev.of_node, "memory-region", 0);
    if (!mem_np)
        return dev_err_probe(&pdev->dev, -ENODEV, "missing memory-region\n");
    ret = of_address_to_resource(mem_np, 0, &mem_res);
    of_node_put(mem_np);
    if (ret)
        return dev_err_probe(&pdev->dev, ret, "bad memory-region\n");

    cap->mem_size = resource_size(&mem_res);
    cap->mem = devm_ioremap(&pdev->dev, mem_res.start, cap->mem_size);
    if (!cap->mem)
        return -ENOMEM;

    magic = ncap_read(cap, 0);
    if (magic != NCAP_MAGIC)
        return dev_err_probe(&pdev->dev, -ENODEV, "capture magic mismatch: %08x\n", magic);

    cap->width = ncap_read(cap, NCAP_WIDTH);
    cap->height = ncap_read(cap, NCAP_HEIGHT);
    cap->frame_stride = ncap_read(cap, NCAP_FRAME_STRIDE);
    cap->frame_count = ncap_read(cap, NCAP_FRAME_COUNT);
    cap->video_base = ncap_read(cap, NCAP_VIDEO_BASE);
    cap->audio_base = ncap_read(cap, NCAP_AUDIO_BASE);
    cap->audio_bytes = ncap_read(cap, NCAP_AUDIO_BYTES);
    cap->audio_mem_offset = cap->audio_base - cap->video_base;

    if (!cap->width || !cap->height || !cap->frame_stride || !cap->frame_count ||
        cap->frame_stride * cap->frame_count > cap->mem_size ||
        cap->audio_mem_offset + cap->audio_bytes > cap->mem_size)
        return dev_err_probe(&pdev->dev, -EINVAL, "invalid capture geometry\n");

    ret = novacap_register_video(cap);
    if (ret)
        return ret;

    ret = novacap_register_audio(cap);
    if (ret) {
        video_unregister_device(&cap->vdev);
        v4l2_device_unregister(&cap->v4l2_dev);
        return ret;
    }

    platform_set_drvdata(pdev, cap);
    dev_info(&pdev->dev, "NovaVM capture: %ux%u video, %u-byte audio ring\n",
             cap->width, cap->height, cap->audio_bytes);
    return 0;
}

static int novacap_remove(struct platform_device *pdev)
{
    struct novacap *cap = platform_get_drvdata(pdev);

    if (cap->audio_thread)
        kthread_stop(cap->audio_thread);
    if (cap->card)
        snd_card_free(cap->card);
    video_unregister_device(&cap->vdev);
    v4l2_device_unregister(&cap->v4l2_dev);
    return 0;
}

static const struct of_device_id novacap_of_match[] = {
    { .compatible = "novavm,capture" },
    { }
};
MODULE_DEVICE_TABLE(of, novacap_of_match);

static struct platform_driver novacap_driver = {
    .probe = novacap_probe,
    .remove = novacap_remove,
    .driver = {
        .name = "novacap",
        .of_match_table = novacap_of_match,
    },
};
module_platform_driver(novacap_driver);

MODULE_AUTHOR("NovaVM");
MODULE_DESCRIPTION("NovaVM standard V4L2/ALSA capture driver");
MODULE_LICENSE("GPL");
