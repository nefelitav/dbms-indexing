all: BF.h BF_64.a SHT.c SHT.h HT.c HT.h mainSHT.c
	gcc -c SHT.c -o SHT.o
	gcc -c HT.c -o HT.o
	gcc mainSHT.c -static BF_64.a SHT.o HT.o -no-pie -o mainSHT
 

clean: SHT.o HT.o
	rm SHT.o HT.o
	#make && ./mainSHT
