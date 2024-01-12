/* $Id: socket.c 1.1 1995/01/01 07:11:14 cthuang Exp $
 *
 * This module has been modified by Radim Kolar for OS/2 emx
 */

/***********************************************************************
  module:       socket.c
  program:      popclient
  SCCS ID:      @(#)socket.c    1.5  4/1/94
  programmer:   Virginia Tech Computing Center
  compiler:     DEC RISC C compiler (Ultrix 4.1)
  environment:  DEC Ultrix 4.3 
  description:  UNIX sockets code.
 ***********************************************************************/
 
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*
套接字创建
    查询用户所给host对应的ip地址
    连接至ip地址
    返回socket编号
*/
int Socket(const char *host, int clientPort)
{
    int sock;
    unsigned long inaddr;
    // sockaddr是c-socket编程通用的地址结构，
    // 但是sockaddr将地址和端口混用
    // 而sockaddr_in将地址和端口分开存储，并且能够转换为sockaddr
    // 因此常用的做法是：sockaddr_in用于sock赋值和定义 sockaddr用于传参
    struct sockaddr_in ad;
    // hostent是gethostbyname的返回值
    // gethostbyname用来解析域名，将域名转换为IP地址
    // 该接口已经被弃用，因为仅支持ipv4协议。
    // 新接口支持ipv4\ipv6与其他协议。
    struct hostent *hp;
    
    memset(&ad, 0, sizeof(ad));
    ad.sin_family = AF_INET;  // sin_family指定地址类型为AF_INET

    // 尝试通过inet_addr 将由点和数字组成的主机地址转换成二进制的网络字节顺序流
    inaddr = inet_addr(host);
    // 如果用户传入的host是域名则会返回INADDR_NONE
    if (inaddr != INADDR_NONE) // INADDR_NONE是错误地址的宏
        memcpy(&ad.sin_addr, &inaddr, sizeof(inaddr));
    // 通过gethostbyname解析域名
    else
    {
        hp = gethostbyname(host);
        if (hp == NULL)
            return -1;
        memcpy(&ad.sin_addr, hp->h_addr, hp->h_length);
    }
     // htons将整形变量从主机字节顺序转为网络字节顺序
    ad.sin_port = htons(clientPort);
    
    // 建立socket
    // socket返回文件描述符id。fid
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return sock;
    // 连接
    if (connect(sock, (struct sockaddr *)&ad, sizeof(ad)) < 0)
        return -1;
    return sock;
}

