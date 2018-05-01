/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2018年04月30日 01时57分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HeWeiPing (HWP), hwp195@163.com
 *   Organization:  Triangle Page
 *
 * =====================================================================================
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>


#define	SERVPORT	3333
#define	BACKLOG		10
#define	MAX_CONNECTED_NO 10
#define	MAXDATASIZE	5


int main(){
    struct sockaddr_in server_sockaddr, client_sockaddr;
    int sin_size, recvbytes;
    int sockfd, client_fd;
    char buf[MAXDATASIZE];

    //    create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
	perror("socket");
	exit(1);
    }
    printf("socket created success, sockfd=%d\n", sockfd);

    /*设置 sockaddr_in 结构体中相关参数*/
    server_sockaddr.sin_family=AF_INET;
    server_sockaddr.sin_port=htons(SERVPORT);
    server_sockaddr.sin_addr.s_addr=INADDR_ANY;
    bzero(&(server_sockaddr.sin_zero), 8);

    /*绑定函数 bind*/
    if(bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr)) < 0){
	perror("bind");
	exit(1);
    }
    printf("bind success...\n");

    /*调用 listen 函数*/
    if(listen(sockfd, BACKLOG) < 0){
	perror("listen");
	exit(1);
    }
    printf("listening............");

    /*调用 accept 函数,等待客户端的连接*/
    client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr, &sin_size);
    if (client_fd < 0){
	perror("accept");
	exit(1);
    }

    /*调用 recv 函数接收客户端的请求*/
    recvbytes = recv(client_fd, buf, MAXDATASIZE, 0);
    if (recvbytes < 0){
	perror("recv");
	exit(1);
    }
    printf("received a connection:%s\n", buf);
    close(sockfd);
}
