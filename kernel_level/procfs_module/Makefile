obj-m += procfs.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
CFLAGS :=  -DEXPORT_SYMTAB

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) CFLAGS=$(CFLAGS) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
