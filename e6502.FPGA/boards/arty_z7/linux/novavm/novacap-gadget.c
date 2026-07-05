#define _XOPEN_SOURCE 700

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

#define CFGFS "/sys/kernel/config"
#define GADGET CFGFS "/usb_gadget/novavm"

static int write_text(const char *path, const char *text)
{
    int fd = open(path, O_WRONLY | O_TRUNC);
    ssize_t len = (ssize_t)strlen(text);
    ssize_t wr;

    if (fd < 0)
        return -errno;
    wr = write(fd, text, len);
    close(fd);
    return wr == len ? 0 : -EIO;
}

static int mkdir_one(const char *path)
{
    if (mkdir(path, 0755) == 0 || errno == EEXIST)
        return 0;
    return -errno;
}

static int mkdir_p(const char *path)
{
    char tmp[512];
    char *p;

    if (strlen(path) >= sizeof(tmp))
        return -ENAMETOOLONG;
    strcpy(tmp, path);

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if (mkdir_one(tmp) < 0)
                return -errno;
            *p = '/';
        }
    }
    return mkdir_one(tmp);
}

static int link_one(const char *target, const char *linkpath)
{
    if (symlink(target, linkpath) == 0 || errno == EEXIST)
        return 0;
    return -errno;
}

static int first_udc(char *buf, size_t len)
{
    DIR *dir = opendir("/sys/class/udc");
    struct dirent *ent;

    if (!dir)
        return -errno;
    while ((ent = readdir(dir))) {
        if (ent->d_name[0] == '.')
            continue;
        if (strlen(ent->d_name) >= len) {
            closedir(dir);
            return -ENAMETOOLONG;
        }
        strcpy(buf, ent->d_name);
        closedir(dir);
        return 0;
    }
    closedir(dir);
    return -ENODEV;
}

static int ensure_configfs(void)
{
    struct stat st;

    if (stat(CFGFS "/usb_gadget", &st) == 0)
        return 0;
    if (mount("configfs", CFGFS, "configfs", 0, NULL) == 0 || errno == EBUSY)
        return 0;
    return -errno;
}

static int setup_uvc(void)
{
    int ret;

    ret = mkdir_p(GADGET "/functions/uvc.0/control/header/h");
    if (ret < 0) return ret;
    ret = mkdir_p(GADGET "/functions/uvc.0/control/class/fs");
    if (ret < 0) return ret;
    ret = mkdir_p(GADGET "/functions/uvc.0/control/class/hs");
    if (ret < 0) return ret;

    ret = mkdir_p(GADGET "/functions/uvc.0/streaming/uncompressed/u/720p");
    if (ret < 0) return ret;
    write_text(GADGET "/functions/uvc.0/streaming/uncompressed/u/720p/wWidth", "720\n");
    write_text(GADGET "/functions/uvc.0/streaming/uncompressed/u/720p/wHeight", "480\n");
    write_text(GADGET "/functions/uvc.0/streaming/uncompressed/u/720p/dwMinBitRate", "165888000\n");
    write_text(GADGET "/functions/uvc.0/streaming/uncompressed/u/720p/dwMaxBitRate", "165888000\n");
    write_text(GADGET "/functions/uvc.0/streaming/uncompressed/u/720p/dwMaxVideoFrameBufferSize", "691200\n");
    write_text(GADGET "/functions/uvc.0/streaming/uncompressed/u/720p/dwDefaultFrameInterval", "333667\n");
    write_text(GADGET "/functions/uvc.0/streaming/uncompressed/u/720p/dwFrameInterval", "333667\n");

    ret = mkdir_p(GADGET "/functions/uvc.0/streaming/header/h");
    if (ret < 0) return ret;
    ret = mkdir_p(GADGET "/functions/uvc.0/streaming/class/fs");
    if (ret < 0) return ret;
    ret = mkdir_p(GADGET "/functions/uvc.0/streaming/class/hs");
    if (ret < 0) return ret;

    ret = link_one(GADGET "/functions/uvc.0/control/header/h",
                   GADGET "/functions/uvc.0/control/class/fs/h");
    if (ret < 0) return ret;
    ret = link_one(GADGET "/functions/uvc.0/control/header/h",
                   GADGET "/functions/uvc.0/control/class/hs/h");
    if (ret < 0) return ret;
    ret = link_one(GADGET "/functions/uvc.0/streaming/uncompressed/u",
                   GADGET "/functions/uvc.0/streaming/header/h/u");
    if (ret < 0) return ret;
    ret = link_one(GADGET "/functions/uvc.0/streaming/header/h",
                   GADGET "/functions/uvc.0/streaming/class/fs/h");
    if (ret < 0) return ret;
    return link_one(GADGET "/functions/uvc.0/streaming/header/h",
                    GADGET "/functions/uvc.0/streaming/class/hs/h");
}

static int setup_uac2(void)
{
    int ret = mkdir_p(GADGET "/functions/uac2.0");
    if (ret < 0)
        return ret;
    write_text(GADGET "/functions/uac2.0/c_chmask", "3\n");
    write_text(GADGET "/functions/uac2.0/c_srate", "48000\n");
    write_text(GADGET "/functions/uac2.0/c_ssize", "2\n");
    write_text(GADGET "/functions/uac2.0/p_chmask", "0\n");
    return 0;
}

static int enable_gadget(void)
{
    char udc[128];
    int ret;

    ret = ensure_configfs();
    if (ret < 0)
        return ret;

    ret = mkdir_p(GADGET "/strings/0x409");
    if (ret < 0) return ret;
    ret = mkdir_p(GADGET "/configs/c.1/strings/0x409");
    if (ret < 0) return ret;

    write_text(GADGET "/idVendor", "0x1209\n");
    write_text(GADGET "/idProduct", "0x4e56\n");
    write_text(GADGET "/bcdDevice", "0x0001\n");
    write_text(GADGET "/bcdUSB", "0x0200\n");
    write_text(GADGET "/strings/0x409/serialnumber", "novavm\n");
    write_text(GADGET "/strings/0x409/manufacturer", "NovaVM\n");
    write_text(GADGET "/strings/0x409/product", "NovaVM Capture\n");
    write_text(GADGET "/configs/c.1/strings/0x409/configuration", "UVC/UAC2 Capture\n");
    write_text(GADGET "/configs/c.1/MaxPower", "250\n");

    ret = setup_uvc();
    if (ret < 0) return ret;
    ret = setup_uac2();
    if (ret < 0) return ret;
    ret = link_one(GADGET "/functions/uvc.0", GADGET "/configs/c.1/uvc.0");
    if (ret < 0) return ret;
    ret = link_one(GADGET "/functions/uac2.0", GADGET "/configs/c.1/uac2.0");
    if (ret < 0) return ret;

    ret = first_udc(udc, sizeof(udc));
    if (ret < 0)
        return ret;
    return write_text(GADGET "/UDC", udc);
}

static int unlink_cb(const char *path, const struct stat *st, int type, struct FTW *ftw)
{
    (void)st;
    (void)type;
    (void)ftw;
    if (strcmp(path, GADGET) == 0)
        return 0;
    return remove(path);
}

static int disable_gadget(void)
{
    struct stat st;

    if (stat(GADGET, &st) != 0)
        return errno == ENOENT ? 0 : -errno;
    write_text(GADGET "/UDC", "\n");
    if (nftw(GADGET, unlink_cb, 32, FTW_DEPTH | FTW_PHYS) != 0)
        return -errno;
    return rmdir(GADGET) == 0 ? 0 : -errno;
}

static int status_gadget(void)
{
    char buf[128] = {0};
    int fd = open(GADGET "/UDC", O_RDONLY);
    ssize_t n;

    if (fd < 0) {
        puts("disabled");
        return 0;
    }
    n = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    if (n > 0 && buf[0] != '\n')
        printf("enabled: %s", buf);
    else
        puts("configured: unbound");
    return 0;
}

int main(int argc, char **argv)
{
    int ret;

    if (argc != 2) {
        fprintf(stderr, "Usage: novacap-gadget enable|disable|status\n");
        return 2;
    }

    if (strcmp(argv[1], "enable") == 0)
        ret = enable_gadget();
    else if (strcmp(argv[1], "disable") == 0)
        ret = disable_gadget();
    else if (strcmp(argv[1], "status") == 0)
        ret = status_gadget();
    else {
        fprintf(stderr, "Usage: novacap-gadget enable|disable|status\n");
        return 2;
    }

    if (ret < 0) {
        fprintf(stderr, "novacap-gadget: %s\n", strerror(-ret));
        return 1;
    }
    return 0;
}
