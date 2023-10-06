#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "file.h"
#include <pthread.h>

/*
 *  Here is the starting point for your netster part.2 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */
void file_server(char* iface, long port, int use_udp, FILE* fp) {
    int serv_sock;
    int rbytes;
    struct sockaddr_in cl_addr;
    char client_message[256];
    size_t data;
    socklen_t cl_ad_size= sizeof(cl_addr);
    struct sockaddr_in serv_ad;
    serv_ad.sin_family = AF_INET;
    serv_ad.sin_addr.s_addr = INADDR_ANY;
    serv_ad.sin_port = htons(port);
    
    if(use_udp==0){

        int cli_con;
        serv_sock=socket(AF_INET, SOCK_STREAM, 0);
        bind(serv_sock, (struct sockaddr *)&serv_ad, sizeof(serv_ad));
        listen(serv_sock, 10);
        cli_con=accept(serv_sock, (struct sockaddr *)&cl_addr, &cl_ad_size);

        while((rbytes = read(cli_con, client_message, 255))>0){
            printf("File receiving in progress\n");
            data = fwrite(client_message, sizeof(char), rbytes, fp);
            if(data < 0){
                perror("Error writing file");
            }
            bzero(client_message, sizeof(client_message));
        }
    }
    
    else{

        serv_sock=socket(AF_INET, SOCK_DGRAM, 0);
        bind(serv_sock, (struct sockaddr *)&serv_ad, sizeof(serv_ad));
        while((rbytes = recvfrom(serv_sock, client_message, 255, 0, (struct sockaddr *)&cl_addr, &cl_ad_size))>0){
            printf("File receiving in progress...\n");
            data = fwrite(client_message, sizeof(char), rbytes, fp);
            if (data < 0){
               perror("Error writing file");
            }
            if (rbytes < 255){
                break;
            }
            bzero(client_message, sizeof(client_message));
        } 
    }
    close(serv_sock);
}

void file_client(char* host, long port, int use_udp, FILE* fp) {
  
    struct sockaddr_in serv_ad;
    int client_socket_fd;
    char server_message[256];
    size_t data;

    if (strcmp(host, "localhost") == 0){
        host = "127.0.0.1";
    }

    serv_ad.sin_family = AF_INET;
    serv_ad.sin_port = htons(port);
    serv_ad.sin_addr.s_addr = inet_addr(host);

    if (use_udp == 0){
        client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        connect(client_socket_fd, (struct sockaddr *)&serv_ad, sizeof(serv_ad));
        bzero(server_message, sizeof(server_message));
        while ((data = fread(server_message, sizeof(char), 255, fp))>0){
            send(client_socket_fd, server_message, data, 0);
        }
    }
    else{
        client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        bzero(server_message, sizeof(server_message));
        while ((data = fread(server_message, sizeof(char), 255, fp))>0){
            sendto(client_socket_fd, server_message, data, 0, (struct sockaddr *)&serv_ad, sizeof(serv_ad));
        }
    }
    close(client_socket_fd);
}