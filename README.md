# Virtual Router in C

## 1. Project Overview

### Brief
This project implements a simple user-space virtual router in C for Linux.  
The router supports static route management, IPv4 longest prefix match (LPM) lookup, interface management, runtime route updates, and CLI-based interaction.

The implementation focuses on:
- Routing logic
- CLI handling
- JSON configuration parsing
- Route/interface table management
- Longest Prefix Match (LPM)
- Runtime configuration updates
- Unit testing and debugging

## 2. Features

### Implemented Features
- IPv4 routing support
- Connected route creation
- Static route handling
- Longest Prefix Match (LPM)
- Route add/delete/modify
- Interface up/down handling
- Route active/inactive handling
- Interface statistics support
- CLI-based interaction
- Explain lookup support
- JSON-based configuration
- Unit Test (UT) support
- Debug and validation logs

---

## 3. High Level Architecture

Main modules:
- CLI Module
- Route Table Manager
- LPM Engine
- JSON Configuration Loader

### High Level Flow
1. Read interface configuration from interfaces.json
2. Populate interface table
3. Create connected routes
4. Read static routes from static_routes.json
5. Populate route table
6. Start CLI loop
7. Perform route/interface operations based on user input
8. Use LPM engine for lookup operations

---

## 4. Project Structure

Routing_LPM_Cisco_Assignment_Rohit_Kumar 
    │ 
    ├── include/ 
    │       ├── common.h 
    │       ├── route_table.h 
    │       ├── interface_table.h 
    │       └── lpm_engine.h      
    ├── src/ 
    │   ├── main.c 
    │   ├── cli.c 
    │   ├── common.c 
    │   ├── json_conf_loader.c 
    │   ├── route_table_manager.c 
    │   └── lpm_engine.c 
    |
    ├── sample/ 
    │   ├── interfaces.json 
    │   └── static_routes.json 
    │ 
    ├── ut/ 
    │   ├── ut_route.c 
    │   └── ut_cli.c 
    │ 
    ├── build/
    |     └── makefile
    │
    └── doc/ 
        ├── README.md 
        ├── VirtualRouter_DesignDocument_v1.0.pdf 
        └── AI_USAGE.md 

## 5. Build Instructions

### Prerequisites
- Linux environment
- GCC compiler
- Make utility

### Build
bash make clean make 

## 6. Run Instructions
cd build/bin 
./router_demo 

## 7. Sample Configuration
### interfaces.json
[   
    { 
        "name":"eth0",     
        "ip_prefix":"10.0.0.1/24",     "admin_state":"up",     
        "oper_state":"up",     
        "rx_packets":1200,     
        "tx_packets":980,     
        "rx_bytes":240000,     
        "tx_bytes":180500   
    } 
] 
### static_routes.json
[   
    {     
        "prefix":"172.16.0.0/16",     
        "next_hop_ip":"192.168.1.254",     
        "egress_interface":"eth1",     
        "preference":10   
    } 
] 

## 8. Supported CLI Commands
show-interfaces            - Display interface information
show-interface <eth>       - Display information for specific interface
show-interface-stats <eth> - Display statistics for specific interface\n");
show-all-interface-stats   - Display statistics for all interfaces\n");
show-route | show-routes   - Display route information\n");
network <eth> down|up      - Update interface status\n");
route-del <net> <prefix> <nexthop> <interface>  - Delete route at index\n");
route-add <net> <prefix> <nexthop> <interface>  - Add a new route\n");
route-mod <net> <prefix> <nexthop> <interface> <preference> - Modify an existing route\n");
lookup <IPv4>              - Perform a route lookup\n");
explain-lookup <IPv4>      - Explain the route lookup process\n");
help | h | ?               - Display the help message\n");
exit | quit | q            - Exit the CLI\n");

## 9. Sample CLI Output
cli> show-route
Network            Prefix   Next Hop           Interface  Route Type     Status Preference
-------            ------   --------           ---------  ----------     ------ ----------
10.0.0.0           24       DIRECT             eth0        Connected     active 0
192.168.1.0        24       DIRECT             eth1        Connected     active 0
172.16.0.0         16       DIRECT                            Static     active 10
172.16.10.0        24       DIRECT             eth0           Static     active 20

## 10. Longest Prefix Match (LPM)

The router performs IPv4 route lookup using longest prefix match logic.

### Lookup Steps
1. Iterate through route table
2. Apply subnet mask comparison
3. Compare prefix lengths
4. Select best matching route
5. Return matched route index

Current implementation uses:
- Linear route table traversal
- Static arrays
- User-space routing simulation

## 11. Validation and Testing

### Unit Tests
Implemented UT coverage for:
- Route table functions
- Interface handling
- LPM engine
- CLI parsing
- Runtime route operations

### Test Areas
- Positive test cases
- Negative test cases
- Boundary validation
- Invalid IPv4 handling
- Route add/delete/modify
- Lookup validation
- Explain lookup validation
- Interface up/down handling

### Run Unit Tests, route testing
#### building test bin
cd ./build/
make test_router
#### running test bin
cd bin/
./test_router

### Run Unit Tests, cli testing
##### building test bin
cd ./build/
make test_cli
##### running test bin
cd bin/
./test_cli
