/*
    dns.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "main.h"
#include "net.h"
#include "dns.h"
#include "packet.h"

#define MAX_DOMAIN_NAME_LEN 50
#define MAX_TABLE_ENTRIES 256
#define DNS_IP 100

struct naming_entry {
    char domain_name[MAX_DOMAIN_NAME_LEN];
    int id;
};

struct naming_table {
    struct naming_entry entries[MAX_TABLE_ENTRIES];
    int num_entries;
};

struct naming_table naming_table;

//Initialize empty naming table
void dns_init(struct naming_table table) {
    table.num_entries = 0;
}

//Regiser host domain name and id to naming table
int dns_add_entry(char* domain, int id) {
    if (naming_table.num_entries > MAX_TABLE_ENTRIES) {
        printf("Error: maximum registered hosts");
        return -1;
    }

    if (strlen(domain) > MAX_DOMAIN_NAME_LEN) {
        printf("Error: domain name is too long (max 50 char)");
        return -1;
    }

    strcpy(naming_table.entries[naming_table.num_entries].domain_name, domain);
    naming_table.entries[naming_table.num_entries].id = id;
    naming_table.num_entries++;
    return 0;
}

void main() {
    dns_init(naming_table);


}

