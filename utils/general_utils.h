#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define IP_ADDRESS "127.0.0.1"
#define PORT 8080

#define USERNAME_LEN 32
#define MESSAGE_LEN 256
#define RESPONSE_LEN (USERNAME_LEN + MESSAGE_LEN)

int create_tcp_socket();
struct sockaddr_in create_ipv4_address(const char *ip_address, uint port);

int max(int a, int b);

#endif // GENERAL_UTILS_H
