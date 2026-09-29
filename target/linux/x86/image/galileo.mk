# SPDX-License-Identifier: GPL-2.0-only

# Use the EFI firmware console for the PCI UART, as the generic GRUB
# "serial --unit=0" command addresses a legacy ISA UART instead.
GRUB_SERIAL_CONFIG :=
GRUB_TERMINAL_CONFIG := terminal_input console; terminal_output console

define Device/galileo
  DEVICE_VENDOR := Intel
  DEVICE_MODEL := Galileo Gen 2
  GRUB2_VARIANT := legacy
endef
TARGET_DEVICES += galileo
