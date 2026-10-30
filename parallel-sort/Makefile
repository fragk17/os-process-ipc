all : mysort sorting1 sorting2

run : mysort sorting1 sorting2
	./mysort $(ARGS)
#	use this : make run ARGS="-i voters50.bin -k 3 -e1 sorting1 -e2 sorting2"

sorting2 : sorting2.o
	gcc -o sorting2 sorting2.o

sorting2.o : sorting2.c
	gcc -c sorting2.c

sorting1 : sorting1.o
	gcc -o sorting1 sorting1.o

sorting1.o : sorting1.c
	gcc -c sorting1.c

mysort : mysort.o merge.o
	gcc -o mysort mysort.o merge.o

mysort.o : mysort.c
	gcc -c mysort.c

merge : merge.o
	gcc -o merge merge.o

merge.o : merge.c
	gcc -c merge.c
clean :
	rm -f *.o mysort sorting1 sorting2
