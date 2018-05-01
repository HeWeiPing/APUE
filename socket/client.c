/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2018年04月30日 03时03分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HeWeiPing (HWP), hwp195@163.com
 *   Organization:  Triangle Page
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVPORT 3333
#define MAXDATASIZE 100
int main(int argc,char *argv[]){
    int sockfd,sendbytes;
    struct hostent *host;
    struct sockaddr_in serv_addr;
    if(argc < 2){
	fprintf(stderr,"Please enter the server's hostname!\n");
	exit(1);
    }
    /*地址解析函数*/
    if((host=gethostbyname(argv[1]))==NULL){
	perror("gethostbyname");
	exit(1);
    }
    /*创建 socket*/
    if((sockfd=socket(AF_INET,SOCK_STREAM,0)) < 0){
	perror("socket");
	exit(1);
    }
    /*设置 sockaddr_in 结构体中相关参数*/
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERVPORT);
    serv_addr.sin_addr=*((struct in_addr *)host->h_addr);
    bzero(&(serv_addr.sin_zero),8);
    /*调用 connect 函数主动发起对服务器端的连接*/
    if(connect(sockfd,(struct sockaddr *)&serv_addr,\
		sizeof(struct sockaddr))<0){
	perror("connect");
	exit(1);
    }
    /*发送消息给服务器端*/
    if((sendbytes=send(sockfd,"hello",5,0))<0){
	perror("send");
	exit(1);
    }
    close(sockfd);
}
