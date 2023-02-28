//
// Created by Atish Sinha on 2/27/23.
//
#include "utilities.h"

#include "client.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define TRUE 1
#define MSG_SIZE 256
#define BUFFER_SIZE 256


int Client::connect_to_server(int sock_fd, struct addrinfo* res){
    return connect(sock_fd, res->ai_addr, res->ai_addrlen);
}


Client::f_descriptors_client Client::connect_to_host(char *server_ip, char *port, struct addrinfo hints, Utilities ut)
{
    f_descriptors_client fd;
    struct addrinfo *res;
    int rv;


    rv = ut.get_address_info(server_ip, port, hints, &res);
    if(rv !=0 ){
        perror("Failure in the function getaddrinfo :");
    }

    /* Socket */
    fd.sock_fd = ut.create_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(fd.sock_fd == -1){
        perror("server: socket");
    }
    /* Connect */
    if (connect_to_server(fd.sock_fd, res) < 0)
        perror("Connect failed");

    freeaddrinfo(res);

    return fd;
}


int Client::run(char* server, char* port, Utilities ut)
{
    std::cout<<"Hey I am inside the client"<<std::endl;

    f_descriptors_client fd;


    struct addrinfo hints, *res;

    /* Set up hints structure */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;


    fd = connect_to_host(server, port, hints, ut);

    while (TRUE) {
        std::cout << "\n[PA1-Client@CSE489/589]$ ";
        std::cout.flush();

        char *msg = new char[MSG_SIZE];
        memset(msg, '\0', MSG_SIZE);
        if (fgets(msg, MSG_SIZE - 1, stdin) == NULL)
            exit(-1);

        std::cout << "I got: " << msg << "(size: " << strlen(msg) << " chars)";

        std::cout << "\nSENDing it to the remote server ... ";
        if (send(fd.sock_fd, msg, strlen(msg), 0) == strlen(msg))
            std::cout << "Done!\n";
        std::cout.flush();

        /* Initialize buffer to receive response */
        char *buffer = new char[BUFFER_SIZE];
        memset(buffer, '\0', BUFFER_SIZE);

        if (recv(fd.sock_fd, buffer, BUFFER_SIZE, 0) >= 0) {
            std::cout << "Server responded: " << buffer;
            std::cout.flush();
        }

        delete[] msg;
        delete[] buffer;
    }
}
