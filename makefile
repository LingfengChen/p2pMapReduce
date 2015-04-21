main:client server

client: obj/clientmain.o obj/client.o obj/csapp.o
	gcc obj/clientmain.o obj/client.o obj/csapp.o -o ./bin/client

server: obj/servermain.o obj/server.o obj/csapp.o obj/fileseg.o
	gcc obj/servermain.o obj/server.o obj/csapp.o obj/fileseg.o -o ./bin/server

./obj/clientmain.o:./src/clientmain.c
	gcc -c ./src/clientmain.c -o ./obj/clientmain.o -I ./include

./obj/servermain.o:./src/servermain.c
	gcc -c ./src/servermain.c -o ./obj/servermain.o -I ./include

./obj/server.o:src/server.c
	gcc -c src/server.c -o obj/server.o -I ./include

./obj/client.o:src/client.c
	gcc -c src/client.c -o obj/client.o -I ./include

./obj/csapp.o:./src/csapp.c
	gcc -c src/csapp.c -o obj/csapp.o -I ./include

./obj/fileseg.o:./src/fileseg.c
	gcc -c src/fileseg.c -o obj/fileseg.o -I ./include


