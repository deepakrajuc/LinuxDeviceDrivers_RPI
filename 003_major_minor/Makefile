obj-m += deviceno.o
KERN_DIR=/lib/modules/6.6.62+rpt-rpi-v6/build
MODULE_DIR=$(PWD)

all:
	make -C $(KERN_DIR) M=$(MODULE_DIR)  modules

clean:
	make -C $(KERN_DIR) M=$(MODULE_DIR) clean
