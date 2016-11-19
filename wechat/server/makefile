object = server.o 

server:$(object)
	gcc -o  server $(object) -I/usr/include/mysql -L/usr/lib/mysql  -lmysqlclient 
server.o:server.c 
	gcc -c  server.c      -I/usr/include/mysql -L/usr/lib/mysql  -lmysqlclient 

clean:
	rm  *.o  server

