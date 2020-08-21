#include"webserver.h"
#include<stdio.h>
#include<string.h>

#define SASSERT(x, msg) if(x == SOCKET_ERROR){\
                     printf("%s: %i\n", msg, WSAGetLastError());\
                    Sleep(3000);\
                    exit(-1);\
                    }

//Send Content
char *content;

WebServer *serverBegin(int port){
    WebServer *r_server;
    r_server = (WebServer *)malloc(sizeof(WebServer));

    WSADATA wsa;
    WSAStartup(MAKEWORD(1,1), &wsa);
    
    r_server->server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    r_server->server_addr.sin_port = htons(port);
    r_server->server_addr.sin_family = AF_INET;
    r_server->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    r_server->port = port;

    return r_server;
}

void serverListen(WebServer *ws){

    SASSERT(bind(ws->server, (SOCKADDR*)&ws->server_addr, sizeof(ws->server_addr)), "BIND ERROR");
    SASSERT(listen(ws->server, 5), "LISTEN ERROR");

    printf("Server running on port %i\n", ws->port);

    SOCKET client;
    SOCKADDR_IN client_addr;
    int len = sizeof(SOCKADDR_IN);
    while (1)
    {
        
        client = accept(ws->server, &client_addr, &len);

        if(client == INVALID_SOCKET){
            closesocket(client);
            WSACleanup();
            exit(-3);
        }
        char msg_r[2048];
        const char *http_res = "HTTP/1.1 200 OK \r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
        

        printf("Connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
        
        SASSERT(recv(client, msg_r, 2048, 0), "RECEIVE ERROR");   
        printf("Received: %s\n", msg_r);

        //default html
        const char *html_content = "<h1>I'm Awesome</h1> <p>You are also awesome.</p>" ;

        SASSERT(send(client, http_res, strlen(http_res) + 1, MSG_OOB), "SEND ERROR");

        
        if(content == NULL){
            SASSERT(send(client, html_content, strlen(html_content) + 1, MSG_OOB), "SEND ERROR");
        }
        else{
            SASSERT(send(client, content, strlen(content) + 1, MSG_OOB), "SEND ERROR");
        }
        
        closesocket(client);
        printf("Closed\n");
    }
}

void serverSend(WebServer *server, const char *send_cont){
    content = send_cont;
}

char *getFileContent(const char *path){
    FILE *file;
    char *ret;

    file = fopen(path, "r");

    if(!file){
        printf("File not found...\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    ret = (char*)malloc(size + 1);

    fread(ret, 1, size, file);
    ret[size] = '\0';

    fclose(file);
    return ret;
}