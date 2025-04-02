#ifndef SERVER_H
#define SERVER_H


struct server;
typedef struct server *Server;

/** Allocating memory for server
 * UDP connection
 */
Server NET_serverCreate();

/** Freeing memory after use, for server */
void NET_serverDestroy(Server aServer);



#endif