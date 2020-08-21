#ifndef WEBSERV_H
#define WEBSERV_H

//Windows inc

#ifdef _WIN32
#include<winsock.h>
//Linux inc
#elif __linux__
#include<sys/sockets.c>

#endif

//WebServer
typedef struct{
    //Windows Elements
    #ifdef _WIN32
    SOCKET server;
    SOCKADDR_IN server_addr;
    //Linux Elements
    #elif __linux__
    int socket;

    #endif

    int port;
}WebServer;

//Functions
WebServer *serverBegin(int port);

void serverListen(WebServer *ws);

void serverSend(WebServer *server, const char *send_cont);

char *getFileContent(const char *path);


#endif