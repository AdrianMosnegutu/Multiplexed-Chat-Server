#include "general_utils.h"

#include <arpa/inet.h>
#include <sys/socket.h>

int create_tcp_socket() { return socket(AF_INET, SOCK_STREAM, 0); }

struct sockaddr_in create_ipv4_address(const char *ip_address, uint port) {
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = ip_address ? inet_addr(ip_address) : INADDR_ANY;

    return address;
}

int max(int a, int b) { return a > b ? a : b; }
