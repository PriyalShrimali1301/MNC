//
// Created by Atish Sinha on 2/27/23.
//

#ifndef MNC_UTILITIES_H
#define MNC_UTILITIES_H


class Utilities {
    public:
        int get_address_info(char* ip, char* node,struct addrinfo hints, addrinfo** res);
        int create_socket(int domain, int type, int protocol);
};


#endif //MNC_UTILITIES_H
