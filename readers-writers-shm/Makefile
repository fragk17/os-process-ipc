files: creator purger writer reader

clear: 
	-rm -f *.o



writer : writer.o
	gcc -o writer writer.o

writer.o : writer.c
	gcc -c writer.c -lpthread

reader : reader.o
	gcc -o reader reader.o

reader.o : reader.c
	gcc -c reader.c -lpthread



create : creator
	./creator

creator : creator.o
	gcc -o creator creator.o

creator.o : creator.c
	gcc -c creator.c -lpthread



purge : purger
	./purger -s 1111

purger : purger.o
	gcc -o purger purger.o

purger.o : purger.c
	gcc -c purger.c -lpthread




# make create
# ./reader -f accounts50.bin -l 48 -d 5 -s 1111
# ./writer -f accounts50.bin -l 48 -d 15 -s 1111 -v 5
# ./reader -f accounts50.bin -l 48,50 -d 5 -s 1111
# make purge
clean :
	rm -f *.o creator purger reader writer
