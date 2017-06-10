# Team 1-F:
#   deradaam
#   derrowap
#   finngw

PROG = kernel
PROG2 = shell
IMG = floppya.img

$(IMG): $(PROG) $(PROG2) bootload message.txt loadFile
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=bootload of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
	dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
	dd if=$(PROG) of=$(IMG) bs=512 conv=notrunc seek=3
	./loadFile $(PROG2)
	./loadFile tstpr2
	./loadFile tstprg
	./loadFile message.txt

$(PROG2): shell.o lib.o
	ld86 -o $(PROG2) -d shell.o lib.o

$(PROG): kernel.o kernel_asm.o
	ld86 -o $(PROG) -d kernel.o kernel_asm.o

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c

shell.o: shell.c
	bcc -ansi -c -o shell.o shell.c

lib.o: lib.asm
	as86 lib.asm -o lib.o

kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o

bootload: bootload.asm
	nasm bootload.asm

loadFile: loadFile.o
	gcc loadFile.o -o loadFile

loadFile.o: loadFile.c
	gcc -c loadFile.c

bochs: opsys.bxrc
	bochs -f opsys.bxrc -q

clean:
	rm -rf $(PROG) $(IMG) $(PROG2) bootload loadFile *.o bochsout.txt