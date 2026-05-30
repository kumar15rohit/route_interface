#ifndef JSON_CONF_LOADER_H
#define JSON_CONF_LOADER_H
#include "common.h"
#include "interface_table.h"
#include "route_table.h"

int read_n_store_interface_config(const char *filename);
int read_n_store_static_routes(const char *filename);

#endif // JSON_CONF_LOADER_H    
