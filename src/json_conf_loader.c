// This is the JSON configuration loader file for project

#include "json_conf_loader.h"

static char *read_file(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }
    long sz = ftell(f);
    if (sz < 0) {
        fclose(f);
        return NULL;
    }
    rewind(f);
    char *buf = malloc(sz + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    if (fread(buf, 1, sz, f) != (size_t)sz) {
        free(buf);
        fclose(f);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    return buf;
}

/* Very small ad-hoc JSON parser sufficient for the expected interfaces.json format.
 * Expects an array of objects with keys: name (string), ip_address (string), prefix_length (number), status (string: up/down)
 */
int read_n_store_interface_config(const char *filename)
{
    if (!filename) {
        printf("[ERROR] filename %s not found on current directory\n", filename);
        return -1;
    }

    char *content = read_file(filename);
    if (!content) {
        printf("[ERROR] failed to read file %s\n", filename);
        return -1;
    }

        /* reset global interface table */
        memset(interface_table, 0, sizeof(interface_table));

    const char *p = content;
    int idx = 0;

    while ((p = strchr(p, '{')) != NULL) {
        if (idx >= MAX_INTERFACES) {
            printf("[ERROR] interface array full (max %d)\n", MAX_INTERFACES);
            break;
        }

        const char *obj_end = strchr(p, '}');
        if (!obj_end) break;

        char name_buf[32] = {0};
        char ip_text[64] = {0};
        int prefix = -1;
        char status_buf[16] = {0};
        uint8_t is_up = 0;
        uint8_t admin_up = 0;
        uint64_t rx_packets = 0, tx_packets = 0, rx_bytes = 0, tx_bytes = 0;

        /* search for keys within the object */
        const char *q = p;
        while (q < obj_end) {
            /* find next quoted key */
            const char *key_start = strchr(q, '"');
            if (!key_start || key_start >= obj_end) break;
            const char *key_end = strchr(key_start + 1, '"');
            if (!key_end || key_end >= obj_end) break;
            size_t key_len = key_end - key_start - 1;
            char key[64];
            if (key_len >= sizeof(key)) key_len = sizeof(key) - 1;
            memcpy(key, key_start + 1, key_len);
            key[key_len] = '\0';

            /* find ':' after key */
            const char *colon = strchr(key_end, ':');
            if (!colon || colon >= obj_end) break;
            const char *val = colon + 1;
            while (val < obj_end && (*val == ' ' || *val == '\t' || *val == '\n' || *val == '\r')) val++;

            if (strncmp(key, "name", 5) == 0) {
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;
                        if (vlen >= sizeof(name_buf)) vlen = sizeof(name_buf) - 1;
                        memcpy(name_buf, val + 1, vlen);
                        name_buf[vlen] = '\0';
                    }
                } else {
                    printf("[ERROR] name value is not a string\n");
                }
            } else if (strncmp(key, "ip_prefix", 9) == 0) {
                /* new combined field: "ip_prefix": "10.0.0.1/24" */
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;
                        if (vlen >= sizeof(ip_text)) vlen = sizeof(ip_text) - 1;
                        /* copy then split */
                        char tmp[64];
                        size_t copy_len = vlen < sizeof(tmp)-1 ? vlen : sizeof(tmp)-1;
                        memcpy(tmp, val + 1, copy_len);
                        tmp[copy_len] = '\0';
                        char *slash = strchr(tmp, '/');
                        if (slash) {
                            *slash = '\0';
                            strncpy(ip_text, tmp, sizeof(ip_text)-1);
                            prefix = atoi(slash + 1);
                        } else {
                            strncpy(ip_text, tmp, sizeof(ip_text)-1);
                        }
                    }
                }
            } else if (strncmp(key, "ip_address", 11) == 0) {
                /* fallback: legacy separate fields */
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;
                        if (vlen >= sizeof(ip_text)) vlen = sizeof(ip_text) - 1;
                        memcpy(ip_text, val + 1, vlen);
                        ip_text[vlen] = '\0';
                    }
                } else {
                    /* also accept bareword */
                    const char *vend = val;
                    while (vend < obj_end && *vend != ',' && *vend != '\n' && *vend != '\r' && *vend != '}') vend++;
                    size_t vlen = vend - val;
                    if (vlen >= sizeof(ip_text)) vlen = sizeof(ip_text) - 1;
                    memcpy(ip_text, val, vlen);
                    /* trim */
                    char *t = ip_text;
                    while (*t == ' ' || *t == '\t') t++;
                    if (t != ip_text) memmove(ip_text, t, strlen(t) + 1);
                }
            } else if (strncmp(key, "prefix_length", 14) == 0) {
                /* legacy separate prefix field */
                prefix = atoi(val);
            } else if (strncmp(key, "oper_state", 11) == 0) {
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;
                        if (vlen >= sizeof(status_buf)) vlen = sizeof(status_buf) - 1;
                        memcpy(status_buf, val + 1, vlen);
                        status_buf[vlen] = '\0';
                    }
                } else {
                    const char *vend = val;
                    while (vend < obj_end && *vend != ',' && *vend != '\n' && *vend != '\r' && *vend != '}') vend++;
                    size_t vlen = vend - val;
                    if (vlen >= sizeof(status_buf)) vlen = sizeof(status_buf) - 1;
                    memcpy(status_buf, val, vlen);
                    /* trim */
                    char *t = status_buf;
                    while (*t == ' ' || *t == '\t') t++;
                    if (t != status_buf) memmove(status_buf, t, strlen(t) + 1);
                }
                is_up = (strcasecmp(status_buf, "up") == 0 || strcasecmp(status_buf, "1") == 0) ? 1 : 0;                 
            }
            else if (strncmp(key, "admin_state", 12) == 0) {
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;
                        if (vlen >= sizeof(status_buf)) vlen = sizeof(status_buf) - 1;
                        memcpy(status_buf, val + 1, vlen);
                        status_buf[vlen] = '\0';
                    }
                } else {
                    const char *vend = val;
                    while (vend < obj_end && *vend != ',' && *vend != '\n' && *vend != '\r' && *vend != '}') vend++;
                    size_t vlen = vend - val;
                    if (vlen >= sizeof(status_buf)) vlen = sizeof(status_buf) - 1;
                    memcpy(status_buf, val, vlen);
                    /* trim */
                    char *t = status_buf;
                    while (*t == ' ' || *t == '\t') t++;
                    if (t != status_buf) memmove(status_buf, t, strlen(t) + 1);
                }
                admin_up = (strcasecmp(status_buf, "up") == 0 || strcasecmp(status_buf, "1") == 0) ? 1 : 0;
            }
            else if(strcmp(key, "rx_packets") == 0) {   
                 rx_packets = strtoull(val, NULL, 10);            
            }
            else if(strcmp(key, "tx_packets") == 0) {
                 tx_packets = strtoull(val, NULL, 10);
            }
            else if(strcmp(key, "rx_bytes") == 0) {
                 rx_bytes = strtoull(val, NULL, 10);
            }
            else if(strcmp(key, "tx_bytes") == 0) {
                 tx_bytes = strtoull(val, NULL, 10);
            }

            q = key_end + 1;
        }

        /* validate required fields */
        if (name_buf[0] == '\0') {
            printf("[ERROR] interface entry missing 'name'\n");
            p = obj_end + 1;
            continue;
        }
        if (ip_text[0] == '\0') {
            printf("[ERROR] interface '%s' missing 'ip_address'\n", name_buf);
            p = obj_end + 1;
            continue;
        }
        if (prefix < 0 || prefix > 32) {
            printf("[ERROR] interface '%s' has invalid prefix_length '%d'\n", name_buf, prefix);
            p = obj_end + 1;
            continue;
        }

        /* parse IP address (IPv4 only) */
        struct in_addr a4;
        struct in6_addr a6;
        if (inet_pton(AF_INET, ip_text, &a4) != 1) {
            if (inet_pton(AF_INET6, ip_text, &a6) == 1) {
                printf("[ERROR] interface '%s' ip_address is IPv6; only IPv4 supported: %s\n", name_buf, ip_text);
            } else {
                printf("[ERROR] interface '%s' has invalid ip_address: %s\n", name_buf, ip_text);
            }
            p = obj_end + 1;
            continue;
        }

        /* store into global array */
        _interface_t *it = &interface_table[idx];
        strncpy(it->name, name_buf, sizeof(it->name) - 1);
        it->ip_addr = ntohl(a4.s_addr);
        it->prefix_len = (uint8_t)prefix;
        it->admin_up = admin_up;
        it->is_up = is_up;
        it->rx_packets = rx_packets;
        it->tx_packets = tx_packets;
        it->rx_bytes = rx_bytes;
        it->tx_bytes = tx_bytes;

        printf("[INFO] stored interface[%d]: name=%s ip=%s/%d status=%s\n",
               idx, it->name, ip_text, it->prefix_len, it->is_up ? "up" : "down");

        /* create connected route for this interface (network = ip & mask) */
        if (it->ip_addr != 0) {
            uint8_t plen = it->prefix_len;
            uint32_t mask = 0;
            if (plen == 0) mask = 0;
            else mask = plen >= 32 ? 0xFFFFFFFFu : (0xFFFFFFFFu << (32 - plen));

            uint32_t network = it->ip_addr & mask;

            /* check duplicate */
            int exists = 0;
            for (int j = 0; j < MAX_ROUTES; ++j) {
                if (route_table[j].interface[0] == '\0') continue;
                if (route_table[j].network == network && route_table[j].prefix_len == plen && strcmp(route_table[j].interface, it->name) == 0) {
                    exists = 1;
                    break;
                }
            }

            if (!exists) {
                /* find first empty slot */
                int placed = 0;
                for (int j = 0; j < MAX_ROUTES; ++j) {
                    if (route_table[j].interface[0] == '\0' && route_table[j].network == 0 && route_table[j].next_hop == 0) {
                        route_table[j].network = network;
                        route_table[j].prefix_len = plen;
                        route_table[j].next_hop = 0; /* connected route */
                        route_table[j].route_type = ROUTE_TYPE_CONNECTED;
                        route_table[j].is_active = it->is_up ? ACTIVE_ROUTE : INACTIVE_ROUTE; /* route active if interface is up */
                        strncpy(route_table[j].interface, it->name, sizeof(route_table[j].interface) - 1);
                        printf("[INFO] created connected route[%d]: ", j);
                        char netbuf[INET_ADDRSTRLEN] = "-";
                        struct in_addr na = { htonl(network) };
                        inet_ntop(AF_INET, &na, netbuf, sizeof(netbuf));
                        printf("%s/%u dev %s\n", netbuf, (unsigned)plen, route_table[j].interface);
                        placed = 1;
                        break;
                    }
                }
                if (!placed) {
                    printf("[ERROR] no space to add connected route for interface %s\n", it->name);
                }
            } else {
                printf("[INFO] connected route for %s already exists, skipping\n", it->name);
            }
        }

        idx++;
        p = obj_end + 1;
    }

    free(content);
    printf("[INFO] read_n_store_interface_config: %d interfaces loaded from %s\n", idx, filename);
    return 0;
}

int read_n_store_static_routes(const char *filename)
{
    if (!filename) {
        printf("[ERROR] filename %s not found on current directory\n", filename);
        return -1;
    }

    char *content = read_file(filename);
    if (!content) {
        printf("[ERROR] failed to read file %s\n", filename);
        return -1;
    }

    const char *p = content;
    int added = 0;

    while ((p = strchr(p, '{')) != NULL) {
        /* insertion will use first free slot; do not assume sequential idx */

        const char *obj_end = strchr(p, '}');
        if (!obj_end) break;

        char network_text[64] = {0};
        int prefix = -1;
        char next_hop_text[64] = {0};
        char iface_buf[32] = {0};
        uint8_t preference = 0;

        const char *q = p;
        while (q < obj_end) {
            const char *key_start = strchr(q, '"');
            if (!key_start || key_start >= obj_end) break;
            const char *key_end = strchr(key_start + 1, '"');
            if (!key_end || key_end >= obj_end) break;
            size_t key_len = key_end - key_start - 1;
            char key[64];
            if (key_len >= sizeof(key)) key_len = sizeof(key) - 1;
            memcpy(key, key_start + 1, key_len);
            key[key_len] = '\0';

            const char *colon = strchr(key_end, ':');
            if (!colon || colon >= obj_end) break;
            const char *val = colon + 1;
            while (val < obj_end && (*val == ' ' || *val == '\t' || *val == '\n' || *val == '\r')) val++;

            if (strncmp(key, "prefix", 6) == 0) {
                /* new combined field: "prefix": "172.16.0.0/16" */
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;
                        if (vlen >= sizeof(network_text)) vlen = sizeof(network_text) - 1;
                        char tmp[64];
                        size_t copy_len = vlen < sizeof(tmp)-1 ? vlen : sizeof(tmp)-1;
                        memcpy(tmp, val + 1, copy_len);
                        tmp[copy_len] = '\0';
                        char *slash = strchr(tmp, '/');
                        if (slash) {
                            *slash = '\0';
                            strncpy(network_text, tmp, sizeof(network_text)-1);
                            prefix = atoi(slash + 1);
                        } else {
                            strncpy(network_text, tmp, sizeof(network_text)-1);
                        }
                    }
                }
            } else if (strncmp(key, "network", 8) == 0 || strncmp(key, "destination", 12) == 0) {
                /* legacy separate field */
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;
                        if (vlen >= sizeof(network_text)) vlen = sizeof(network_text) - 1;
                        memcpy(network_text, val + 1, vlen);
                        network_text[vlen] = '\0';
                    }
                }
            } else if (strncmp(key, "prefix_length", 14) == 0) {
                prefix = atoi(val);
            } else if (strncmp(key, "next_hop", 8) == 0) {
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;
                        if (vlen >= sizeof(next_hop_text)) vlen = sizeof(next_hop_text) - 1;
                        memcpy(next_hop_text, val + 1, vlen);
                        next_hop_text[vlen] = '\0';
                    }
                }
            } else if (strncmp(key, "next_hop_ip", 11) == 0) {
                /* new name for next hop */
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;
                        if (vlen >= sizeof(next_hop_text)) vlen = sizeof(next_hop_text) - 1;
                        memcpy(next_hop_text, val + 1, vlen);
                        next_hop_text[vlen] = '\0';
                    }
                }

            }
            else if (strncmp(key, "preference", 10) == 0) {
              preference = atoi(val);
            }else if (strncmp(key, "interfaces", 10) == 0) {

                /* old format: "interfaces": ["eth0"] */
                const char *arr_start = strchr(val, '[');
                if (arr_start && arr_start < obj_end) {
                    const char *s = arr_start + 1;

                    while (s < obj_end && (*s == ' ' || *s == '\t' ||
                    *s == '\n' || *s == '\r' || *s == ',')) {

                        s++;
                    }
                    if (*s == '"') {
                        const char *vend = strchr(s + 1, '"');

                        if (vend && vend < obj_end) {
                            size_t vlen = vend - s - 1;
                            if (vlen >= sizeof(iface_buf)) {
                                vlen = sizeof(iface_buf) - 1;
                            }
                            memcpy(iface_buf, s + 1, vlen);
                            iface_buf[vlen] = '\0';
                        }

                    }

                }
            }else if (strncmp(key, "egress_interface", 16) == 0) {
                /* new format: "egress_interface":"eth0" */
                if (*val == '"') {
                    const char *vend = strchr(val + 1, '"');
                    if (vend && vend < obj_end) {
                        size_t vlen = vend - val - 1;\
                        if (vlen >= sizeof(iface_buf)) {\
                            vlen = sizeof(iface_buf) - 1;
                        }
                        memcpy(iface_buf, val + 1, vlen);
                        iface_buf[vlen] = '\0';\
                    }

                }
            }
            
            q = key_end + 1;
        }

        struct in_addr net4;
        struct in_addr nh4;
        if (inet_pton(AF_INET, network_text, &net4) != 1) {
            printf("[ERROR] invalid IPv4 network: %s\n", network_text);
            p = obj_end + 1;
            continue;
        }
        if (inet_pton(AF_INET, next_hop_text, &nh4) != 1 && strcmp(next_hop_text, "DIRECT") != 0 && next_hop_text[0] != '\0') {
            printf("[ERROR] invalid IPv4 next_hop: %s\n", next_hop_text);
            p = obj_end + 1;
            continue;
        }
        uint32_t net_host = ntohl(net4.s_addr);
        uint32_t nh_host = 0;
        if(strcmp(next_hop_text, "DIRECT") != 0 && next_hop_text[0] != '\0') 
        {
            nh_host = ntohl(nh4.s_addr);
        }
        /* validate */
        if (network_text[0] == '\0') {
            printf("[ERROR] route entry missing 'network'\n");
            p = obj_end + 1;
            continue;
        }
        if (prefix < 0 || prefix > 32) {
            printf("[ERROR] route for network %s has invalid prefix_length %d\n", network_text, prefix);
            p = obj_end + 1;
            continue;
        }
#if 0
        if (next_hop_text[0] == '\0') {
            printf("[ERROR] route for network %s missing 'next_hop'\n", network_text);
            p = obj_end + 1;
            continue;
        }
        if (iface_buf[0] == '\0') {
            printf("[ERROR] route for network %s missing interface\n", network_text);
            p = obj_end + 1;
            continue;
        }
#endif
        if (iface_buf[0] == '\0' && nh_host == 0) {
             printf("[ERROR] DIRECT route missing egress_interface\n");
            p = obj_end + 1;
            continue;
        }

        /* check for duplicate */
        int dup = 0;
        for (int j = 0; j < MAX_ROUTES; ++j) {
            if (route_table[j].interface[0] == '\0') continue;
            if (route_table[j].network == net_host && route_table[j].prefix_len == (uint8_t)prefix && route_table[j].next_hop == nh_host && strcmp(route_table[j].interface, iface_buf) == 0) {
                dup = 1;
                break;
            }
        }
        if (dup) {
            printf("[INFO] duplicate route for %s/%d via %s dev %s, skipping\n", network_text, prefix, next_hop_text, iface_buf);
            p = obj_end + 1;
            continue;
        }

        /* find first empty slot */
        int placed = 0;
        for (int j = 0; j < MAX_ROUTES; ++j) {
            if (route_table[j].interface[0] == '\0' && route_table[j].network == 0 && route_table[j].next_hop == 0) {
                route_table[j].network = net_host;
                route_table[j].prefix_len = (uint8_t)prefix;
                if (nh_host == 0) {
                     route_table[j].route_type = ROUTE_TYPE_CONNECTED;
                }else {
                    route_table[j].route_type = ROUTE_TYPE_STATIC;
                }
                route_table[j].route_type = ROUTE_TYPE_STATIC;
                if (iface_buf[0] != '\0') {
                    route_table[j].is_active =
                    get_interface_status(iface_buf) ? ACTIVE_ROUTE : INACTIVE_ROUTE;
                    } else {
                        route_table[j].is_active = ACTIVE_ROUTE;
                    }
                strncpy(route_table[j].interface, iface_buf, sizeof(route_table[j].interface) - 1);
                route_table[j].preference = preference;
                printf("[INFO] stored route[%d]: %s/%d via %s dev %s\n", j, network_text, (unsigned)route_table[j].prefix_len, next_hop_text, route_table[j].interface);
                placed = 1;
                added++;
                break;
            }
        }
        if (!placed) {
            printf("[ERROR] no free slot to store route %s/%d\n", network_text, prefix);
        }
        p = obj_end + 1;
    }

    free(content);
    printf("[INFO] read_n_store_static_routes: %d routes added from %s\n", added, filename);
    return 0;
}
