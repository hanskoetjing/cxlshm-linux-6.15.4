# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the Linux kernel device drivers.
#
# 15 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
#

obj-y				+= cache/
obj-y				+= irqchip/
obj-y				+= bus/

obj-$(CONFIG_GENERIC_PHY)	+= phy/

# GPIO must come after pinctrl as gpios may need to mux pins etc
obj-$(CONFIG_PINCTRL)		+= pinctrl/
obj-$(CONFIG_GPIOLIB)		+= gpio/
obj-y				+= pwm/

# LEDs must come before PCI, it is needed by NPEM driver
obj-y				+= leds/

obj-y				+= pci/

obj-$(CONFIG_PARISC)		+= parisc/
obj-$(CONFIG_RAPIDIO)		+= rapidio/
obj-y				+= video/
obj-y				+= idle/

# IPMI must come before ACPI in order to provide IPMI opregion support
obj-y				+= char/ipmi/

obj-$(CONFIG_ACPI)		+= acpi/

# PnP must come after ACPI since it will eventually need to check if acpi
# was used and do nothing if so
obj-$(CONFIG_PNP)		+= pnp/
obj-y				+= amba/

obj-y				+= clk/
# Many drivers will want to use DMA so this has to be made available
# really early.
obj-$(CONFIG_DMADEVICES)	+= dma/

# SOC specific infrastructure drivers.
obj-y				+= soc/
obj-$(CONFIG_PM_GENERIC_DOMAINS)	+= pmdomain/

obj-y				+= virtio/
obj-$(CONFIG_VDPA)		+= vdpa/
obj-$(CONFIG_XEN)		+= xen/

# regulators early, since some subsystems rely on them to initialize
obj-$(CONFIG_REGULATOR)		+= regulator/

# reset controllers early, since gpu drivers might rely on them to initialize
obj-$(CONFIG_RESET_CONTROLLER)	+= reset/

# tty/ comes before char/ so that the VT console is the boot-time
# default.
obj-y				+= tty/
obj-y				+= char/

# iommu/ comes before gpu as gpu are using iommu controllers
obj-y				+= iommu/

# gpu/ comes after char for AGP vs DRM startup and after iommu
obj-y				+= gpu/

obj-$(CONFIG_CONNECTOR)		+= connector/

# i810fb depends on char/agp/
obj-$(CONFIG_FB_I810)           += video/fbdev/i810/

obj-$(CONFIG_PARPORT)		+= parport/
obj-y				+= base/ block/ misc/ mfd/ nfc/
obj-$(CONFIG_LIBNVDIMM)		+= nvdimm/
obj-y				+= dax/
obj-$(CONFIG_DMA_SHARED_BUFFER) += dma-buf/
obj-$(CONFIG_NUBUS)		+= nubus/
obj-y				+= cxl/
obj-y				+= macintosh/
obj-y				+= scsi/
obj-y				+= nvme/
obj-$(CONFIG_ATA)		+= ata/
obj-$(CONFIG_TARGET_CORE)	+= target/
obj-$(CONFIG_MTD)		+= mtd/
obj-$(CONFIG_SPI)		+= spi/
obj-$(CONFIG_SPMI)		+= spmi/
obj-$(CONFIG_HSI)		+= hsi/
obj-$(CONFIG_SLIMBUS)		+= slimbus/
obj-y				+= net/
obj-$(CONFIG_ATM)		+= atm/
obj-$(CONFIG_FUSION)		+= message/
obj-y				+= firewire/
obj-$(CONFIG_UIO)		+= uio/
obj-$(CONFIG_VFIO)		+= vfio/
obj-y				+= cdrom/
obj-y				+= auxdisplay/
obj-$(CONFIG_PCCARD)		+= pcmcia/
obj-$(CONFIG_DIO)		+= dio/
obj-$(CONFIG_SBUS)		+= sbus/
obj-$(CONFIG_ZORRO)		+= zorro/
obj-$(CONFIG_ATA_OVER_ETH)	+= block/aoe/
obj-$(CONFIG_TC)		+= tc/
obj-$(CONFIG_USB_PHY)		+= usb/
obj-$(CONFIG_USB)		+= usb/
obj-$(CONFIG_USB_SUPPORT)	+= usb/
obj-$(CONFIG_PCI)		+= usb/
obj-$(CONFIG_USB_GADGET)	+= usb/
obj-$(CONFIG_OF)		+= usb/
obj-$(CONFIG_SERIO)		+= input/serio/
obj-$(CONFIG_GAMEPORT)		+= input/gameport/
obj-$(CONFIG_INPUT)		+= input/
obj-$(CONFIG_RTC_LIB)		+= rtc/
obj-y				+= i2c/ i3c/ media/
obj-$(CONFIG_PPS)		+= pps/
obj-y				+= ptp/
obj-$(CONFIG_W1)		+= w1/
obj-y				+= power/
obj-$(CONFIG_HWMON)		+= hwmon/
obj-$(CONFIG_THERMAL)		+= thermal/
obj-$(CONFIG_WATCHDOG)		+= watchdog/
obj-$(CONFIG_MD)		+= md/
obj-$(CONFIG_BT)		+= bluetooth/
obj-$(CONFIG_ACCESSIBILITY)	+= accessibility/
obj-$(CONFIG_ISDN)		+= isdn/
obj-$(CONFIG_EDAC)		+= edac/
obj-$(CONFIG_EISA)		+= eisa/
obj-$(CONFIG_PM_OPP)		+= opp/
obj-$(CONFIG_CPU_FREQ)		+= cpufreq/
obj-$(CONFIG_CPU_IDLE)		+= cpuidle/
obj-y				+= mmc/
obj-y				+= ufs/
obj-$(CONFIG_MEMSTICK)		+= memstick/
obj-$(CONFIG_INFINIBAND)	+= infiniband/
obj-y				+= firmware/
obj-$(CONFIG_FWCTL)		+= fwctl/
obj-$(CONFIG_CRYPTO)		+= crypto/
obj-$(CONFIG_SUPERH)		+= sh/
obj-y				+= clocksource/
obj-$(CONFIG_DCA)		+= dca/
obj-$(CONFIG_HID_SUPPORT)	+= hid/
obj-$(CONFIG_PPC_PS3)		+= ps3/
obj-$(CONFIG_OF)		+= of/
obj-$(CONFIG_SSB)		+= ssb/
obj-$(CONFIG_BCMA)		+= bcma/
obj-$(CONFIG_VHOST_RING)	+= vhost/
obj-$(CONFIG_VHOST_IOTLB)	+= vhost/
obj-$(CONFIG_VHOST)		+= vhost/
obj-$(CONFIG_GREYBUS)		+= greybus/
obj-$(CONFIG_COMEDI)		+= comedi/
obj-$(CONFIG_STAGING)		+= staging/
obj-y				+= platform/

obj-$(CONFIG_MAILBOX)		+= mailbox/
obj-$(CONFIG_HWSPINLOCK)	+= hwspinlock/
obj-$(CONFIG_REMOTEPROC)	+= remoteproc/
obj-$(CONFIG_RPMSG)		+= rpmsg/
obj-$(CONFIG_SOUNDWIRE)		+= soundwire/

# Virtualization drivers
obj-$(CONFIG_VIRT_DRIVERS)	+= virt/
obj-$(subst m,y,$(CONFIG_HYPERV))	+= hv/

obj-$(CONFIG_PM_DEVFREQ)	+= devfreq/
obj-$(CONFIG_EXTCON)		+= extcon/
obj-$(CONFIG_MEMORY)		+= memory/
obj-$(CONFIG_IIO)		+= iio/
obj-$(CONFIG_IPACK_BUS)		+= ipack/
obj-$(CONFIG_NTB)		+= ntb/
obj-$(CONFIG_POWERCAP)		+= powercap/
obj-$(CONFIG_MCB)		+= mcb/
obj-$(CONFIG_PERF_EVENTS)	+= perf/
obj-$(CONFIG_RAS)		+= ras/
obj-$(CONFIG_USB4)		+= thunderbolt/
obj-$(CONFIG_CORESIGHT)		+= hwtracing/coresight/
obj-y				+= hwtracing/intel_th/
obj-$(CONFIG_STM)		+= hwtracing/stm/
obj-$(CONFIG_HISI_PTT)		+= hwtracing/ptt/
obj-y				+= android/
obj-$(CONFIG_NVMEM)		+= nvmem/
obj-$(CONFIG_FPGA)		+= fpga/
obj-$(CONFIG_FSI)		+= fsi/
obj-$(CONFIG_TEE)		+= tee/
obj-$(CONFIG_MULTIPLEXER)	+= mux/
obj-$(CONFIG_SIOX)		+= siox/
obj-$(CONFIG_GNSS)		+= gnss/
obj-$(CONFIG_INTERCONNECT)	+= interconnect/
obj-$(CONFIG_COUNTER)		+= counter/
obj-$(CONFIG_MOST)		+= most/
obj-$(CONFIG_PECI)		+= peci/
obj-$(CONFIG_HTE)		+= hte/
obj-$(CONFIG_DRM_ACCEL)		+= accel/
obj-$(CONFIG_CDX_BUS)		+= cdx/
obj-$(CONFIG_DPLL)		+= dpll/

obj-$(CONFIG_S390)		+= s390/

obj-$(CONFIG_CXL_SHMM_HANDLER)		+= cxlshm/
