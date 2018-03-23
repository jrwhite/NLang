//
// Created by jrwhi on 3/15/2018.
//

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "synapse.h"

bool send_ap(ApBuf *ap) {
    Ap *front = ap->front;
    if (front == NULL) {
        return false;
    } else {
        ap->next_delay += 1;
    }
    assert(front);

    if (front->delay-- == 0) {
        ap->front = ap->front->next;
        free(front);
        return true;
    }

    return false;
}

Ap * new_ap(int delay) {
    Ap *new = (Ap *) xmalloc(sizeof(Ap));
    new->delay = delay;
    new->next = NULL;
    return new;
}

void add_ap(ApBuf *ap) {
    if (ap->rear) {
        ap->rear->next = new_ap(ap->next_delay);
        ap->rear = ap->rear->next;
        // debug below
        if (!ap->front) ap->front = ap->rear;
    } else {
        ap->front = ap->rear = new_ap(ap->next_delay);
    }
    assert(ap->rear);
    assert(ap->front);
    ap->next_delay = 0;
}

void proc_synapse(Synapse *s) {
    if (s->axon_n->fire_t == 1) {
        add_ap(s->ap_buf);
    }
    if (send_ap(s->ap_buf)) excite_neuron(s->dendrite_n, s->weighting);
    if (s->logger) s->logger(s);
}

void log_firing_rate(Synapse *s) {
    s->period_t += 1;
    if (s->axon_n->fire_t == 1) {
        printf("period %d\n", s->period_t);
        s->period_t = 0;
    }
}

void deinit_synapse(Synapse *s) {
    Ap *front = s->ap_buf->front;
    while (front) {
        s->ap_buf->front = s->ap_buf->front->next;
        free(front);
        front = s->ap_buf->front;
    }
}