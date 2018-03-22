//
// Created by jrwhi on 3/15/2018.
//

#ifndef SPIKENET_SYNAPSE_H
#define SPIKENET_SYNAPSE_H

#include <stdbool.h>
#include <stdint.h>
#include "neuron.h"

typedef struct Synapse {
    bool is_firing;
    int32_t weighting;
    Neuron *axon_n;
    Neuron *dendrite_n;
} Synapse;

extern void proc_synapse(Synapse *s);

#endif //SPIKENET_SYNAPSE_H
