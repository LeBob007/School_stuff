#Makefile

all: makeandinstall

makeandinstall:
	cd /usr/src && sudo make buildkernel KERNCONF=MYKERNEL -DKERNFAST && sudo make installkernel KERNCONF=MYKERNEL && sudo reboot

benchmark:
	cc benchmark.c -o benchmark

copy:
	sudo cp kern_switch.c /usr/src/sys/kern

clean:
	rm -f benchmark
