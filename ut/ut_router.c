#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "route_table.h"
#include "interface_table.h"
#include "lpm_engine.h"
#include "common.h"

/* Consolidated canonical router/LPM unit tests
 * - single set of helpers
 * - use explain-based LPM API and select best-match in tests
 */

static void clear_tables(void)
{
    memset(route_table, 0, sizeof(route_table));
    memset(interface_table, 0, sizeof(interface_table));
}

static uint32_t ip(const char *s)
{
    uint32_t v;
    int rc = parse_ip_address(s, &v);
    assert(rc == 0);
    return v;
}

static int find_route(uint32_t network, uint8_t prefix_len, uint32_t next_hop, const char *iface)
{
    for (int i = 0; i < MAX_ROUTES; ++i) {
        _route_t *r = &route_table[i];
        if (r->network == network && r->prefix_len == prefix_len && r->next_hop == next_hop && strcmp(r->interface, iface) == 0) return i;
    }
    return -1;
}

static void test_add_and_delete_routes(void)
{
    clear_tables();
    add_route(ip("192.0.2.0"), 24, ip("192.0.2.1"), "eth0");
    int idx = find_route(ip("192.0.2.0"), 24, ip("192.0.2.1"), "eth0");
    assert(idx >= 0);
    delete_route(ip("192.0.2.0"), 24, ip("192.0.2.1"), "eth0");
    int idx2 = find_route(ip("192.0.2.0"), 24, ip("192.0.2.1"), "eth0");
    assert(idx2 == -1);
    printf("PASS: test_add_and_delete_routes\n");
}

static void test_lpm_longest_prefix_choice(void)
{
    clear_tables();
    add_route(ip("10.0.0.0"), 8, ip("10.0.0.1"), "eth0");
    add_route(ip("10.1.0.0"), 16, ip("10.0.0.2"), "eth0");
    add_route(ip("10.1.2.3"), 32, ip("10.0.0.3"), "eth0");

    _explain_t buf[MAX_ROUTES];
    int cnt = lpm_engine_explain_lookup(ip("10.1.2.3"), buf);
    assert(cnt >= 1);
    int best = 0;
    for (int i = 1; i < cnt; ++i) if (buf[i].prefix_len > buf[best].prefix_len) best = i;
    assert(route_table[buf[best].idx].prefix_len == 32);
    printf("PASS: test_lpm_longest_prefix_choice\n");
}

static void test_default_route_behavior(void)
{
    clear_tables();
    _explain_t buf[MAX_ROUTES];
    int cnt0 = lpm_engine_explain_lookup(ip("203.0.113.5"), buf);
    assert(cnt0 == 0);
    add_route(0, 0, ip("10.0.0.254"), "eth0");
    int cnt = lpm_engine_explain_lookup(ip("203.0.113.5"), buf);
    assert(cnt >= 1);
    int best = 0;
    for (int i = 1; i < cnt; ++i) if (buf[i].prefix_len > buf[best].prefix_len) best = i;
    assert(route_table[buf[best].idx].prefix_len == 0);
    printf("PASS: test_default_route_behavior\n");
}

static void test_invalid_and_boundary_cases(void)
{
    clear_tables();
    uint32_t out;
    assert(parse_ip_address("invalid.ip", &out) != 0);
    /* prefix > 32 should be ignored by LPM - adding but LPM must prefer valid entries */
    add_route(ip("172.31.0.0"), 40, ip("1.2.3.4"), "eth0");
    add_route(ip("172.31.1.0"), 24, ip("1.2.3.5"), "eth0");
    _explain_t buf[MAX_ROUTES];
    int cnt = lpm_engine_explain_lookup(ip("172.31.1.5"), buf);
    assert(cnt >= 1);
    int best = 0;
    for (int i = 1; i < cnt; ++i) if (buf[i].prefix_len > buf[best].prefix_len) best = i;
    assert(route_table[buf[best].idx].prefix_len == 24);
    printf("PASS: test_invalid_and_boundary_cases\n");
}

int main(void)
{
    printf("=== Running Consolidated Router/LPM Unit Tests ===\n");
    test_add_and_delete_routes();
    test_lpm_longest_prefix_choice();
    test_default_route_behavior();
    test_invalid_and_boundary_cases();
    printf("=== Router/LPM tests complete ===\n");
    return 0;
}
