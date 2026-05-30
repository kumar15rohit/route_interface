#include "route_table.h"
#include "interface_table.h"

extern int explain_lookup_index[MAX_ROUTES];
extern _interface_t interface_table[MAX_INTERFACES];
_route_t route_table[MAX_ROUTES];

_route_t *show_route(void)
{
    return route_table;   
}

uint8_t handle_lookup(const char *ip_addr, _explain_t *out_best)
{
    uint32_t ip;

    if (!ip_addr || parse_ip_address(ip_addr, &ip) != 0) {
        return 0;
    }
    return lpm_engine_lookup(ip, out_best);
}

uint8_t handle_explain_lookup(const char *ip_addr, _explain_t *matched_routes_array)
{
    uint32_t ip;

    if (!ip_addr || parse_ip_address(ip_addr, &ip) != 0) {
        printf("Error: missing or invalid IPv4 address. Usage: explain-lookup <IPv4>\n");
        return 0;
    }
    
    return lpm_engine_explain_lookup(ip, matched_routes_array);
}

void update_route_status_based_on_interface(const char *interface_name, uint8_t is_up)
{
    for (int i = 0; i < MAX_ROUTES; ++i) {
        _route_t *r = &route_table[i];
        if (r->interface[0] == '\0') continue;

        /* find corresponding interface */
        int iface_idx = -1;
        for (int j = 0; j < MAX_INTERFACES; ++j) {
            if (strcmp(interface_name, r->interface) == 0) {
                iface_idx = j;
                break;
            }
        }

        if (iface_idx >= 0) {
            r->is_active = is_up ? ACTIVE_ROUTE : INACTIVE_ROUTE;
        } 
        else 
        {
            printf("[INFO] Interface %s not found in route table\n", interface_name);
            /* if interface not found in route table, add a placeholder route to ensure future lookups can find the interface status */
        }

    }
}

void delete_route(uint32_t network, uint8_t prefix_len, uint32_t next_hop, const char *interface)
{
    for (int i = 0; i < MAX_ROUTES; ++i) {
        _route_t *r = &route_table[i];
        if (r->network == network && r->prefix_len == prefix_len && r->next_hop == next_hop && strcmp(r->interface, interface) == 0) {
            memset(r, 0, sizeof(_route_t));
            printf("Route at index %d deleted\n", i);
            return;
        }
    }
    printf("Error: route not found\n");
}

void add_route(uint32_t network, uint8_t prefix_len, uint32_t next_hop, const char *interface)
{
    if (prefix_len > 32) {
        printf("Error: invalid prefix length\n");
        return;
    }
    if (!interface || interface[0] == '\0') {
        printf("Error: invalid interface\n");
        return;
    }
    int if_status = get_interface_status(interface);
    if (if_status < 0) {
        printf("Error: interface does not exist\n");
        return;
    }

    for (int i = 0; i < MAX_ROUTES; ++i)
    {
        if (route_table[i].interface[0] == '\0' && route_table[i].network == 0 && route_table[i].next_hop == 0)
        {
            route_table[i].network = network;
            route_table[i].prefix_len = prefix_len;
            route_table[i].next_hop = next_hop;
            route_table[i].route_type = (next_hop == 0) ? ROUTE_TYPE_CONNECTED : ROUTE_TYPE_STATIC;
            strncpy(route_table[i].interface, interface, sizeof(route_table[i].interface) - 1);
            route_table[i].interface[sizeof(route_table[i].interface) - 1] = '\0';
            route_table[i].is_active = if_status ? ACTIVE_ROUTE :INACTIVE_ROUTE;

            struct in_addr net_addr = { htonl(network) };
            struct in_addr nh_addr  = { htonl(next_hop) };
            printf("Route added at index %d: %s/%u via %s dev %s\n",
                   i, inet_ntoa(net_addr), (unsigned)prefix_len,
                    next_hop == 0 ? "DIRECT" : inet_ntoa(nh_addr),
                    interface);
            return;
        }
    }
    printf("Error: route table full, cannot add new route\n");
}
void modify_route(uint32_t network, uint8_t prefix_len, uint32_t next_hop, const char *interface, uint8_t pref)
{
    bool found = false;

    for (int idx = 0; idx < MAX_ROUTES; ++idx)
    {
        if (route_table[idx].network == network &&
            route_table[idx].prefix_len == prefix_len)
        {
            _route_t *r = &route_table[idx];
            r->network = network;
            r->prefix_len = prefix_len;
            r->next_hop = next_hop;
            r->preference = pref;

            if (next_hop == 0) {
                r->route_type = ROUTE_TYPE_CONNECTED;
            } else {
                r->route_type = ROUTE_TYPE_STATIC;
            }

            strncpy(r->interface,
                    interface,
                    sizeof(r->interface) - 1);

            r->is_active =
                get_interface_status(interface) ?
                ACTIVE_ROUTE : INACTIVE_ROUTE;

            struct in_addr net_addr = { htonl(network) };
            struct in_addr nh_addr  = { htonl(next_hop) };

            printf("Route at index %d modified: %s/%u via %s dev %s\n",
                   idx, inet_ntoa(net_addr), (unsigned)prefix_len,
                   next_hop == 0 ? "DIRECT" : inet_ntoa(nh_addr),
                   interface);
            found = true;
            break;
        }
    }

    if (!found) {
        printf("Error: route not found, cannot modify\n");
    }
}