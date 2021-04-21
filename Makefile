all: main.c HT.c HT.h BF.h BF_64.a
	gcc -c HT.c -o HT.o
	gcc -o main main.c HT.c -no-pie BF_64.a

clean:
	rm -f *.o main HT