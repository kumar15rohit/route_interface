# Note:
AI-generated outputs shown below are summarized representations of interactions used during development. All generated code and suggestions were manually reviewed and validated before integration into the project.

# Study and Technical Discussions:
## The following topics were studied and discussed during project development:
 Longest Prefix Match (LPM) routing logic
 IPv4 subnet matching
 Route table organization
 CLI command handling in C
 JSON parsing in C
 Modular project structure
 User-space router design concepts
 Route lookup explanation flow
 Error handling strategy
 Interface and route configuration management
 Design documentation preparation
 Sequence diagram preparation
 AI-assisted development workflow management

# Code support:

## Task : Create sample JSON configuration files for interfaces and static routes.
### Prompt : Create a simple route configuration file in json format containing route information:
- network
- prefix length
- next hop
- interfaces
Modify only static_routes.json file.
### AI Response Summary:
Created static_routes.json
Added route entries using JSON array format
Suggested integrating with JSON loader
### Action Taken:
Reviewed generated content
Accepted JSON structure after verification
Ignored additional integration suggestions

## Task: Interface Configuration JSON Creation\
### Prompt:
Create a simple route configuration file in json format containing:
- interface name
- ip address
- prefix length
- up/down status
Modify only interfaces.json file.
### AI Response Summary:
Created sample interfaces.json
Added interface entries with status field
Suggested updating JSON loader
#### Action Taken:
Reviewed generated JSON
Accepted interface configuration
Ignored unrelated suggestions

## Task: Read JSON files and populate internal route/interface structures.
### Prompt: 
Update the read_n_store_interface_config function in C such that it should:
- read and parse interfaces.json
- use memset to reset interface structure array
- fill global interface structure array
- support error handling
- print logs while filling data
no architecture changes
modify only json_conf_loader.c
### AI Response Summary:
Implemented JSON parsing logic
Added structure reset using memset
Added validation and debug prints
Suggested additional header inclusion
### Action Taken:
Removed unnecessary newly created header file
Reviewed implementation manually
Added additional debug logs

## Task: Static Route Parsing
### Prompt:
Update the read_n_store_static_routes function in C such that it should:
- read and parse static_routes.json
- use memset to reset route structure array
- populate global route array
- support error handling
- print logs while filling data
- no architecture changes
- modify only json_conf_loader.c
no new header file
### AI Response Summary:
Added static route parsing implementation
Added validation checks
Added logging and route summary
Suggested build verification
### Action Taken:
Reviewed generated code
Added additional debug logs
ignored build/test automation suggestions

## Task: CLI run Development
### Initial Prompt:
Init CLI run which waits for user input until exit command.
Support:
- show-interfaces
- show-route
- lookup <ip>
explain-lookup <ip>
### Observation:
AI started modifying multiple files and changing overall architecture.
### Action Taken:
Stopped generated changes
Reframed prompt with stricter boundaries
### Revised Prompt:
Create a simple C function:
void cli_run(void)
Requirements:
- run infinite loop
- use fgets()
- support exit
- print entered command
- no architecture changes
modify only cli.c
### AI Response Summary:
Added simple CLI loop implementation using fgets()
Added exit handling
### Action Taken:
Reviewed generated implementation
Accepted after validation

## Task: CLI Command Parsing
### Prompt:
Update cli_loop() to support:
1. show-interfaces
2. show-routes
3. lookup <IPv4>
4. explain-lookup <IPv4>
Requirements:
- print usage if IP missing
- reject IPv6 addresses
- no architecture changes
modify only cli.c
### AI Response Summary:
Added command parsing logic
Added validation and usage messages
Suggested integration with LPM engine
### Action Taken:
Reviewed generated code
Accepted implementation
Ignored additional architecture suggestions

## Task:Interface Display Implementation
### Prompt: 
Update show_interfaces() to print:
- read and print interfaces from interface data already stored from JSON parser
- reference files:
  - route_table.h
  - interface_table.h
  - json_config_loader.c
Requirements:
- print "interface is not configured" if interface array is empty
- print values in table format similar to real CLI output
- no architecture changes
- modify only route_table_manager.c file
### AI Response Summary:
Updated show_interfaces() implementation
Printed interface information in table format
Used inet_ntop() for IPv4 formatting
Added handling for empty interface table
Suggested running build/test validation
### Action Taken:
Reviewed generated code manually
Accepted implementation after verification
Ignored build/test automation suggestion

## Route Table Display Implementation
### Prompt: 
Update show_route() to print:
- read and print routes from route_table data already stored from JSON parser
- reference files:
  - route_table.h
  - json_config_loader.c
Requirements:
- print "routes is not configured" if route table is empty
- print values in table format similar to real CLI output
- no architecture changes
modify only route_table_manager.c file
### AI Response Summary:
Updated show_route() implementation
Printed route entries in table format
Used inet_ntop() for IPv4 formatting
Removed local route table and used global route array
Suggested CLI/build testing
### Action Taken
Reviewed generated implementation
Accepted code after verification
Ignored additional build/test suggestions

## Task: Connected Route Addition
### Prompt:
Update the read_n_store_static_routes function in C such that it should:
keep exiting functionality to read and parse static_routes.json, populate global route_table array, support error handling, print logs while filling data
Update the function to check if data is there in route_table array do not update, traverse and check if values are 0 or duplicate in array index then update with new values read from json file  
- no architecture changes
- modify only json_conf_loader.c
no new header file
Update the read_n_store_interface_config function in C such that it should:
keep exiting functionality to read and parse json file
Add new functionality to populate global route_table array, by calculating network = ip & subnet_mask from interface values. Update the route_table
No architecture changes
modify only json_conf_loader.c

### AI Response Summary:
Static Route Loader
Parses static_routes.json
Avoids duplicate route insertion using:
    network
    prefix
    next hop
    interface comparison
Inserts new route into first empty slot in global route_table
Does not overwrite existing valid entries
Prints progress and error logs while updating route table
Prints summary after route loading completion

Interface Loader
Keeps existing interface parsing logic
Stores interface information in interface table
Calculates connected network using: network = ip_address & subnet_mask
 Creates connected route entry with:
    next_hop = 0
    interface = interface name
Inserts connected route into first empty route table entry
Avoids duplicate connected route insertion
Prints progress and error logs
### Action Taken
Reviewed generated code
Accepted code after review
Added logic for updating route type during connected and static route insertion
Improved debug and information logs
Ignored additional build/test suggestions

## Task: LPM logic Addition 

### PROMPT:
Update lpm_engine_lookup() function in lpm_engine.c.
LPM engine should perform longest prefix match calculation using linear search and return best matching route table index.
Upon returning from function call, modify handle_lookup() function in route_table_manager.c to print route table index using returned value.
Requirements:
Input IPv4 address received in integer format
Traverse route table
Apply subnet mask comparison
Select route with longest matching prefix
Return selected route table index
No architecture changes
Modify only: lpm_engine.c and route_table_manager.c
Update lpm_engine_explain_lookup() function in lpm_engine.c.
LPM engine should perform longest prefix match calculation using linear search and return matching route table indexes in predefined global array: explain_lookup_index
Modify handle_explain_lookup() function in route_table_manager.c to:
Read returned index array
Print matching route table entries
Requirements:
Input IPv4 address received in integer format
Traverse route table
Apply subnet mask comparison
Select route with longest matching prefix
Return selected route table index
No architecture changes
Modify only: lpm_engine.c and route_table_manager.c

### AI Response Summary:
Changes Implemented in lpm_engine.c
lpm_engine_lookup(uint32_t ip, uint32_t *next_hop)
Performs longest prefix match using linear search on route_table[]
Applies subnet mask comparison for each route entry
Selects route with longest matching prefix
Returns matching route table index:
    >= 0 for valid match
    -1 if no match found
Updates: *next_hop with matched route next hop value
lpm_engine_explain_lookup(uint32_t ip, uint32_t *next_hop)
Performs longest prefix match calculation using linear search
Finds all route entries matching best prefix length
Updates predefined global array: explain_lookup_index[]  
Unused array entries are updated with: -1
Updates: *next_hop with next hop information of first matched route
Returns:
0 on successful match
-1 if no matching route found
initialize_lpm_engine()
Initializes explain_lookup_index[] with default value: -1

 Changes Implemented in route_table_manager.c
handle_lookup()
Calls: lpm_engine_lookup()
Prints:
next hop
matched route table index
Returns lookup status
handle_explain_lookup()
Calls:lpm_engine_explain_lookup()
Prints:
matched route table indexes
route information for matched entries
Notes
All changes were restricted to: lpm_engine.c and route_table_manager.c
Longest prefix match logic uses:
linear search
subnet mask comparison
prefix length comparison
Route with largest prefix length is selected as best match

### Action Taken:
Reviewed generated code
Accepted code after review
Modified lpm_engine_explain_lookup() return handling to return array address instead of exposing global array directly outside engine module
Added proper handling in calling function
Reviewed longest prefix match logic, further testing pending
Improved debug and information logs
Ignored additional build and test suggestions

# Explain Lookup Debugging and Fix
### Issue Observed: 
explain-lookup command was not printing all possible matching routes correctly. Output was showing only the final LPM matched route.
#### RCA : 
Added debug logs inside lpm_engine_explain_lookup() and observed the function was traversing the route table multiple times:
First loop: Finding LPM and Calculating best prefix length
Second loop: Updating explain_lookup_index[] array
#### Issue found: 
Even during the second loop, the code was still checking the LPM condition, causing only the best matched route entry to be stored instead of all expected matching routes.\
FIX: Used AI for fixing the issue after analysis
#### AI Prompt Used: 
Debug and simplify the function lpm_engine_explain_lookup() in lpm_engine.c.
Current implementation looks like it is traversing the route table multiple times and checking prefix lengths repeatedly. Rewrite the function using a simpler and optimized approach.
Requirements:
- Reuse the existing lpm_engine_lookup() function if possible to get the best matching route index.
- Use a simple single loop to traverse the route table.
- Compare the input uint32_t ip against the route table entries using subnet mask logic.
- Store only the matched/best route index in explain_lookup_index array.
- Clear/reset explain_lookup_index before filling.
- Return explain_lookup_index array from the function.
- Keep existing functionality and output behaviour unchanged.
- No architecture changes.
- Modify only lpm_engine.c.
- Add proper debug logs while matching routes.
#### AI Response Summary:
AI simplified the lpm_engine_explain_lookup() function by:
Clearing explain_lookup_index[]
Reusing lpm_engine_lookup() for LPM calculation
Avoiding multiple route table traversals
Storing only the best matched route index in explain_lookup_index[0]
Adding debug logs for matched route details
Returning the explain_lookup_index[] array
#### Action Taken:
Reviewed generated code
Verified route lookup logic manually
Accepted optimized implementation after review
Added additional debug logs for validation
Ignored additional build/test suggestions from AI

# Enhancement Discussion and Design Consideration
Used AI for discussing possible routing and interface state enhancement design improvements.
## Enhancement Ideas
Discussed adding a route status flag inside the route table structure to support active/inactive route handling based on interface operational state.

#### Proposed Design
Add a route status field in route structure to indicate:
Active route
Inactive route
During parsing of interfaces.json:
Interface operational status (up/down) will be checked
While updating connected routes in route table, corresponding route status flag will also be updated
During route lookup:
LPM engine will skip inactive routes
Only active routes will participate in route selection

## Additional CLI Enhancement Discussed
Discussed adding dynamic interface state commands such as: 
network eth1 down
network eth1 up
### These commands would:
Update interface operational state
Update corresponding route table active/inactive flags
Automatically affect route lookup behaviour without deleting or re-adding routes
## Design Benefit
#### This approach avoids:
Deleting routes from route table
Recreating routes during interface state changes

This provides a cleaner and scalable design for future dynamic route/interface state handling.

# Unit Test Development Support using AI
## UT Implementation – Routing and LPM
### Prompt:
Generate Unit Test (UT) implementation in C for all Route Table Manager and LPM related functions in the virtual router project.
Requirements:
Use existing project architecture only
No major architecture changes
Do not modify existing APIs or function prototypes
Add UT code in separate test file(s) only
Reuse existing headers, structures, and helper APIs
Use standard C and assert() based validation
Add positive, negative, and boundary test cases
Add readable PASS/FAIL logs
Clear global route_table and interface_table before every test
Populate mock route/interface entries for validation
Functions to cover:
add_route()
delete_route()
modify_route()
handle_lookup()
handle_explain_lookup()
lpm_engine_lookup()
lpm_engine_explain_lookup()
get_interface_status()
update_interface_status()
Test scenarios:
Connected and static route addition
Duplicate route validation
Route delete and modify validation
Exact match and longest prefix match lookup
Default route lookup
Empty route table handling
Prefix boundary validation (/0 and /32)
Explain lookup validation
Route table full condition
Active/inactive route validation
Preference handling validation
## AI Response Summary:
AI generated routing UT scaffolding and helper logic in separate file ut_routing.c.
Generated functionality:
Added UT initialization and cleanup handling
Reused existing project headers and APIs
Added assert() based validation and readable PASS logs
Added helper APIs for populating mock route and interface entries
Added routing and LPM validation test cases
Added boundary and negative test scenarios
Generated test coverage:
Add connected route
Add static route
Duplicate route handling
Route delete validation
Route modify validation
LPM exact match and longest-prefix match
Default route and empty table handling
Prefix boundary values (/0 and /32)
Explain lookup validation
Route table full condition
## Actions Taken:
Reviewed generated UT code manually
Verified compatibility with existing project architecture
Validated route_table and interface_table usage
Updated UT expectations based on actual project behavior
Improved logs and readability
Verified LPM return index logic manually
Accepted UT scaffolding after review
Ignored optional framework migration suggestions

# Unit Test (UT) Implementation – CLI
## Prompt:
Generate Unit Test (UT) implementation in C for all CLI related functions in the virtual router project.
Requirements:
Use existing project architecture only
No major architecture changes
Do not modify existing APIs or CLI flow
Add UT code in separate test file(s) only
Reuse existing headers, route table, and interface table
Use standard C and assert() based validation
Add positive, negative, and boundary test cases
Add readable PASS/FAIL logs
Mock stdin/stdout wherever required
Validate command parsing and argument handling
CLI features to cover:
show-interfaces
show-interface 
show-interface-stats 
show-all-interface-stats
show-route / show-routes
lookup 
explain-lookup 
network  up/down
route-add
route-mod
route-del
help
exit/quit
invalid command handling
Test scenarios:
Valid and invalid CLI commands
Missing argument handling
Invalid IPv4 validation
Invalid prefix validation
Duplicate route handling
Lookup and explain-lookup validation
Default route lookup
Empty table handling
Interface-not-found handling
Route-not-found handling
CLI whitespace/tab handling
Extra argument handling

## AI Response Summary:
AI generated CLI unit test scaffolding in separate file ut_cli.c.
Generated functionality:
Added stdin/stdout redirection for CLI testing
Reused existing CLI APIs and helper functions
Added assert() based validation and readable PASS logs
Added UT setup and cleanup logic
Added mock CLI command execution handling
Generated test coverage:
show-interfaces validation
show-route validation
Invalid command handling
Missing argument validation
Invalid IPv4 and prefix handling
route-add and duplicate route validation
route-mod and route-del validation
lookup and explain-lookup validation
network up/down handling
Empty table and boundary condition handling
help and exit command validation
CLI whitespace and tab handling
## Actions Taken:
Reviewed generated CLI UT code manually
Verified compatibility with existing CLI architecture
Updated UT expectations based on actual CLI behavior
Improved readability and logging
Validated CLI parsing flow manually
Accepted generated UT scaffolding after review
Ignored optional framework migration suggestions

# AI Usage Summary
## Context
AI assistance was used during debugging, refactoring discussions, and unit test stabilization for the routing/LPM project.
The initial issue originated from CLI unit test failures caused by direct `printf()` usage inside lower routing layers, which made stdout-based UT validation unstable.
# Initial AI Suggestion (Rejected)
AI initially suggested introducing a complete logging framework to replace `printf()` usage across the project.
This approach was rejected because:
- it introduced excessive architectural changes,
- increased regression risk,
- impacted existing CLI/unit test behavior,
- and was unnecessary for the actual root cause.
Instead, a smaller and more controlled redesign was implemented.

# Final Implemented Approach
## 1. Separation of Calculation and Printing
Printing responsibility was moved out of routing/LPM logic and centralized at the CLI layer.
### Previous design
Lower layers performed direct printing:
- route_table_manager.c
- LPM lookup paths
- interface handling paths
### Updated design
Lower layers now:
- compute results,
- fill structures/arrays,
- return data to caller.
CLI layer became responsible for user-visible output formatting.
## 2. New `print_utils` Module
A dedicated `print_utils` module was introduced to centralize:
- route display formatting,
- explain lookup formatting,
- interface output formatting,
- CLI-visible printing.
Benefits:
- cleaner separation of concerns,
- deterministic stdout handling,
- easier CLI unit testing,
- reusable formatting logic.
## 3. Interface Layer Refactor
A new interface handling layer/file was introduced.
Instead of directly printing interface/route information:
- APIs now return table pointers or result structures,
- CLI consumes returned data,
- printing happens only through print utilities.
This reduced tight coupling between:
- routing logic,
- interface logic,
- CLI output behavior.
## 4. Explain Lookup Redesign
The explain lookup implementation was redesigned.
### Previous behavior
- returned limited/best-match-only information,
- relied on direct printing.
### Updated behavior
- introduced dedicated explain result structures,
- LPM logic fills explain structures during lookup,
- caller controls presentation/output,
- explain data can now be unit tested independently from stdout.
## 5. Unit Test Updates
AI was later used to assist with UT migration.
Initial AI-generated UT changes caused:
- duplicate test cases,
- unnecessary new test files,
- compilation issues,
- mismatched APIs.
Those generated changes were reviewed manually and corrected.
Final approach:
- updated existing UT files only,
- aligned tests with current APIs and behavior,
- preserved existing UT framework and runner structure,
- minimized unnecessary refactoring.
# Engineering Decisions
The final implementation intentionally prioritized:
- minimal regression risk,
- deterministic CLI testing,
- separation of business logic from presentation,
- controlled API evolution,
- review-friendly changes.
AI suggestions were treated as guidance only and were manually evaluated before integration.

# AI Usage Approach
AI tools were used as development assistance utilities during implementation of the virtual router project. The primary usage areas included:
## Implementation guidance for routing and CLI logic
## JSON parsing support and data extraction handling
## CLI command parsing and input handling suggestions
## Debugging assistance for routing, parsing, and lookup issues
## Unit Test (UT) scaffolding generation
## LPM (Longest Prefix Match) logic discussion and refinement
## Enhancement brainstorming and feature design discussions
## Documentation templates and formatting support
## Logging and error handling improvement suggestions

AI-generated outputs were primarily used to accelerate repetitive development tasks, explore implementation approaches, and assist during debugging activities.

All generated code, logic suggestions, and documentation outputs were manually reviewed, validated, modified where required, and tested before acceptance into the project.

Final implementation decisions, architecture alignment, debugging validation, feature integration, and behavior verification were manually performed during development.