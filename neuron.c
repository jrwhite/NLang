//
// Created by jrwhi on 3/14/2018.
//

#include <assert.h>
#include "neuron.h"

enum NEURON_PARAMS {
    HYPERPOLARIZATION   =   -2000,
    FIRE_TIME           =   3
};

static inline void decay_neuron(Neuron *n) {
    n->potential *= 63;
    n->potential /= 64;
}

static inline void fire_neuron(Neuron *n) {
    n->potential = HYPERPOLARIZATION;
    n->is_firing = true;
}

static inline void fire_reset(Neuron *n) {
    n->fire_t = 0;
    n->is_firing = false;
}

void excite_neuron(Neuron *n, int32_t ex) {
    n->potential += ex;
}

void proc_neuron(Neuron *n) {
    // check if fired
    if (n->is_firing) {
        if (++(n->fire_t) >= FIRE_TIME) fire_reset(n);
    } else if (n->potential >= n->threshold) {
        fire_neuron(n);
    }
    decay_neuron(n);
}
