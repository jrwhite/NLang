//
// Created by jrwhi on 3/14/2018.
//

#ifndef SPIKENET_NEURON_H
#define SPIKENET_NEURON_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Neuron Neuron;

typedef struct Neuron {
    int32_t potential;
    int32_t threshold;
    bool is_firing;
    uint8_t fire_t;
} Neuron;

extern void excite_neuron(Neuron *n, int32_t ex);
extern void proc_neuron(Neuron *n);
#endif //SPIKENET_NEURON_H
