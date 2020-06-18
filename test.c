//
// Created by Jun on 6/7/20.
//

#include "netdb.h"
#include <stdio.h>

int main() {
    struct servent *se = NULL;

    se = getservbyport(htons(80), "TCP");
//    se = getservbyname("domain", "tcp");
    if (!se) {
        printf("NULL\n");
        return -1;
    }


    printf("name : %s\n", se->s_name);
    printf("port : %d\n", ntohs(se->s_port));
    printf("proto : %s\n", se->s_proto);
    for (int i = 0; se->s_aliases[i]; i++)
        printf("aliases : %s\n", se->s_aliases[i]);

    return 0;

}