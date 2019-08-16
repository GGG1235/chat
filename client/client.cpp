//
// Created by GGG1235 on 2019-08-15.
//

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void send_msg(int sock);
void recv_msg(int sock);
int output(const char *arg,...);
int error_output(const char *arg,...);
void error_handling(const std::string &message);

std::string name="DEFAULT";
std::string msg;

int main(int argc,const char **argv,const char **envp){
    int sock;
    sockaddr_in serv_addr{};

    if (argc!=4){
        error_output("Usage : %s <IP> <Port> <Name> \n",argv[0]);
        exit(1);
    }

    name="["+std::string(argv[3])+"]";
    sock=socket(PF_INET,SOCK_STREAM,0);
    if (sock==-1){
        error_handling("socket() error!");
    }

    memset(&serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
    
    if (connect(sock,(sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("connect() error!");
    }
    
    std::thread snd(send_msg,sock);
    std::thread rcv(recv_msg,sock);
    
    snd.join();
    rcv.join();
    
    close(sock);

    return 0;
}

void send_msg(int sock){
    while(1){
        std::cin>>msg;
        if (msg=="Q"||msg=="q"){
            close(sock);
            exit(0);
        }
        std::string name_msg=name+" "+msg;
        send(sock,name_msg.c_str(),name_msg.length()+1,0);
    }
}
void recv_msg(int sock){
    char name_msg[BUF_SIZE+name.length()+1];
    while (1){
        int str_len=recv(sock,name_msg,BUF_SIZE+name.length()+1,0);
        if (str_len==-1){
            exit(-1);
        }
        std::cout<<std::string(name_msg)<<std::endl;
    }
}

int output(const char *arg,...){
    int res;
    va_list ap;
    va_start(ap,arg);
    res=vfprintf(stdout,arg,ap);
    va_end(ap);
    return res;
}

int error_output(const char *arg,...){
    int res;
    va_list ap;
    va_start(ap,arg);
    res=vfprintf(stderr,arg,ap);
    va_end(ap);
    return res;
}

void error_handling(const std::string &message){
    std::cerr<<message<<std::endl;
    exit(1);
}