#ifndef WEBSERV_H
#define WEBSERV_H



#include<dirent.h>

//Windows inc
#ifdef _WIN32
#include<winsock.h>
//Linux inc
#elif __linux__
#include<sys/sockets.h>

#endif

//WebServer
typedef struct dirent DIRENT;

/////Functions/////

//WebServer
void wsBegin(int port);
void wsListen();
void wsSetServePath(const char *path);


//Files
char *readFile(const char *path);


#endif