#include "interface_table.h"

_interface_t interface_table[MAX_INTERFACES];

_interface_t *show_interface()
{
    return interface_table;
}

_interface_t *show_interface_statistics()
{
    return interface_table;
}   

void bringup_new_interface(const char *interface_name, uint8_t is_up)
{
    /* find first empty slot */
    for (int i = 0; i < MAX_INTERFACES; ++i) {
        _interface_t *it = &interface_table[i];
        if (it->name[0] == '\0') {
            strncpy(it->name, interface_name, sizeof(it->name) - 1);
            it->ip_addr = 0;
            it->prefix_len = 0;
            it->is_up = is_up;
            printf("New interface %s added with status %s\n", it->name, it->is_up ? "up" : "down");
            return;
        }
    }
    printf("Error: interface table full, cannot add new interface %s\n", interface_name);
}

void update_interface_status(char *interface_name, uint8_t is_up)
{
    bool found = false;
    
    for (int i = 0; i < MAX_INTERFACES; ++i) {
        _interface_t *it = &interface_table[i];
        if (it->name[0] == '\0') continue;

        /* find corresponding routes and update their status */
        if (strcmp(it->name, interface_name) == 0) {
            it->is_up = is_up;
            printf("Interface %s status updated to %s\n", it->name, it->is_up ? "up" : "down");
            update_route_status_based_on_interface(interface_name, is_up);
            found = true;
            break;
        }
    }
    if(!found)
    {
        printf("Interface %s not found!\n", interface_name);
    }
}

uint8_t get_interface_status(const char *interface_name)
{
    for (int i = 0; i < MAX_INTERFACES; ++i) {
        _interface_t *it = &interface_table[i];
        if (it->name[0] == '\0') continue;

        if (strcmp(it->name, interface_name) == 0) {
            return it->is_up;
        }
    }
    return 0; /* default to down if interface not found */
}