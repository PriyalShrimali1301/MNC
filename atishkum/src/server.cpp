//
// Created by Atish Sinha on 2/25/23.
//
#include "utilities.h"
#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256


//int Server::get_address_info(char* ip, char* node,struct addrinfo hints,struct addrinfo** res){
//    return getaddrinfo(ip, node, &hints, res);
//}


//int Server::create_socket(int domain, int type, int protocol) {
//    return socket(domain, type, protocol);
//}

int Server::bind_socket(int sd, sockaddr *addr, socklen_t t) {
    return bind(sd, addr, t);
}

int Server::listen_socket(int socket_descriptor, int no_of_sockets_allowed=BACKLOG) {
    return listen(socket_descriptor,no_of_sockets_allowed);
}

Server::f_descriptors Server::socket_set_up_and_listen(Utilities ut, struct addrinfo* p) {
    f_descriptors fd;
    /*
  * Creation of socket.
  */
    fd.sock_fd = ut.create_socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if(fd.sock_fd == -1){
        perror("server: socket");
    }

    /*
     * Binding of socket.
     */
    fd.bind_fd = bind_socket(fd.sock_fd, p->ai_addr, p->ai_addrlen);
    if(fd.bind_fd==-1){
        close(fd.sock_fd);
        perror("server:bind");
    }
    /*
     * Once the socket is bound we need not use the res again, so we free it.
     */
    freeaddrinfo(p);

    /*
     * Listening for incoming connections.
     */
    fd.listen_fd = listen_socket(fd.sock_fd);
    if(fd.listen_fd ==- 1){
        perror("listen");
        exit(1);
    }
    return fd;
}




int Server::run(char* ip, char* port, Utilities ut){
    std::cout<<"From inside the server"<<std::endl;
    int head_fd, socket_index, selector_return, fdaccept=0;
    int caddr_len;
    struct addrinfo *res;
    struct sockaddr_in client_addr;


    int rv;
    fd_set master_list, watch_list;

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /*
     * Get the address information and populate the structures for later use.
     * It Returns the pointer to res (results).
     */
    rv = ut.get_address_info(NULL, port, hints, &res);
    if(rv !=0 ){
      perror("Failure in the function getaddrinfo :");
    }


    /*
     * Sets up the socket, binds socket to the port and starts listening on that port
     */
    f_descriptors fd = socket_set_up_and_listen(ut, res);

    std::cout<<"waiting for connection"<<std::endl;

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    /* Register the listening socket */
    FD_SET(fd.sock_fd, &master_list);
    /* Register STDIN */
    FD_SET(STDIN, &master_list);

    head_fd = fd.sock_fd;

    while(TRUE){
        memcpy(&watch_list, &master_list, sizeof(master_list));

        //printf("\n[PA1-Server@CSE489/589]$ ");
        //fflush(stdout);

        /* select() system call. This will BLOCK */
        selector_return = select(head_fd + 1, &watch_list, NULL, NULL, NULL);
        if(selector_return < 0)
            perror("select failed.");

        /* Check if we have sockets/STDIN to process */
        if(selector_return > 0){
            /* Loop through socket descriptors to check which ones are ready */
            for(socket_index=0; socket_index<=head_fd; socket_index+=1){

                if(FD_ISSET(socket_index, &watch_list)){

                    /* Check if new command on STDIN */
                    if (socket_index == STDIN){
                        std::cout<<"Got standard input"<<std::endl;
                        char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

                        memset(cmd, '\0', CMD_SIZE);
                        if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
                            exit(-1);

                        printf("\nI got: %s\n", cmd);

                        //Process PA1 commands here ...

                        free(cmd);
                    }
                        /* Check if new client is requesting connection */
                    else if(socket_index == fd.sock_fd){
                        caddr_len = sizeof(client_addr);
                        fdaccept = accept(fd.sock_fd, (struct sockaddr* )&client_addr, (socklen_t*)&caddr_len);
                        if(fdaccept < 0)
                            perror("Accept failed.");

                        printf("\nRemote Host connected!\n");

                        /* Add to watched socket list */
                        FD_SET(fdaccept, &master_list);
                        if(fdaccept > head_fd) head_fd = fdaccept;
                    }
                        /* Read from existing clients */
                    else{
                        /* Initialize buffer to receieve response */
                        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);

                        if(recv(socket_index, buffer, BUFFER_SIZE, 0) <= 0){
                            close(socket_index);
                            printf("Remote Host terminated connection!\n");

                            /* Remove from watched list */
                            FD_CLR(socket_index, &master_list);
                        }
                        else {
                            //Process incoming data from existing clients here ...

                            printf("\nClient sent me: %s\n", buffer);
                            printf("ECHOing it back to the remote host ... ");
                            if(send(fdaccept, buffer, strlen(buffer), 0) == strlen(buffer))
                                printf("Done!\n");
                            fflush(stdout);
                        }

                        free(buffer);
                    }
                }
            }
        }
    }


}

