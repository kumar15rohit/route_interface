/* This is main file for the project. It initializes the virtual router, reads configuration files, and starts the CLI for user interaction.
 * main.c
 */
#include "common.h"
#include "json_conf_loader.h"
#include "lpm_engine.h"
#include "cli.h"


int main()
{
    char path_to_interface_config[] = "interfaces.json";
    char path_to_static_routes[] = "static_routes.json";
    printf("*****************************************************\n");  
    printf("Project: Virtual Router Started :\n");
    printf("*****************************************************\n");

    // Read and store static configuration of interfaces and toutes from interface.json and static_routes.json
    read_n_store_interface_config(path_to_interface_config);
    read_n_store_static_routes(path_to_static_routes);
    cli_run();
 
    return 0;
}
