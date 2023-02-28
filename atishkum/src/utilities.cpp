//
// Created by Atish Sinha on 2/27/23.
//

#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>

inline int Utilities::get_address_info(char* ip, char* node,struct addrinfo hints, addrinfo** res){
    return getaddrinfo(ip, node, &hints, res);
}
inline int Utilities::create_socket(int domain, int type, int protocol){
    return socket(domain, type, protocol);
}

