#ifndef LPM_ENGINE_H
#define LPM_ENGINE_H
#include "common.h"
#include "route_table.h"

typedef struct explain_t
{
    int idx;
    uint32_t matched_prefix;
    int prefix_len;
    char interface[16];
    uint8_t route_type;
    uint8_t is_active;
    int preference;
    u_int32_t next_hop;
} _explain_t;

uint8_t lpm_engine_lookup(uint32_t ip, _explain_t *out_best);
uint8_t lpm_engine_explain_lookup(uint32_t ip, _explain_t *out_explain_array);
void initialize_lpm_engine(void);

#endif // LPM_ENGINE_H 