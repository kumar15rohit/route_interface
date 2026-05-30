#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdbool.h>

#define MAX_ROUTES      128
#define MAX_INTERFACES  16
#define INTERFACE_NAME_LEN 32

int parse_ip_address(const char *text, uint32_t *out_ip);

#endif // COMMON_H