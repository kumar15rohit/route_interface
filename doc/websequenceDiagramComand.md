title Virtual Router CLI and LPM Flow

participant User
participant CLI
participant RouteTableManager
participant PrintUtils
participant JsonConfigLoader
participant InterfaceTable
participant RouteTable
participant LPMEngine

User->JsonConfigLoader: Read interfaces.json
JsonConfigLoader->InterfaceTable: Populate interface_table[]
JsonConfigLoader->RouteTable: Generate connected routes
JsonConfigLoader->RouteTable: Update route type = CONNECTED
JsonConfigLoader->RouteTable: Update active/inactive state

User->JsonConfigLoader: Read static_routes.json
JsonConfigLoader->RouteTable: Populate static routes
JsonConfigLoader->RouteTable: Update route type = STATIC
JsonConfigLoader->RouteTable: Update preference/next-hop/interface info

User->CLI: Start CLI loop
loop CLI Loop
    User->CLI: Enter command

    alt show-interfaces
        CLI->RouteTableManager: show_interfaces()
        RouteTableManager->InterfaceTable: Fetch interface data
        RouteTableManager->PrintUtils: Print interface table
    end

    alt show-route
        CLI->RouteTableManager: show_route()
        RouteTableManager->RouteTable: Fetch route entries
        RouteTableManager->PrintUtils: Print route table
    end

    alt lookup <IPv4>
        CLI->RouteTableManager: handle_lookup(ip)
        RouteTableManager->LPMEngine: lpm_engine_lookup(ip)

        loop Traverse Route Table
            LPMEngine->RouteTable: Compare subnet mask
            LPMEngine->RouteTable: Skip inactive routes
            LPMEngine->LPMEngine: Select longest prefix
        end

        LPMEngine-->RouteTableManager: Return best matching route info
        RouteTableManager->PrintUtils: Print lookup result
    end

    alt explain-lookup <IPv4>
        CLI->RouteTableManager: handle_explain_lookup(ip)
        RouteTableManager->LPMEngine: lpm_engine_explain_lookup(ip)

        loop Traverse Route Table
            LPMEngine->RouteTable: Compare subnet mask
            LPMEngine->RouteTable: Store explain lookup results
        end

        LPMEngine-->RouteTableManager: Return explain result structures
        RouteTableManager->PrintUtils: Print explain lookup result
    end

    alt route-add / route-del / route-mod
        CLI->RouteTableManager: Route operation request
        RouteTableManager->RouteTable: Update route entries
        RouteTableManager->PrintUtils: Print operation status
    end

    alt network <ifname> up/down
        CLI->RouteTableManager: Update interface state
        RouteTableManager->InterfaceTable: Update operational state
        RouteTableManager->RouteTable: Update active/inactive routes
        RouteTableManager->PrintUtils: Print interface state update
    end

    alt invalid command
        CLI->PrintUtils: Print validation/error message
    end

    alt exit
        CLI->PrintUtils: Print exit message
        CLI-->User: Exit application
    end
end