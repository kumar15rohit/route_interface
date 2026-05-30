#include "common.h"
#include "cli.h"

void print_help()
{
    printf("=== CLI Help ===\n");
    printf("Supported commands: \n");
    printf("  show-interfaces            - Display interface information\n");
    printf("  show-interface <eth>       - Display information for specific interface\n");
    printf("  show-interface-stats <eth> - Display statistics for specific interface\n");
    printf("  show-all-interface-stats   - Display statistics for all interfaces\n");
    printf("  show-route | show-routes   - Display route information\n");
    printf("  network <eth> down|up      - Update interface status\n");
    printf("  route-del <net> <prefix> <nexthop> <interface>  - Delete route at index\n");
    printf("  route-add <net> <prefix> <nexthop> <interface>  - Add a new route\n");
    printf("  route-mod <net> <prefix> <nexthop> <interface> <preference> - Modify an existing route\n");
    printf("  lookup <IPv4>              - Perform a route lookup\n");
    printf("  explain-lookup <IPv4>      - Explain the route lookup process\n");
    printf("  help | h | ?               - Display the help message\n");
    printf("  exit | quit | q            - Exit the CLI\n");
    printf("================\n");
}

void print_interface(_interface_t *it, const char *interface_name)
{
    bool found = false;

    printf("%-16s %-18s %-8s %-18s %s\n", "Interface", "IP Address", "Prefix", "Oper Status", "Admin Status");
    printf("%-16s %-18s %-8s %-18s %s\n", "---------", "----------", "------", "------------", "------------");

    if(interface_name) {
        for (int i = 0; i < MAX_INTERFACES; ++i) {
            if (it[i].name[0] == '\0') continue;
            if (strcmp(it[i].name, interface_name) == 0)
            {
                char ipbuf[INET_ADDRSTRLEN] = "0.0.0.0";
                if (it[i].ip_addr != 0) {
                    struct in_addr a = { htonl(it[i].ip_addr) };
                    inet_ntop(AF_INET, &a, ipbuf, sizeof(ipbuf));
                }

                printf("%-16s %-18s %-8u %-18s %s\n",
                   it[i].name,
                   ipbuf,
                   (unsigned)it[i].prefix_len,
                   it[i].is_up ? "up" : "down",
                   it[i].admin_up ? "up" : "down");
                found = true;
                return;
            }
        }
    } else {
     for (int i = 0; i < MAX_INTERFACES; ++i) {
        _interface_t *it = &interface_table[i];
        if (it->name[0] == '\0') continue;

        char ipbuf[INET_ADDRSTRLEN] = "0.0.0.0";
        if (it->ip_addr != 0) {
            struct in_addr a = { htonl(it->ip_addr) };
            inet_ntop(AF_INET, &a, ipbuf, sizeof(ipbuf));
        }

        printf("%-16s %-18s %-8u %-18s %s\n",
               it->name,
               ipbuf,
               (unsigned)it->prefix_len,
               it->is_up ? "up" : "down",
               it->admin_up ? "up" : "down");
        found = true;
        }
    }
    if(!found) {
        printf("interface is not configured\n");
    }
}

void print_interface_statistics(_interface_t *it, const char *interface_name)
{
    bool found = false;
    printf("%-16s %15s %15s %15s %15s\n", "Interface", "RX Packets", "TX Packets", "RX Bytes", "TX Bytes");
    printf("%-16s %15s %15s %15s %15s\n", "---------", "----------", "----------", "--------", "--------");

    if(interface_name) {
        for (int i = 0; i < MAX_INTERFACES; ++i) {
            if (it[i].name[0] == '\0') continue;
            if (strcmp(it[i].name, interface_name) == 0)
            {
                printf("%-16s %15lu %15lu %15lu %15lu\n",
                    it[i].name,
                    it[i].rx_packets,
                    it[i].tx_packets,
                    it[i].rx_bytes,
                    it[i].tx_bytes);
                found = true;
                break;
            }
        }
        }else{
            for (int i = 0; i < MAX_INTERFACES; ++i) {
                _interface_t *it = &interface_table[i];
                if (it->name[0] == '\0') continue;

                printf("%-16s %15lu %15lu %15lu %15lu\n",
                    it->name,
                    it->rx_packets,
                    it->tx_packets,
                    it->rx_bytes,
                    it->tx_bytes);
                found = true;
            }
        }
    if(!found) {
        printf("interface is not configured\n");
    }
}

void print_route(_route_t *rt)
{
    printf("%-18s %-8s %-18s %-10s %10s %10s %s\n", "Network", "Prefix", "Next Hop", "Interface", "Route Type", "Status", "Preference");
    printf("%-18s %-8s %-18s %-10s %10s %10s %s\n", "-------", "------", "--------", "---------", "----------", "------", "----------");
    bool found = false;

    for (int i = 0; i < MAX_ROUTES; ++i) {
        if (rt[i].interface[0] == '\0' && rt[i].network == 0 && rt[i].next_hop == 0) continue;

        char netbuf[INET_ADDRSTRLEN] = "0.0.0.0";
        char nhbuf[INET_ADDRSTRLEN] = "DIRECT";
        if (rt[i].network != 0) {
            struct in_addr a = { htonl(rt[i].network) };
            inet_ntop(AF_INET, &a, netbuf, sizeof(netbuf));
        }
        if (rt[i].next_hop != 0) {
            struct in_addr b = { htonl(rt[i].next_hop) };
            inet_ntop(AF_INET, &b, nhbuf, sizeof(nhbuf));
        }

        printf("%-18s %-8u %-18s %-10s %10s %10s %d\n",
               netbuf,
               (unsigned)rt[i].prefix_len,
               nhbuf,
               rt[i].interface,
               rt[i].route_type == ROUTE_TYPE_CONNECTED ? "Connected" : "Static",
               rt[i].is_active ? "active" : "inactive",
               (unsigned)rt[i].preference);
        found = true;
    }

    if (!found) {
        printf("routes is not configured\n");
    }
}

void print_lookup_result(_explain_t *best, const char *ip_addr)
{
    if (!best) {
        printf("Error: route not found\n");
        return;
    }
    printf("Lookup Result for IP     : %s\n", ip_addr);
    printf("Matched Route Index      : %d\n", best->idx);
    printf("Outgoing Interface       : %s\n", best->interface);
    printf("Prefix Length            : %u\n", (unsigned)best->prefix_len);
    printf("Route Type               : %s\n", best->route_type == ROUTE_TYPE_CONNECTED ? "Connected" : "Static");
    printf("Next Hop                 : %s\n", best->next_hop == 0 ? "DIRECT" : inet_ntoa((struct in_addr){htonl(best->next_hop)}));
    printf("Status                   : %s\n", best->is_active ? "active" : "inactive");
}

void print_explain_lookup_result(uint8_t count, _explain_t *explain_array, const char *ip_addr)
{
    printf("Explain Lookup Result for IP: %s\n", ip_addr);
    printf("%-10s %-18s %-8s %-10s %10s %10s %s\n", "Index", "Matched Prefix", "Prefix Len", "Interface", "Route Type", "Status", "Preference");
    printf("%-10s %-18s %-8s %-10s %10s %10s %s\n", "-----", "--------------", "----------", "---------", "----------", "------", "----------");
    for (uint16_t i = 0; i < count; i++)
    {
        if (explain_array[i].idx == -1) continue;

        _explain_t *match = &explain_array[i];
        printf("%-10d %-18s %-8u %-10s %10s %10s %d\n",
               match->idx,
               inet_ntoa((struct in_addr){htonl(match->matched_prefix)}),
               (unsigned)match->prefix_len,
               match->interface,
               route_table[match->idx].route_type == ROUTE_TYPE_CONNECTED ? "Connected" : "Static",
               route_table[match->idx].is_active ? "active" : "inactive",
               route_table[match->idx].preference);    
    }
    
}
