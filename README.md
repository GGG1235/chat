# chat
基于Socket-TCP的简易聊天室

多线程

#### Server:
```
void send_msg(const std::string &msg){
    mtx.lock();
    for (int i = 0; i < clnt_cnt ; i++) {
        send(clnt_socks[i],msg.c_str(),msg.length()+1,0);
    }
    mtx.unlock();
}
```
服务端在收到某一客户端信息后把这一信息发送给所有在线的客户端.

#### Client
```shell script
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
```
客户端收到服务端传来的别的客户端发送的消息后把消息打印在控制台上.

###### make
```shell script
mkdir build

cd build

cmake ../

make
```

OR

```shell script
./make.sh
```

##### 截图:
