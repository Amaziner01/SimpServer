#include"webserver.h"
#include<stdio.h>
#include<dirent.h>



//Assert Macro
#define SASSERT(x, msg) if(x == SOCKET_ERROR){\
                     printf("%s: %i\n", msg, WSAGetLastError());\
                    Sleep(3000);\
                    exit(-1);\
                    }

//Global variables;
char *filepath;
int _port;
SOCKET server;
SOCKADDR_IN server_addr;

//File Funcs
char *readFile(const char *path){
    FILE *file;
    char *result;

    file = fopen(path, "rb");

    if (!file)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    printf("%i\n",size);
    
    fseek(file, 0, SEEK_SET);
    //printf("%i\n",ftell(file));

    
    result = malloc(size + 1);
    fread(result, 1, size, file);
    result[size] = '\0';

    //printf("Result: %s", result);
    
    fclose(file);

    return result;
}


//Private Functions
char *getExtension(const char *filename){
    int count = 0;
    for (size_t i = 0; i < strlen(filename); i++)
    {
        if(filename[i - 1] == '.'){
            break;
        }
        count++;
    }

    int left = strlen(filename) - count;
    char *ret = malloc(left);
    ret = malloc(left);

    char name[80];
    strncpy(name, filename, count);
    strncpy(ret, filename + count, left);
    ret[left] = 0;
    
    return ret;    
}

char *MakePath(const char *dir, const char *filename){
    char *fullpath;
    
    int size1 = strlen(dir);
    int size2 = strlen(filename);

    fullpath = malloc(size1 + size2 + 2);
    strcpy(fullpath, dir);
    strcat(fullpath, "/");
    strcat(fullpath, filename);


    return fullpath;
}

void Route(SOCKET client, const char *route){
    DIR *dir;
    DIRENT *files;
    dir = opendir(filepath);

    if(!dir){
        printf("Directory not found\n");
        SendDefaultHTML(client);
    }
    else
    {
        while (files = readdir(dir))
        {
            char http_res[100];

            if(!strcmp(files->d_name, route)){
                printf("File found\n");

                //Extension check
                char *ext = getExtension(files->d_name);
                if(!strcmp(ext, "html") || !strcmp(ext, "css") || !strcmp(ext, "js") || !strcmp(ext, "json")){
                    //////Send Response///////
                    
                    sprintf(http_res, "HTTP/1.1 200 OK \r\nContent-Type: text/%s; charset=utf-8\r\n\r\n", ext);

                    printf("%s\n", http_res);
                    SASSERT(send(client, http_res, strlen(http_res) + 1, MSG_OOB), "SEND ERROR");
                }
                else
                {
                    sprintf(http_res, "HTTP/1.1 200 OK \r\nContent-Type: text/plain; charset=utf-8\r\n\r\n");
                    SASSERT(send(client, http_res, strlen(http_res) + 1, MSG_OOB), "SEND ERROR");
                }
                
                //Send File
                char *file = readFile(MakePath(filepath, files->d_name));
                SASSERT(send(client, file, strlen(file) + 1, MSG_OOB), "SEND ERROR");

                closedir(dir); 
                return;
            }
        }
        printf("File not found\n");
        SendDefaultHTML(client);
    }
    closedir(dir);  
    return;
}

void SendDefaultHTML(SOCKET client){
    char *def_http_res = "HTTP/1.1 200 OK \r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
    SASSERT(send(client, def_http_res, strlen(def_http_res) + 1, MSG_OOB), "SEND ERROR");

    const char *html_content = "<h1>I'm Awesome</h1> <p>You are also awesome.</p>" ;
    SASSERT(send(client, html_content, strlen(html_content) + 1, MSG_OOB), "SEND ERROR");
}

void HandleClient(SOCKET client, SOCKADDR_IN client_addr){
    if(client == INVALID_SOCKET){
            closesocket(client);
            WSACleanup();
            exit(-3);
    }

    //Connection Succesful
    printf("Connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));

    
    ///////Handle Receive///////
    char msg_r[2048];
    SASSERT(recv(client, msg_r, 2048, 0), "RECEIVE ERROR");
    printf("Received: %s\n", msg_r);

    //Handle File Route
    char *route;
    sscanf(msg_r, "GET /%s HTTP/1.1", route);
    printf("Route: %s\n", route);
       

    //Send file
    Route(client, route);

    closesocket(client);
    printf("Closed\n");
}


//Web Server Funcs
void wsBegin(int port){
    WSADATA wsa;
    WSAStartup(MAKEWORD(1,1), &wsa);
    
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    _port = port;
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

}

void wsListen(){

    SASSERT(bind(server, (SOCKADDR*)&server_addr, sizeof(server_addr)), "BIND ERROR");
    SASSERT(listen(server, 20), "LISTEN ERROR");

    printf("Server running on port %i\n", _port);

    //Client declarations
    SOCKET client;
    SOCKADDR_IN client_addr;
    int len = sizeof(SOCKADDR_IN);

    while (1)
    {
        client = accept(server, &client_addr, &len);
        HandleClient(client, client_addr);
    }
}

void wsSetServePath(const char *path){
    filepath = path;
}