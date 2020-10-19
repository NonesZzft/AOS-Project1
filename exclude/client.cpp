// Client side C/C++ program to demonstrate Socket programming
// Server in the dc01.utdallas.edu
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <bits/stdc++.h>
using namespace std;
#define PORT 8080

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);


    // char hostbuffer[18] = "dc01.utdallas.edu";
    // struct hostent *host_entry;
    // host_entry = gethostbyname(hostbuffer);
    // char *IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));


    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    // if(inet_pton(AF_INET, IPbuffer, &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    string s = to_string(5);
    const char * buf = s.c_str();
    cout << strlen(buf) << endl;
    send(sock , buf , strlen(buf) , 0 );
    printf("Hello message sent\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );
    return 0;
}
