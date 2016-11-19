object = client.o 

client:$(object)
	gcc -o  client $(object) -I/usr/include/mysql -L/usr/lib/mysql  -lmysqlclient
client.o:client.c 
	gcc -c  client.c  -I/usr/include/mysql -L/usr/lib/mysql  -lmysqlclient

clean:
	rm  *.o  client

