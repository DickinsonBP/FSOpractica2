all : winsuport.o tennis3

winsuport.o : winsuport.c winsuport.h
	gcc -Wall -c winsuport.c -o winsuport.o

winsuport2.o : winsuport2.c winsuport2.h
	gcc -Wall -c winsuport2.c -o winsuport2.o

tennis0 : tennis0.c winsuport.o winsuport.h
	gcc -Wall tennis0.c winsuport.o -o tennis0 -lcurses

tennis1 : tennis1.c winsuport.o winsuport.h
	gcc -Wall tennis1.c winsuport.o -o tennis1 -lcurses -lpthread

tennis2: tennis2.c winsuport.o winsuport.h
	gcc -Wall tennis2.c winsuport.o -o tennis2 -lcurses -lpthread

tennis3: tennis3.c winsuport.o winsuport2.h
	gcc -Wall tennis3.c winsuport.o -o tennis3 -lcurses -lpthread
	gcc pal_ord3.c winsuport.o -o pal_ord3 -lcurses

clean: 
	rm winsuport.o tennis3
