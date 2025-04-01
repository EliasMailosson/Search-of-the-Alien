#ifndef SERVER_H
#define SERVER_H


struct server;
typedef struct server *Server;

Server NET_serverCreate();
void NET_serverDestroy(Server aServer);



#endif