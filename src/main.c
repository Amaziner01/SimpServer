#include<stdio.h>
#include"webserver.h"



int main(){
    wsBegin(1234);
    wsSetServePath("TestPath");
    wsListen();
    
    getchar();
}