// This is CLI file for project
#include "cli.h"

void cli_run(void)
{
    char buffer[256];
    // printf("CLI loop started. Type 'exit' to quit.\n");
    print_help();

    while (1) 
    {
        printf("cli> ");
        fflush(stdout);

        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Error reading input. Exiting.\n");
            break;
        }
            
        /* tokenise input */
        char *saveptr = NULL;
        char *cmd = strtok_r(buffer, " \t\n", &saveptr);
        if (!cmd) {
            continue;
        }

        if ((strcmp(cmd, "exit") == 0) || (strcmp(cmd, "quit") == 0) || (strcmp(cmd, "q") == 0)) {
            break;
        }
        if ((strcmp(cmd, "help") == 0) || (strcmp(cmd, "h") == 0) || (strcmp(cmd, "?") == 0)) {
            print_help();
            continue;
        }

        if (strcmp(cmd, "show-interfaces") == 0) {
            print_interface(show_interface(), NULL); /* NULL means show all */
            continue;
        }

        if(strcmp(cmd, "show-interface") == 0) {
            char *iface_name = strtok_r(NULL, " \t\n", &saveptr);
            if (!iface_name) {
                printf("Error: missing interface name. Usage: show-interface <interface>\n");
                continue;
            }
            print_interface(show_interface(), iface_name);
            continue;
        }

        if(strcmp(cmd, "show-interface-stats") == 0) {
            char *iface_name = strtok_r(NULL, " \t\n", &saveptr);
            if (!iface_name) {
                printf("Error: missing interface name. Usage: show-interface-stats <interface>\n");
                continue;
            }
            print_interface_statistics(show_interface_statistics(), iface_name);
            continue;
        }   

        if(strcmp(cmd, "show-all-interface-stats") == 0) {
            print_interface_statistics(show_interface_statistics(), NULL); /* NULL means show all */
            continue;
        }

        if (strcmp(cmd, "show-route") == 0 || strcmp(cmd, "show-routes") == 0) {
            print_route(show_route());
            continue;
        }
        if(strcmp(cmd, "network") == 0) {
            char *iface_name = strtok_r(NULL, " \t\n", &saveptr);
            char *status = strtok_r(NULL, " \t\n", &saveptr);
            if (!iface_name || !status) {
                printf("Error: missing arguments. Usage: network <interface> up|down\n");
                continue;
            }
            uint8_t is_up = (strcmp(status, "up") == 0) ? 1 : 0;
            update_interface_status(iface_name, is_up);
            continue;
        }
        if(strcmp(cmd, "route-del") == 0) {
            char *net_text = strtok_r(NULL, " \t\n", &saveptr);
            char *prefix_text = strtok_r(NULL, " \t\n", &saveptr);
            char *nexthop_text = strtok_r(NULL, " \t\n", &saveptr);
            char *iface_text = strtok_r(NULL, " \t\n", &saveptr);
            if (!net_text || !prefix_text || !nexthop_text || !iface_text) {
                printf("Error: missing arguments. Usage: route-del <network> <prefix> <nexthop> <interface> \n");
                continue;
            }

            struct in_addr addr;
            if (inet_pton(AF_INET, net_text, &addr) != 1) {
                 printf("Invalid network address\n");
                continue;
            }

            uint32_t net = ntohl(addr.s_addr);
            uint8_t prefix = (uint8_t)atoi(prefix_text);
            uint32_t nexthop = 0;

            if (strcmp(nexthop_text, "DIRECT") == 0) 
            {
               nexthop = 0;
            } 
            else 
            {
                if (inet_pton(AF_INET, nexthop_text, &addr) != 1) 
                {
                    printf("Invalid next hop address\n");
                    continue;
                }
                nexthop = ntohl(addr.s_addr);
            }
            
            delete_route(net, prefix, nexthop, iface_text);
            continue;
        }
        if(strcmp(cmd, "route-add") == 0) {
            char *net_text = strtok_r(NULL, " \t\n", &saveptr);
            char *prefix_text = strtok_r(NULL, " \t\n", &saveptr);
            char *nexthop_text = strtok_r(NULL, " \t\n", &saveptr);
            char *iface_text = strtok_r(NULL, " \t\n", &saveptr);
            if (!net_text || !prefix_text || !nexthop_text || !iface_text) {
                printf("Error: missing arguments. Usage: route-add <network> <prefix> <nexthop> <interface> \n");
                continue;
            }
            struct in_addr addr;

            if (inet_pton(AF_INET, net_text, &addr) != 1) {
                 printf("Invalid network address\n");
                continue;
            }

            uint32_t net = ntohl(addr.s_addr);
            uint8_t prefix = (uint8_t)atoi(prefix_text);
            uint32_t nexthop = 0;

            if (strcmp(nexthop_text, "DIRECT") == 0) 
            {
               nexthop = 0;
            } 
            else 
            {
                if (inet_pton(AF_INET, nexthop_text, &addr) != 1) 
                {
                    printf("Invalid next hop address\n");
                    continue;
                }
                nexthop = ntohl(addr.s_addr);
            }
            add_route(net, prefix, nexthop, iface_text);
            continue;
        }
        if(strcmp(cmd, "route-mod") == 0) {
            char *net_text = strtok_r(NULL, " \t\n", &saveptr);
            char *prefix_text = strtok_r(NULL, " \t\n", &saveptr);
            char *nexthop_text = strtok_r(NULL, " \t\n", &saveptr);
            char *iface_text = strtok_r(NULL, " \t\n", &saveptr);
            char *pref_text = strtok_r(NULL, " \t\n", &saveptr);
            if (!net_text || !prefix_text || !nexthop_text || !iface_text || !pref_text) {
                printf("Error: missing arguments. Usage: route-mod <network> <prefix> <nexthop> <interface> <preference> \n");
                continue;
            }

            struct in_addr addr;
            if (inet_pton(AF_INET, net_text, &addr) != 1) {
                 printf("Invalid network address\n");
                continue;
            }

            uint32_t net = ntohl(addr.s_addr);
            uint8_t prefix = (uint8_t)atoi(prefix_text);
            uint32_t nexthop = 0;

            if (strcmp(nexthop_text, "DIRECT") == 0) 
            {
               nexthop = 0;
            } 
            else 
            {
                if (inet_pton(AF_INET, nexthop_text, &addr) != 1) 
                {
                    printf("Invalid next hop address\n");
                    continue;
                }
                nexthop = ntohl(addr.s_addr);
            }
            
            uint8_t pref = (uint8_t)atoi(pref_text);
            modify_route(net, prefix, nexthop, iface_text, pref);
            continue;
        }

        if ((strcmp(cmd, "lookup") == 0) || (strcmp(cmd, "explain-lookup") == 0)) {
            char *ip_text = strtok_r(NULL, " \t\n", &saveptr);
            if (!ip_text) 
            {
                printf("Error: missing IPv4 address. Usage: %s <IPv4 address>\n", cmd);
                continue;
            }

            struct in6_addr addr6;
            struct in_addr addr4;
            _explain_t best = {0};
            
            if (inet_pton(AF_INET, ip_text, &addr4) == 1) {
                if (strcmp(cmd, "lookup") == 0) {
                    handle_lookup(ip_text, &best);
                    print_lookup_result(&best, ip_text);
                } else {
                    _explain_t explain_array[MAX_ROUTES] = {0};
                    handle_lookup(ip_text, &best);
                    uint16_t count = handle_explain_lookup(ip_text, explain_array);
                    print_lookup_result(&best, ip_text);
                    print_explain_lookup_result(count, explain_array, ip_text);
                }
            } else if (inet_pton(AF_INET6, ip_text, &addr6) == 1) {
                printf("Usage: %s <IPv4 address>\n", cmd);
            } else {
                printf("Error: invalid IPv4 address. Usage: %s <IPv4 address>\n", cmd);
            }

            continue;
        }

        printf("Unknown command: %s\n", cmd);
        print_help();
     }

    printf("CLI loop exited.\n");
}

