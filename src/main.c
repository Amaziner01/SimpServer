#include<stdio.h>
#include"webserver.h"

int main(){
    WebServer *ws;
    ws = serverBegin(1234);
    serverSend(ws, getFileContent("index.html"));
    serverListen(ws);

    getchar();
}