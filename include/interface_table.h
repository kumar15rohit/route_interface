#ifndef INTERFACE_TABLE_H
#define INTERFACE_TABLE_H
#include "common.h"
#include "route_table.h"

typedef struct interface_t
{
    char name[INTERFACE_NAME_LEN];
    uint32_t ip_addr;
    uint8_t prefix_len;
    uint8_t admin_up;
    uint8_t is_up; /* operational status: 1 for up, 0 for down */
    uint64_t rx_packets;
    uint64_t tx_packets;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
} _interface_t;

extern _interface_t interface_table[MAX_INTERFACES];

_interface_t *show_interface(void);
_interface_t *show_interface_statistics(void);
void update_interface_status(char *interface_name, uint8_t is_up);
uint8_t get_interface_status(const char *interface_name);
void bringup_new_interface(const char *interface_name, uint8_t is_up);

#endif // INTERFACE_TABLE_H