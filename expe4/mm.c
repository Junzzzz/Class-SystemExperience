#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define INVALID -1
#define total_instruction 320
#define total_vp 32
#define clear_period 50

typedef struct {
    int pn,
            pfn,
            counter,
            time;
} pl_type;
pl_type pl[total_vp];

struct pfc_struct {
    int pn,
            pfn;
    struct pfc_struct *next;
};
typedef struct pfc_struct pfc_type;
pfc_type pfc[total_vp], *freepf_head, *busypf_head, *busypf_tail;

int diseffect;
int a[total_instruction];
int page[total_instruction], offset[total_instruction];

void initialize(int);

void fifo(int);

void lru(int);

void opt(int);

void lfu(int);

void nur(int);

int main() {
    int s, i, j;

    srand(getpid() * 10);
    s = (float) 319 * rand() / 2147483647;

    for (i = 0; i < total_instruction; i += 4) {
        a[i] = s;
        a[i + 1] = a[i] + 1;
        a[i + 2] = (float) a[i] * rand() / 2147483647;
        a[i + 3] = a[i + 2] + 1;
        s = (float) rand() * (317 - a[i + 2]) / 2147483147 + a[i + 2] + 2;
    }

    for (i = 0; i < total_instruction; i++) {
        page[i] = a[i] / 10;
        offset[i] = a[i] % 10;
    }

    for (i = 4; i <= 32; i++) {
        printf("%2d page frames", i);
        fifo(i);               /* FIFO�㷨 */
        lru(i);                /* LRU�㷨  */
        opt(i);                /* OPT�㷨  */
        lfu(i);                /* LFU�㷨  */
        nur(i);                /* NUR�㷨  */
        printf("\n");
    }
}

void initialize(int total_pf) {
    int i;
    diseffect = 0;

    for (i = 0; i < total_vp; i++) {
        pl[i].pn = i;
        pl[i].pfn = INVALID;
        pl[i].counter = 0;
        pl[i].time = -1;
    }

    for (i = 1; i < total_pf; i++) {
        pfc[i - 1].next = &pfc[i];
        pfc[i - 1].pfn = i - 1;
    }
    pfc[total_pf - 1].next = NULL;
    pfc[total_pf - 1].pfn = total_pf - 1;
    freepf_head = &pfc[0];
}

void fifo(int total_pf) {
    int i, j;
    pfc_type *p;
    initialize(total_pf);
    busypf_head = busypf_tail = NULL;

    for (i = 0; i < total_instruction; i++)
        if (pl[page[i]].pfn == INVALID) {
            diseffect++;

            if (freepf_head == NULL) {
                p = busypf_head->next;
                pl[busypf_head->pn].pfn = INVALID;
                freepf_head = busypf_head;
                freepf_head->next = NULL;
                busypf_head = p;
            }

            p = freepf_head->next;
            freepf_head->next = NULL;
            freepf_head->pn = page[i];
            pl[page[i]].pfn = freepf_head->pfn;
            if (busypf_tail == NULL)
                busypf_head = busypf_tail = freepf_head;
            else {
                busypf_tail->next = freepf_head;
                busypf_tail = freepf_head;
            }
            freepf_head = p;
        }

    printf(" FIFO: %6.4f", 1 - (float) diseffect / 320);
}

void lru(int total_pf) {
    int min, minj, i, j, present_time;
    initialize(total_pf);
    present_time = 0;
    for (i = 0; i < total_instruction; i++) {
        if (pl[page[i]].pfn == INVALID) {
            diseffect++;
            if (freepf_head == NULL) {
                min = 32767;
                for (j = 0; j < total_vp; j++)
                    if (min > pl[j].time && pl[j].pfn != INVALID) {
                        min = pl[j].time;
                        minj = j;
                    }
                freepf_head = &pfc[pl[minj].pfn];
                pl[minj].pfn = INVALID;
                pl[minj].time = -1;
                freepf_head->next = NULL;
            }
            pl[page[i]].pfn = freepf_head->pfn;
            pl[page[i]].time = present_time;
            freepf_head = freepf_head->next;
        } else pl[page[i]].time = present_time;
        present_time++;
    }
    printf(" LRU: %6.4f", 1 - (float) diseffect / 320);
}

void opt(int total_pf) {
    int i, j, max, maxpage, d, dist[total_vp];
    initialize(total_pf);
    for (i = 0; i < total_instruction; i++) {
        if (pl[page[i]].pfn == INVALID) {
            diseffect++;
            if (freepf_head == NULL) {
                for (j = 0; j < total_vp; j++)
                    if (pl[j].pfn != INVALID) dist[j] = 32767;
                    else dist[j] = 0;
                d = 1;
                for (j = i + 1; j < total_instruction; j++) {
                    if (pl[page[j]].pfn != INVALID) dist[page[j]] = d;
                    d++;
                }
                max = -1;
                for (j = 0; j < total_vp; j++)
                    if (max < dist[j]) {
                        max = dist[j];
                        maxpage = j;
                    }
                freepf_head = &pfc[pl[maxpage].pfn];
                freepf_head->next = NULL;
                pl[maxpage].pfn = INVALID;
            }
            pl[page[i]].pfn = freepf_head->pfn;
            freepf_head = freepf_head->next;
        }
    }
    printf(" OPT: %6.4f", 1 - (float) diseffect / 320);
}

void lfu(int total_pf) {
    int i, j, min, minpage;
    initialize(total_pf);
    for (i = 0; i < total_instruction; i++) {
        if (pl[page[i]].pfn == INVALID) {
            diseffect++;
            if (freepf_head == NULL) {
                min = 32767;
                for (j = 0; j < total_vp; j++) {
                    if (min > pl[j].counter && pl[j].pfn != INVALID) {
                        min = pl[j].counter;
                        minpage = j;
                    }
                    pl[j].counter = 0;
                }
                freepf_head = &pfc[pl[minpage].pfn];
                pl[minpage].pfn = INVALID;
                freepf_head->next = NULL;
            }
            pl[page[i]].pfn = freepf_head->pfn;
            freepf_head = freepf_head->next;
        } else
            pl[page[i]].counter++;
    }
    printf(" LFU: %6.4f", 1 - (float) diseffect / 320);
}

void nur(int total_pf) {
    int i, j, dp, cont_flag, old_dp;
    initialize(total_pf);
    dp = 0;
    for (i = 0; i < total_instruction; i++) {
        if (pl[page[i]].pfn == INVALID) {
            diseffect++;
            if (freepf_head == NULL) {
                cont_flag = TRUE;
                old_dp = dp;
                while (cont_flag)
                    if (pl[dp].counter == 0 && pl[dp].pfn != INVALID)
                        cont_flag = FALSE;
                    else {
                        dp++;
                        if (dp == total_vp) dp = 0;
                        if (dp == old_dp)
                            for (j = 0; j < total_vp; j++) pl[j].counter = 0;
                    }
                freepf_head = &pfc[pl[dp].pfn];
                pl[dp].pfn = INVALID;
                freepf_head->next = NULL;
            }
            pl[page[i]].pfn = freepf_head->pfn;
            freepf_head = freepf_head->next;
        } else pl[page[i]].counter = 1;
        if (i % clear_period == 0)
            for (j = 0; j < total_vp; j++) pl[j].counter = 0;
    }
    printf(" NUR: %6.4f", 1 - (float) diseffect / 320);
}
