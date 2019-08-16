//
// Created by GGG1235 on 2019-08-15.
//

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define MAX_CLNT 256

void handle_clnt(int clnt_sock);
void send_msg(const std::string &msg);
int output(const char *arg,...);
int error_output(const char *arg,...);
void error_handling(const std::string &message);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
std::mutex mtx;

int main(int argc,const char **argv,const char **envp){
    int serv_sock,clnt_sock;
    sockaddr_in serv_addr{},clnt_addr{};
    socklen_t clnt_addr_size;

    if (argc!=2){
        error_output("Usage : %s <port> \n",argv[0]);
        exit(1);
    }

    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    if (serv_sock==-1){
        error_handling("socket() error!");
    }

    memset(&serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if (bind(serv_sock,(sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("bind() error!");
    }

    if (listen(serv_sock,5)==-1){
        error_handling("listen() error!");
    }

    while(1){
        clnt_addr_size= sizeof(clnt_addr);
        clnt_sock=accept(serv_sock,(sockaddr*)&clnt_addr,&clnt_addr_size);
        if (clnt_sock==-1){
            error_handling("accept() error!");
        }

        mtx.lock();
        clnt_socks[clnt_cnt++]=clnt_sock;
        mtx.unlock();

        std::thread th(handle_clnt,clnt_sock);
        th.detach();

        output("Connected client IP: %s \n",inet_ntoa(clnt_addr.sin_addr));

    }
    close(serv_sock);
    return 0;
}

void handle_clnt(int clnt_sock){
    char msg[BUF_SIZE];

    while(recv(clnt_sock,msg, sizeof(msg),0)!=0){
        send_msg(std::string(msg));
    }
    mtx.lock();
    for (int i = 0; i < clnt_cnt ; i++) {
        if (clnt_sock==clnt_socks[i]){
            while (i++<clnt_cnt-1){
                clnt_socks[i]=clnt_socks[i+1];
            }
            break;
        }
    }
    clnt_cnt--;
    mtx.unlock();
    close(clnt_sock);
}

void send_msg(const std::string &msg){
    mtx.lock();
    for (int i = 0; i < clnt_cnt ; i++) {
        send(clnt_socks[i],msg.c_str(),msg.length()+1,0);
    }
    mtx.unlock();
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