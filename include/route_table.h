#ifndef ROUTE_TABLE_H
#define ROUTE_TABLE_H
#include "common.h" 
#include "lpm_engine.h"

typedef enum route_interface_status_t
{
    INACTIVE_ROUTE = 0,
    ACTIVE_ROUTE
} _route_interface_status_t;

typedef enum route_type_t
{
    ROUTE_TYPE_CONNECTED = 0,
    ROUTE_TYPE_STATIC
} _route_type_t;

typedef struct route_t
{
    uint32_t network;
    uint8_t prefix_len;
    uint32_t next_hop;
    char interface[INTERFACE_NAME_LEN];
    _route_type_t route_type;
    bool is_active; /* used to mark routes 
                       as active/inactive based on interface status 
                       or other criteria */
    uint16_t preference; /* lower value means higher preference, used for tie-breaking */
} _route_t;

extern _route_t route_table[MAX_ROUTES];

_route_t *show_route(void);
uint8_t handle_lookup(const char *ip_text, _explain_t *out_best);
uint8_t handle_explain_lookup(const char *ip_text, _explain_t *out_explain_array);
void update_route_status_based_on_interface(const char *interface_name, uint8_t is_up);
void delete_route(uint32_t network, uint8_t prefix_len, uint32_t next_hop, const char *interface);
void add_route(uint32_t network, uint8_t prefix_len, uint32_t next_hop, const char *interface);
void modify_route(uint32_t network, uint8_t prefix_len, uint32_t next_hop, const char *interface, uint8_t pref);
#endif // ROUTE_TABLE_H