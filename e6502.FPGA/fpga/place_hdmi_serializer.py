# Coarse physical plan for the ULX3S build.
#
# This script runs after nextpnr packs cells and before placement. The goal is
# not to hand-place the design. The goal is to give each board-level subsystem a
# physical neighborhood, then tightly constrain only the small IO-adjacent
# timing islands that actually need it.
#
# Yosys/ABC may give packed FF/LUT cells auto-generated names, so classification
# checks both the packed cell name and the net names attached to its ports.

REGIONS = [
    # gpdi_dp[2] red+ is at X76/Y0.
    {
        "name": "hdmi_ser_red",
        "box": (70, 0, 84, 8),
        "enforce": True,
        "match": ("lattice_ecp5_shift[2]", "load_channel[2]", "tmds_shift[2]", "tmds_pair[2]"),
    },
    # gpdi_dp[1] green+ is at X83/Y0.
    {
        "name": "hdmi_ser_green",
        "box": (78, 0, 92, 8),
        "enforce": True,
        "match": ("lattice_ecp5_shift[1]", "load_channel[1]", "tmds_shift[1]", "tmds_pair[1]"),
    },
    # gpdi_dp[0] blue+ is at X110/Y0.
    {
        "name": "hdmi_ser_blue",
        "box": (104, 0, 118, 8),
        "enforce": True,
        "match": ("lattice_ecp5_shift[0]", "load_channel[0]", "tmds_shift[0]", "tmds_pair[0]"),
    },
    # gpdi_dp[3] clock+ is at X116/Y0.
    {
        "name": "hdmi_ser_clock",
        "box": (112, 0, 124, 8),
        "enforce": True,
        "match": ("ddr_tmds_clock", "load_clock", "tmds_clock_shift", "tmds_clock_pair"),
    },
    # Shared pixel-to-shift-domain synchronizer and edge detector.
    {
        "name": "hdmi_ser_control",
        "box": (88, 1, 112, 8),
        "enforce": True,
        "match": ("tmds_control", "tmds_control_synchronizer_chain", "load_edge"),
    },

    # Top-band HDMI encoder/packet logic. The serializer cells above remain
    # tightly constrained; this region only keeps the pixel/audio HDMI logic
    # close enough to GPDI without forcing exact locations.
    {
        "name": "hdmi_pixel",
        "box": (52, 1, 124, 32),
        "enforce": False,
        "match": ("hdmi_inst", "\\hdmi_inst", "packet_picker", "packet_assembler"),
    },

    # SDRAM lives on the east edge. Keep the controller, CDC shim, and XRAM/SID
    # SDRAM clients near that edge so 100 MHz SDRAM paths do not cross the chip.
    {
        "name": "sdram_edge",
        "box": (94, 8, 126, 95),
        "enforce": False,
        "match": ("sdram_inst", "\\sdram_inst", "dbg_sdram_port_b_cdc", "xram_sdram_inst", "curve_reader_inst"),
    },

    # ESP/FTDI pins are on the west edge. Keep the byte transports and host
    # bridge on that side, with enough vertical space for the FIFOs.
    {
        "name": "host_io",
        "box": (1, 28, 48, 95),
        "enforce": False,
        "match": ("dbg_spi", "\\dbg_spi", "uart_inst", "\\uart_inst", "dbg_bridge", "\\dbg_bridge", "debug_bridge", "fio_inst", "nic_inst"),
    },

    # VGC analysis regions. These are not enforced yet; they show how the giant
    # VGC block should be split before we tighten its placement.
    {
        "name": "vgc_timing",
        "box": (50, 16, 72, 30),
        "enforce": False,
        "match": ("timing_inst", "\\timing_inst"),
    },
    {
        "name": "vgc_text",
        "box": (42, 26, 78, 58),
        "enforce": False,
        "match": ("text_inst", "\\text_inst", "char_mem", "color_mem", "attr_mem", "font_mem"),
    },
    {
        "name": "vgc_gfx_artist",
        "box": (62, 32, 106, 72),
        "enforce": False,
        "match": ("gfx_inst", "\\gfx_inst", "artist_inst", "\\artist_inst", "gfx_mem"),
    },
    {
        "name": "vgc_sprites",
        "box": (40, 54, 96, 86),
        "enforce": False,
        "match": ("sprite_inst", "\\sprite_inst", "spr_mem0", "spr_mem1", "slb_ram"),
    },
    {
        "name": "vgc_copper",
        "box": (74, 18, 104, 42),
        "enforce": False,
        "match": ("copper_inst", "\\copper_inst", "copper_list_mem"),
    },
    {
        "name": "vgc_io_regs",
        "box": (30, 28, 78, 82),
        "enforce": False,
        "match": ("core.vgc_inst", "core.\\vgc_inst", "\\vgc_inst", "vgc_inst", "key_fifo_inst", "key_data_xlat"),
    },

    # Whole-chip analysis regions. These are intentionally non-enforced while
    # we collect sizing data.
    {
        "name": "vgc_video",
        "box": (42, 14, 112, 82),
        "enforce": False,
        "match": (),
    },

    # The 6502, main RAM, ROMs, and bus decode are central so they can reach
    # VGC, DMA/blitter, NIC, and SDRAM clients without one long dominant route.
    {
        "name": "cpu_mem",
        "box": (12, 18, 76, 92),
        "enforce": False,
        "match": ("core.cpu_inst", "core.\\cpu_inst", "\\cpu_inst", "cpu_inst", "main_ram", "basic_rom_inst", "ext_rom_inst", "xmc_regs"),
    },

    # DMA and blitter sit between CPU/RAM, VGC, and XRAM.
    {
        "name": "bus_masters",
        "box": (28, 40, 86, 92),
        "enforce": False,
        "match": ("blt_inst", "\\blt_inst", "dma_inst", "\\dma_inst"),
    },

    # SID is mostly audio-side compute. Keep it out of the SDRAM/HDMI pin lanes
    # but still near the HDMI audio bridge.
    {
        "name": "sid_audio",
        "box": (8, 1, 82, 44),
        "enforce": False,
        "match": ("sid_inst", "\\sid_inst", "sid_hdmi_audio_inst"),
    },
]


def cell_net_names(cell):
    names = []
    for _, port in cell.ports:
        try:
            net = port.net
        except Exception:
            continue
        if net is None:
            continue
        try:
            names.append(net.name)
        except Exception:
            pass
    return names


def matches_any(value, needles):
    return any(needle in value for needle in needles)


def classify_cell(cell_name, net_names):
    # Regions are ordered from tight timing islands to broad subsystem homes.
    text = [cell_name] + net_names
    for region in REGIONS:
        if any(matches_any(value, region["match"]) for value in text):
            return region["name"]
    return None


def region_contains(region, loc):
    x0, y0, x1, y1 = region["box"]
    return x0 <= loc.x <= x1 and y0 <= loc.y <= y1


def cell_bucket(cell):
    try:
        return str(ctx.getBelBucketForCellType(cell.type))
    except Exception:
        return "cell_type:" + str(cell.type)


def count_region_capacity(region_names):
    capacity = {name: {} for name in region_names}
    for bel in ctx.getBels():
        try:
            loc = ctx.getBelLocation(bel)
            bucket = str(ctx.getBelBucketForBel(bel))
        except Exception:
            continue
        for region in REGIONS:
            name = region["name"]
            if region_contains(region, loc):
                capacity[name][bucket] = capacity[name].get(bucket, 0) + 1
    return capacity


for region in REGIONS:
    name = region["name"]
    x0, y0, x1, y1 = region["box"]
    ctx.createRectangularRegion(name, x0, y0, x1, y1)

region_names = [region["name"] for region in REGIONS]
counts = {name: 0 for name in region_names}
bucket_demand = {name: {} for name in region_names}
type_demand = {name: {} for name in region_names}
for cell_name, cell in ctx.cells:
    cell_name = str(cell_name)
    region_name = classify_cell(cell_name, cell_net_names(cell))
    if region_name is None:
        continue
    counts[region_name] += 1
    bucket = cell_bucket(cell)
    cell_type = str(cell.type)
    bucket_demand[region_name][bucket] = bucket_demand[region_name].get(bucket, 0) + 1
    type_demand[region_name][cell_type] = type_demand[region_name].get(cell_type, 0) + 1
    if next(region for region in REGIONS if region["name"] == region_name)["enforce"]:
        ctx.constrainCellToRegion(cell_name, region_name)

print("Nova floorplan constraints: " + ", ".join("%s=%d" % item for item in sorted(counts.items())))

capacity = count_region_capacity(region_names)
for name in region_names:
    if counts[name] == 0:
        continue
    bucket_parts = []
    for bucket, demand in sorted(bucket_demand[name].items()):
        cap = capacity[name].get(bucket, 0)
        spare = cap - demand
        used = 100.0 * demand / cap if cap else 0.0
        bucket_parts.append("%s=%d/%d %.1f%% spare=%d" % (bucket, demand, cap, used, spare))

    top_types = sorted(type_demand[name].items(), key=lambda item: (-item[1], item[0]))[:4]
    type_parts = ["%s=%d" % item for item in top_types]
    print("Nova floorplan region %s: cells=%d; buckets: %s; top cell types: %s" %
          (name, counts[name], ", ".join(bucket_parts), ", ".join(type_parts)))
