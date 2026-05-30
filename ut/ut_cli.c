/* Consolidated CLI unit tests (canonical existing suite) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cli.h"
#include "route_table.h"
#include "interface_table.h"
#include "lpm_engine.h"
#include "common.h"

/* Helpers for redirecting stdin/stdout to temporary files for testing */
static FILE *save_stdin = NULL;
static FILE *save_stdout = NULL;

static void redirect_input_from_string(const char *input, FILE **tmpf)
{
    *tmpf = tmpfile();
    assert(*tmpf != NULL);
    fwrite(input, 1, strlen(input), *tmpf);
    fflush(*tmpf);
    fseek(*tmpf, 0, SEEK_SET);

    save_stdin = stdin;
    stdin = *tmpf; /* redirect */
}

static void capture_stdout(FILE **outf)
{
    *outf = tmpfile();
    assert(*outf != NULL);
    save_stdout = stdout;
    stdout = *outf;
}

static char *read_stream_to_string(FILE *f)
{
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (!buf) return NULL;
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    return buf;
}

static void restore_io(FILE *in_tmp, FILE *out_tmp)
{
    if (in_tmp) {
        fflush(in_tmp);
        stdin = save_stdin;
        fclose(in_tmp);
    }
    if (out_tmp) {
        fflush(out_tmp);
        stdout = save_stdout;
        fclose(out_tmp);
    }
}

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

/* Utility to run CLI with given input and return captured output string (caller must free) */
static char *run_cli_with_input(const char *input)
{
    FILE *in_tmp = NULL;
    FILE *out_tmp = NULL;
    redirect_input_from_string(input, &in_tmp);
    capture_stdout(&out_tmp);

    cli_run();

    fflush(out_tmp);
    char *out = read_stream_to_string(out_tmp);

    /* restore and close */
    restore_io(in_tmp, out_tmp);
    return out;
}

/* Tests */

static void test_valid_show_interfaces(void)
{
    clear_tables();
    strncpy(interface_table[0].name, "eth0", sizeof(interface_table[0].name)-1);
    interface_table[0].ip_addr = ip("10.0.0.1");
    interface_table[0].prefix_len = 24;
    interface_table[0].is_up = 1;

    char *out = run_cli_with_input("show-interfaces\nexit\n");
    assert(strstr(out, "Interface") != NULL);
    assert(strstr(out, "eth0") != NULL);
    printf("PASS: test_valid_show_interfaces\n");
    free(out);
}

static void test_valid_show_route(void)
{
    clear_tables();
    add_route(ip("192.168.1.0"), 24, 0, "eth0");
    char *out = run_cli_with_input("show-route\nexit\n");
    assert(strstr(out, "Network") != NULL);
    assert(strstr(out, "192.168.1.0") != NULL || strstr(out, "192.168.1.0") != NULL);
    printf("PASS: test_valid_show_route\n");
    free(out);
}

static void test_invalid_command_handling(void)
{
    clear_tables();
    char *out = run_cli_with_input("foozle\nexit\n");
    assert(strstr(out, "Unknown command") != NULL);
    printf("PASS: test_invalid_command_handling\n");
    free(out);
}

static void test_missing_arg_validation(void)
{
    clear_tables();
    char *out = run_cli_with_input("show-interface\nshow-interface-stats\nnetwork eth0\nroute-add 1.1.1.0 24\nroute-mod 1.1.1.0 24\nlookup\nexit\n");
    assert(strstr(out, "Error: missing interface name") != NULL);
    assert(strstr(out, "Error: missing arguments") != NULL || strstr(out, "Error: missing IPv4 address") != NULL);
    printf("PASS: test_missing_arg_validation\n");
    free(out);
}

static void test_invalid_ipv4_and_prefix(void)
{
    clear_tables();
    char *out = run_cli_with_input("route-add 999.999.999.999 40 1.1.1.1 eth0\nlookup 300.300.300.300\nroute-add 1.1.1.0 33 1.1.1.1 eth0\nexit\n");
    assert(strstr(out, "Invalid network address") != NULL || strstr(out, "Invalid next hop address") != NULL || strstr(out, "Error: invalid IPv4 address") != NULL);
    printf("PASS: test_invalid_ipv4_and_prefix\n");
    free(out);
}

static void test_route_add_and_duplicate(void)
{
    clear_tables();
    add_route(ip("10.10.0.0"), 16, ip("10.0.0.1"), "eth0");
    char *out = run_cli_with_input("route-add 10.10.0.0 16 10.0.0.1 eth0\nshow-route\nexit\n");
    /* add_route currently allows duplicates; expect two entries when adding via CLI */
    int count = 0;
    for (int i = 0; i < MAX_ROUTES; ++i) {
        _route_t *r = &route_table[i];
        if (r->network == ip("10.10.0.0") && r->prefix_len == 16 && r->next_hop == ip("10.0.0.1")) count++;
    }
    assert(count >= 2);
    printf("PASS: test_route_add_and_duplicate (count=%d)\n", count);
    free(out);
}

static void test_route_mod_and_del(void)
{
    clear_tables();
    add_route(ip("172.16.0.0"), 16, ip("172.16.0.1"), "eth1");
    char *out1 = run_cli_with_input("route-mod 172.16.0.0 16 172.16.0.254 eth1 10\nshow-route\nexit\n");
    /* verify modification applied (next_hop 172.16.0.254 present) */
    int found_mod = 0;
    for (int i = 0; i < MAX_ROUTES; ++i) {
        _route_t *r = &route_table[i];
        if (r->network == ip("172.16.0.0") && r->prefix_len == 16 && r->next_hop == ip("172.16.0.254")) found_mod++;
    }
    assert(found_mod >= 1);
    free(out1);

    char *out2 = run_cli_with_input("route-del 172.16.0.0 16 172.16.0.254 eth1\nshow-route\nexit\n");
    int found_after_del = 0;
    for (int i = 0; i < MAX_ROUTES; ++i) {
        _route_t *r = &route_table[i];
        if (r->network == ip("172.16.0.0") && r->prefix_len == 16 && r->next_hop == ip("172.16.0.254")) found_after_del++;
    }
    assert(found_after_del == 0);
    printf("PASS: test_route_mod_and_del\n");
    free(out2);
}

static void test_lookup_exact_and_default(void)
{
    clear_tables();
    add_route(ip("192.0.2.0"), 24, ip("1.1.1.1"), "eth0");
    add_route(0, 0, ip("10.0.0.254"), "eth0");
    char *out = run_cli_with_input("lookup 192.0.2.5\nlookup 8.8.8.8\nexit\n");
    assert(strstr(out, "Matched Route Index      :") != NULL);
    assert(strstr(out, "Outgoing Interface       :") != NULL);
    printf("PASS: test_lookup_exact_and_default\n");
    free(out);
}

static void test_explain_lookup_validation(void)
{
    clear_tables();
    add_route(ip("10.2.0.0"), 16, ip("10.0.0.1"), "eth0");
    char *out = run_cli_with_input("explain-lookup\nexplain-lookup 10.2.1.5\nexit\n");
    assert(strstr(out, "Error: missing IPv4 address") != NULL || strstr(out, "Matched route[") != NULL);
    printf("PASS: test_explain_lookup_validation\n");
    free(out);
}

static void test_network_up_down_and_interface_not_found(void)
{
    clear_tables();
    strncpy(interface_table[0].name, "eth0", sizeof(interface_table[0].name)-1);
    interface_table[0].is_up = 1;
    char *out = run_cli_with_input("network eth0 down\nnetwork ethX up\nexit\n");
    assert(strstr(out, "Interface eth0 status updated") != NULL || strstr(out, "not found") != NULL);
    printf("PASS: test_network_up_down_and_interface_not_found\n");
    free(out);
}

static void test_empty_tables_and_boundary_handling(void)
{
    clear_tables();
    char *out = run_cli_with_input("show-interfaces\nshow-route\nlookup 1.2.3.4\nexit\n");
    assert(strstr(out, "interface is not configured") != NULL || strstr(out, "routes is not configured") != NULL || strstr(out, "No matching route found") != NULL);
    printf("PASS: test_empty_tables_and_boundary_handling\n");
    free(out);
}

static void test_help_and_exit_and_whitespace(void)
{
    clear_tables();
    char *out = run_cli_with_input("   help\n\t?\nexit\n");
    assert(strstr(out, "CLI Help") != NULL);
    printf("PASS: test_help_and_exit_and_whitespace\n");
    free(out);
}

static void test_cli_unknown_and_whitespace_tab_handling(void)
{
    clear_tables();
    char *out = run_cli_with_input("\tunknowncmd    arg1   arg2\nexit\n");
    assert(strstr(out, "Unknown command") != NULL || strstr(out, "CLI Help") != NULL);
    printf("PASS: test_cli_unknown_and_whitespace_tab_handling\n");
    free(out);
}

int main(void)
{
    printf("=== Running CLI Unit Tests ===\n");
    test_valid_show_interfaces();
    test_valid_show_route();
    test_invalid_command_handling();
    test_missing_arg_validation();
    test_invalid_ipv4_and_prefix();
    test_route_add_and_duplicate();
    test_route_mod_and_del();
    test_lookup_exact_and_default();
    test_explain_lookup_validation();
    test_network_up_down_and_interface_not_found();
    test_empty_tables_and_boundary_handling();
    test_help_and_exit_and_whitespace();
    test_cli_unknown_and_whitespace_tab_handling();
    printf("=== All CLI tests executed ===\n");
    return 0;
}
