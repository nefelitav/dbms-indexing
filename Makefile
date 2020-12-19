all: main.c HP.c HP.h BF.h BF_64.a
	gcc -c HP.c -o HP.o
	gcc -o main main.c HP.c -no-pie BF_64.a

clean:
	rm -o main HP
	//make && ./main