#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
using namespace std;
#define SERVER_PORT 12138
#define CON_QUEUE 20
#define MAX_DATA_SIZE 4096
#define MAX_EVENTS 500
#define MTU 1008
void AcceptConn(int sockfd,int epollfd)
{

    struct sockaddr_in sin;

    socklen_t len=sizeof(struct sockaddr_in);

    bzero(&sin,len);

    int confd=accept(sockfd,(struct sockaddr *)&sin,&len);//所谓的另一个阻塞

    if(confd<0)
    {
        perror("connect error\n");
        exit(-1);
    }
    char *str = inet_ntoa(sin.sin_addr);
    cout << "accapt a connection from " << str<<"   "<<confd << endl;
    //把客户端新建立的连接添加到EPOLL的监听中

    struct epoll_event event;
    event.data.fd=confd;
    event.events=EPOLLIN|EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,confd,&event);
    return ;
}
void RecHandle(int clientfd)
{
    if(clientfd<0)
    {
        return;
    };
    int recvLen=0,totalLen;
    char recvBuf[MAX_DATA_SIZE];
    //memset(recvBuf,0,sizeof(recvBuf));
    recvLen=recv(clientfd,(char *)recvBuf,MAX_DATA_SIZE-1,0);
    if(recvLen==0)
    {
        close(clientfd);
        return ;
    }
    else if(recvLen<0)
    {
        if (errno == ECONNRESET)
        {
            close(clientfd);
        }
        else
        {
            perror("recv Error");
            exit(-1);
        }

    }
    totalLen=recvLen;
    while(recvLen>=MTU){
        recvLen=recv(clientfd,(char *)(recvBuf+totalLen),MAX_DATA_SIZE-totalLen-1,0);
        if(recvLen<0)
        {
            perror("recv Error");
            exit(-1);
        }
        totalLen+=recvLen;
    }
    //各种处理
    recvBuf[totalLen]='\0';
    printf("接收到%d字节数据:%s \n",totalLen,recvBuf);

    //close(clientfd);
    return ;
}
void SendHandle(int clientfd)
{

}
int main(int argc, char *argv[])
{
        //char* a = new char[8];//char* a="asada";

        struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
        char buf[1024] = { 0 };
        int s, sockfd,client, bytes_read;
        socklen_t opt = sizeof(rem_addr);

        // allocate socket
        sockfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

        // bind socket to port 1 of the first available
        // local bluetooth adapter
        loc_addr.rc_family = AF_BLUETOOTH;
        loc_addr.rc_bdaddr = (bdaddr_t) {{0, 0, 0, 0, 0, 0}};
        loc_addr.rc_channel = (uint8_t) 20;
        if(bind(sockfd, (struct sockaddr *)&loc_addr, sizeof(loc_addr))==-1)
        {
            perror("绑定失败");
            exit(-1);
        };

        // put socket into listening mode

        if(listen(sockfd, CON_QUEUE)==-1)
        {
             perror("监听失败");
             exit(-1);
        };

        //epoll初始化
        int epollfd;//epoll描述符
        struct epoll_event eventList[MAX_EVENTS];
        //epoll第一个接口函数，传入最大监听数
        epollfd=epoll_create(MAX_EVENTS);

        struct epoll_event event;
        event.events=EPOLLIN|EPOLLET;//可读，边缘触发
        event.data.fd=sockfd;//把server socket fd封装进events里面

        //epoll_ctl设置属性,注册事件,第二个接口函数
        if(epoll_ctl(epollfd,EPOLL_CTL_ADD,sockfd,&event)<0)
        {
            printf("epoll 加入失败 fd:%d\n",sockfd);
            exit(-1);
        }

        while(1)
        {
            int timeout=300;//设置超时;在select中使用的是timeval结构体

            //epoll_wait epoll处理
            //ret会返回在规定的时间内获取到IO数据的个数，并把获取到的event保存在eventList中，注意在每次执行该函数时eventList都会清空，由epoll_wait函数填写。
            //而不清除已经EPOLL_CTL_ADD到epollfd描述符的其他加入的文件描述符。这一点与select不同，select每次都要进行FD_SET，具体可看我的select讲解。
            //epoll里面的文件描述符要手动通过EPOLL_CTL_DEL进行删除。
            //可见epoll实际上也是同步，会阻塞在epoll_wait上
            int ret=epoll_wait(epollfd,eventList,MAX_EVENTS,timeout);

            if(ret<0)
            {
                perror("epoll error\n");
                break;
            }
            else if(ret==0)
            {
                //超时
                continue;
            }

            //直接获取了事件数量，给出了活动的流，这里就是跟selec，poll区别的关键
            //select要用遍历整个数组才知道是那个文件描述符有事件。而epoll直接就把有事件的文件描述符按顺序保存在eventList中
            //原来关键在这儿
            for(int i=0;i<ret;i++)
            {
                //错误输出 客户端手动关闭链接也会发送这个消息
                if((eventList[i].events & EPOLLERR) || (eventList[i].events & EPOLLHUP))
                {
                    printf("close:%d\n",eventList[i].data.fd);
                    close(eventList[i].data.fd);
                    continue;
                    //exit(-1);
                }

                if(eventList[i].data.fd==sockfd)
                {
                    //这个是判断sockfd的，主要是用于接收客户端的连接accept
                    AcceptConn(sockfd,epollfd);
                }
                else if(eventList[i].events&EPOLLIN) //里面可以通过判断eventList[i].events&EPOLLIN 或者 eventList[i].events&EPOLLOUT 来区分当前描述符的连接是对应recv还是send
                {
                    //其他所有与客户端连接的clientfd文件描述符
                    //获取数据等操作
                    //如需不接收客户端发来的数据，但是不关闭连接。
                    //epoll_ctl(epollfd, EPOLL_CTL_DEL,eventList[i].data.fd,eventList[i]);
                    //Handle对各个客户端发送的数据进行处理
                    RecHandle(eventList[i].data.fd);
                }
                else if (eventList[i].events&EPOLLOUT) {
                    SendHandle(eventList[i].data.fd);
                }

            }
        }

        close(epollfd);
        close(sockfd);
        return 0;

    //    struct sockaddr_rc addr={0};
    //       int s,status;
    //       char *dest = "00:1A:7D:DA:71:10";
    //       //char *dest = "00:01:E3:64:DD:9B";
    //       char* buf; //="00:11:67:32:61:62";


    //       // allocate socket
    //       s=socket(PF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM);
    //       if(s<0){
    //           perror("create socket error");
    //           exit(1);
    //         }
    //       // set the connection parameters (who to connect to )
    //       addr.rc_family=AF_BLUETOOTH;
    //       addr.rc_channel=(uint8_t)20;
    //       str2ba(dest,&addr.rc_bdaddr);
    //       // connect to server
    //       printf("connectting...\n");

    //       struct timeval start;
    //       struct timeval end;
    //       float timer;
    //       //gettimeofday(&start,NULL);           //结构体start当前时间（1970年1月1日到现在的时间）

    //       status=connect(s,(struct sockaddr *)&addr,sizeof(addr));

    //       //gettimeofday(&end,NULL);
    //       timer = end.tv_sec - start.tv_sec + (float)(end.tv_usec - start.tv_usec)/1000000;      //计算start和end之间的时间差
    //       printf("timer = %fs\n",timer);


    //       //printf("addr is %s",addr);
    //       // send a message
    //       if(status==0){
    //         printf("success!\n");
    //         std::string str = "hello!";
    //         //for(int i=0;i<300;++i){
    //             //str+="hello!";
    //         //}
    //         status=write(s,str.c_str(),str.size());
    //         printf("发送字节数：%d",str.size());
    //    //   do{
    //    //     scanf("%s",buf);
    //    //     status=write(s,buf,strlen(buf));
    //    //     if(status<0) perror("uh oh");
    //    //     }
    //    //     while(strcmp(buf,"goodbye")!=0);
    //       }
    //       else
    //           printf("Failed!\n");


    //       close(s);
    //       return 0;
}
int nmain(int argc, char *argv[])
{



       struct sockaddr_rc addr = { 0 };
       int s, status;
       char dest[18] = "00:1A:7D:DA:71:10";
                        //99:88:77:66:55:44  荣
                        //00:1A:7D:DA:71:10  建
       // allocate a socket
       s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);//RFCOMM是TCP连接

       // set the connection parameters (who to connect to)
       addr.rc_family = AF_BLUETOOTH;
       addr.rc_channel = (uint8_t) 20;
       str2ba( dest, &addr.rc_bdaddr );

       //int res = bind(s,(struct sockaddr *)&addr,sizeof(addr));
       // connect to server
       printf("start connect\n");
       status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
        printf("end connect\n");
       // send a message
       if( status == 0 ) {
           status = write(s, "hello!", 6);
           printf("gesendet\n");
       }

       if( status < 0 ) perror("uh oh");

       close(s);
       return 0;
//     inquiry_info *ii = NULL;
//     int max_rsp, num_rsp;
//     int dev_id, sock, len, flags;
//     int i;
//     char addr[19] = { 0 };
//     char name[248] = { 0 };
//     //只有一个适配器或使用哪一个并不重要，所以将NULL传递给hci_get_route将检索第一个可用的蓝牙适配器的ID：
//     dev_id = hci_get_route(NULL);//获取蓝牙适配器资源编号
//     sock = hci_open_dev( dev_id );
//     if (dev_id < 0 || sock < 0) {
//         perror("opening socket");
//         exit(1);
//     }

//     len = 8;//持续寻找时间 len*1.28秒
//     max_rsp = 255;//至多max_rsp设备将在输出参数ii中返回
//     /*
//    如果要刷新先前检测到的设备的缓存，请将标志设置为IREQ_CACHE_FLUSH;
//    否则，使用0，hci_inquiry也将返回超出范围，但仍在缓存中的蓝牙设备。
//    */
//     flags = IREQ_CACHE_FLUSH;
//     ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

//     num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);//对设备的查询或发现
//     if( num_rsp < 0 ) perror("hci_inquiry");

//     for (i = 0; i < num_rsp; i++)
//     {
//         ba2str(&(ii+i)->bdaddr, addr);
//         memset(name, 0, sizeof(name));
//         if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name),
//             name, 0) < 0)
//         strcpy(name, "[unknown]");
//         printf("%s %s\n", addr, name);
//     }

//     free( ii );
//     close( sock );
//     return 0;
}
