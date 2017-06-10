# Team 1-F:
#  	deradaam
#  	derrowap
#  	finngw

PROG = kernel
IMG = floppya.img

$(IMG): $(PROG) bootload message.txt
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=bootload of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=$(PROG) of=$(IMG) bs=512 conv=notrunc seek=3
	dd if=message.txt of=$(IMG) bs=512 count=1 seek=30 conv=notrunc

$(PROG): kernel.o kernel_asm.o
	ld86 -o $(PROG) -d kernel.o kernel_asm.o

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c

kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o

bootload: bootload.asm
	nasm bootload.asm

bochs: opsys.bxrc
	bochs -f opsys.bxrc -q

# this is what should have been done to include message.txt in floppya.img
# but for the sake of this milestone and making it easier to test (not having
# to run another make command), it is included as default behaviour when
# building floppya.img
#testReadSector: message.txt
#	dd if=message.txt of=$(IMG) bs=512 count=1 seek=30 conv=notrunc

clean:
	rm -rf $(PROG) $(IMG) bootload *.o bochsout.txt