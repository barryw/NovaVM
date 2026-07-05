SUMMARY = "NovaVM standard HDMI/audio capture driver"
DESCRIPTION = "Registers the novavm,capture PL block as V4L2 video capture and ALSA PCM capture devices."
LICENSE = "CLOSED"

inherit module

SRC_URI = "file://Makefile \
           file://novacap.c"

S = "${WORKDIR}"

RPROVIDES:${PN} += "kernel-module-novacap"
KERNEL_MODULE_AUTOLOAD += "novacap"
