//
// Created by jrwhi on 3/15/2018.
//

#ifndef SPIKENET_SYNAPSE_H
#define SPIKENET_SYNAPSE_H

#include <stdbool.h>
#include <stdint.h>

#include "neuron.h"

#define AP_BUFSIZ 5

typedef struct Ap {
    struct Ap *next;
    int delay;
} Ap;

typedef struct ApBuf {
    Ap *front;
    Ap *rear;
    int next_delay;
} ApBuf;

typedef struct Synapse {
    bool is_firing;
    int32_t weighting;
    Neuron *axon_n;
    Neuron *dendrite_n;
    ApBuf *ap_buf;
    uint32_t firing_rate;
    uint32_t period_t;
    void (*logger)(struct Synapse *);
    void (*printer) (struct Synapse *);
} Synapse;

extern void proc_synapse(Synapse *s);
void deinit_synapse(Synapse *s);
void log_firing_rate(Synapse *s);

#endif //SPIKENET_SYNAPSE_H

