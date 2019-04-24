all: client server

distsys.h: distsys.x
	rpcgen distsys.x
	
distsys_svc.c distsys_clnt.c receiver.c: distsys.h

client: receiver.o distsys_clnt.o
	cc receiver.c distsys_clnt.c -o client

server: processor.o distsys_svc.o
	cc processor.c distsys_svc.c -o server

.PHONY: clean

clean:
	-rm *.o
	-rm client
	-rm server
	-rm distsys.h
	-rm distsys_clnt.c
	-rm distsys_svc.c


