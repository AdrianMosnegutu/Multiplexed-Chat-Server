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

/*
 *  @brief Create and return a configured TCP socket.
 *
 *  @return A TCP socket.
 */
int create_tcp_socket();

/*
 *  @brief Create and return a configured IPv4 address.
 *  If no `ip_address` is provided (it is `NULL`), it will be considered as `INADDR_ANY` (a server address).
 *
 *  @param `ip_address` The IP address to assign.
 *  @param `port` The port to assign.
 *  @return An IPv4 address.
 */
struct sockaddr_in create_ipv4_address(const char *ip_address, uint port);

/*
 *  @brief Calculate the maximum of two integers.
 *
 *  @param `a` The first integer.
 *  @param `b` The second integer
 *  @return The maximum of two integers.
 */
int max(int a, int b);

#endif // GENERAL_UTILS_H
