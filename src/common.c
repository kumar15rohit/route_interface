
#include "common.h"

// This is the common file for project. It contains common definitions and utility functions used across the project.
int parse_ip_address(const char *text, uint32_t *out_ip)
{
    struct in_addr addr;
    if (inet_pton(AF_INET, text, &addr) != 1) {
        return -1;
    }
    *out_ip = ntohl(addr.s_addr);
    return 0;
}
