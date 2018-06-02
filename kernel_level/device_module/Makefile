obj-m += procfs.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
CFLAGS +=  -DEXPORT_SYMTAB

default:
	$(MAKE) $(CFLAGS) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
