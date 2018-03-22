//
// Created by jrwhi on 3/15/2018.
//

#include "synapse.h"

void proc_synapse(Synapse *s) {
    if (s->axon_n->fire_t == 1) {
        excite_neuron(s->dendrite_n, s->weighting);
    }
}