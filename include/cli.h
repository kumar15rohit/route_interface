#ifndef CLI_H
#define CLI_H
#include "common.h"
#include "route_table.h"
#include "interface_table.h"

void cli_run();
void print_help(void);
void print_interface(_interface_t *it, const char *interface_name);
void print_interface_statistics(_interface_t *it, const char *interface_name);
void print_route(_route_t *rt);
void print_lookup_result(_explain_t *best, const char *ip_addr);
void print_explain_lookup_result(uint8_t count, _explain_t *explain_array, const char *ip_text);

#endif // CLI_H