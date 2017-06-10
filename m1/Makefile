# Team 1-F:
#  	deradaam
#  	derrowap
#  	finngw

PROG = kernel

$(PROG): kernel.o kernel_asm.o floppya.img
	ld86 -o kernel -d kernel.o kernel_asm.o
	dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c

kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o

floppya.img: floppya.img bootload
	dd if=/dev/zero of=floppya.img bs=512 count=2880
	dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc

bootload: bootload.asm
	nasm bootload.asm

clean:
	rm -rf $(PROG) bootload *.o bochsout.txt