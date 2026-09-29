BOARDNAME:=Intel Galileo (Quark X1000)

CPU_TYPE:=lakemont
# GCC's bare Lakemont profile disables x87; Quark X1000 has an x87 FPU.
CPU_CFLAGS_lakemont:=-march=lakemont -m80387

FEATURES += pci pcie usb usbgadget gpio
FEATURES := $(filter-out pcmcia,$(FEATURES))

define Target/Description
	Build firmware images for Intel Galileo boards based on
	the Intel Quark X1000 SoC.
endef
