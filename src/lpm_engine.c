// This is the LPM engine file for project
#include "common.h"
#include "route_table.h"

/* explain_lookup_index is declared here to provide callers with the list
 * of matching route indexes for the last explain-lookup operation.
 */

uint8_t lpm_engine_lookup(uint32_t ip, _explain_t *out_best)
{
    int best_idx = -1;
    int best_prefix = -1;

    for (int i = 0; i < MAX_ROUTES; ++i) {
        if (route_table[i].is_active == INACTIVE_ROUTE)
            continue;
        _route_t *r = &route_table[i];
        if (r->interface[0] == '\0' && r->network == 0 && r->next_hop == 0)
            continue;
        int plen = r->prefix_len;
        if (plen < 0 || plen > 32)
            continue;

        uint32_t mask = (plen == 0) ? 0u :
                        (plen == 32 ? 0xFFFFFFFFu : (0xFFFFFFFFu << (32 - plen)));
        if ((ip & mask) == r->network) {
            if (plen > best_prefix) {
                best_prefix = plen;
                best_idx = i;
                if (out_best) {
                    out_best->idx = i;
                    out_best->matched_prefix = r->network;
                    out_best->prefix_len = plen;
                    strncpy(out_best->interface, r->interface,
                            sizeof(out_best->interface) - 1);
                    out_best->interface[sizeof(out_best->interface) - 1] = '\0';
                    out_best->route_type = r->route_type;
                    out_best->is_active = r->is_active;
                    out_best->preference = r->preference;
                    out_best->next_hop = r->next_hop;
                }
            }
        }
    }
    return best_idx;
}

uint8_t lpm_engine_explain_lookup(uint32_t ip, _explain_t *matched_routes_array)
{
    if (!matched_routes_array)
        return 0;

    int best_idx = lpm_engine_lookup(ip, NULL);
    if (best_idx < 0)
        return 0;

    int explain_idx = 0;

    for (int i = 0; i < MAX_ROUTES; ++i) {
        _route_t *r = &route_table[i];
        if (r->interface[0] == '\0' && r->network == 0 && r->next_hop == 0)
            continue;

        int plen = r->prefix_len;
        if (plen < 0 || plen > 32)
            continue;

        uint32_t mask = (plen == 0) ? 0u : (plen == 32 ? 0xFFFFFFFFu : (0xFFFFFFFFu << (32 - plen)));

        if ((ip & mask) == r->network) {
            matched_routes_array[explain_idx].idx = i;
            matched_routes_array[explain_idx].matched_prefix = r->network;
            matched_routes_array[explain_idx].prefix_len = plen;
            matched_routes_array[explain_idx].route_type = r->route_type;
            matched_routes_array[explain_idx].is_active = r->is_active;
            matched_routes_array[explain_idx].preference = r->preference;
            matched_routes_array[explain_idx].next_hop = r->next_hop;

            strncpy(matched_routes_array[explain_idx].interface,
                    r->interface,
                    sizeof(matched_routes_array[explain_idx].interface) - 1);
            matched_routes_array[explain_idx].interface[
                sizeof(matched_routes_array[explain_idx].interface) - 1
            ] = '\0';
            explain_idx++;
        }
    }
    return explain_idx;
}