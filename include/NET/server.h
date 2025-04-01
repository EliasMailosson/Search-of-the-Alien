#ifndef SERVER_H
#define SERVER_H

#define MAX_CLIENTS 8
#define PORT 1234


struct server;
typedef struct server *Server;

Server NET_serverCreate();
void NET_serverDestroy(Server aServer);



#endif