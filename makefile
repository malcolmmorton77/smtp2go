emclient: emclient.o
	gcc -o emclient emclient.o
emclient.o:
	gcc -c emclient.c
clean:
	rm emclient.o emclient
update:
	make clean
	make