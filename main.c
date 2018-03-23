#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "neuron.h"
#include "synapse.h"
#include "swim_world.h"

#define INTERNEURON     {0, 10000, false, 0}
#define ATTACH_LOGGER(syn)  syn.logger = log_firing_rate;

void test_synapse() {
    const uint8_t num_neurons = 7;
    const uint8_t num_synapses = 10;

    uint8_t i;
    uint32_t t;
    const uint32_t timesteps = 100;

    Neuron *n = (Neuron []){
            {0, 1000, false},
            {0, 1000, false},
            {0, 1000, false},
            {0, 1000, false},
            {0, 1000, false},
            {0, 1000, false},
            {0, 1000, false}
    };

    Neuron * n1 = &n[0];
    Neuron * n2 = &n[1];

    Synapse *s = (Synapse []){
            {false, 500, n1, n2},
            {false, 1500, &n[0], &n[1]},
            {false, 1900, &n[1], &n[2]},
            {false, 900, &n[1], &n[3]},
            {false, 1500, &n[2], &n[4]},
            {false, 500, &n[3], &n[2]},
            {false, 500, &n[3], &n[4]},
            {false, 500, &n[2], &n[5]},
            {false, 500, &n[2], &n[5]},
            {false, 500, &n[4], &n[5]}
    };

    for (t = 0; t < timesteps; t++) {
        excite_neuron(n1, 100);
        // proc neurons
        for (i = 0; i < num_neurons; i++) {
            proc_neuron(&n[i]);
        }
        // proc synapses
        for (i = 0; i < num_synapses; i++) {
            proc_synapse(&s[i]);
        }
        printf("n2 potential: %d\n", n2->potential);
    }

    free(n);
    free(s);
}

void neuron_thread() {

}

void test_bulk_synapses() {

}

void test_swim_world_init() {
    init_world();
    ObjToken obj = init_obj();
    printf("Light %d\n", get_light(obj));
    print_world(obj);
    des_obj(obj);
}

void test_swim_world_physics() {
    init_world();
    ObjToken obj = init_obj();
    const int num_timesteps = 10;
    int t = 0;

    for (t = 0; t < num_timesteps; t++) {
        print_obj_info(obj);
        proc_obj(obj, (Vector) {0, 0});
    }

    des_obj(obj);
}

void test_swim_world() {
    // initialize world
    init_world();
    ObjToken obj = init_obj();

    const int num_neurons = 9;
    Neuron n[] = {
            [0 ... 9-1] = INTERNEURON
    };

    Neuron * light_sensory_n = &n[0];
    Neuron * dark_sensory_n = &n[1];

    Neuron * x_pos_motor_n = &n[2];
    Neuron * x_neg_motor_n = &n[3];
    Neuron * y_motor_n = &n[4];

    Neuron * inter1_n = &n[5];
    Neuron * inter2_n = &n[6];
    Neuron * inter3_n = &n[7];
    Neuron * inter4_n = &n[8];

    const int32_t ictrl_w = 500;

    const int num_synapses = 4;
    Synapse s[] = {
            // sensory processing
            {false, 10000, dark_sensory_n, inter1_n},
            {false, 10000, light_sensory_n, inter2_n},
            // proportional control
            {false, 100, inter1_n, x_pos_motor_n},
            {false, 100, inter2_n, x_neg_motor_n},
            // integral control
    };

//    ATTACH_LOGGER(s[3]);
    ATTACH_LOGGER(s[1]);

    const int num_timesteps = 100;
    int t = 0;
    int i;

    double new_force = 0;

    for (t = 0; t < num_timesteps; t++) {
//        print_obj_info(obj);

        // proc sensory neurons
        excite_neuron(light_sensory_n, 100 * get_light(obj));
        excite_neuron(dark_sensory_n, 100 * get_dark(obj));

        // proc neurons
        for (i = 0; i < num_neurons; i++) {
            proc_neuron(&n[i]);
        }
        // proc synapses
        for (i = 0; i < num_synapses; i++) {
            proc_synapse(&s[i]);
        }
        // proc obj
//        printf("Dark potential %d\n", dark_sensory_n->potential);
//        printf("Light potential %d\n", light_sensory_n->potential);
//        printf("Motor potential %d\n", x_pos_motor_n->potential);
        // temporarily use the new_force stager
        new_force = obj.kin->pos.x <= 15 ? 0.1 : -0.1;
        new_force *= (double) x_pos_motor_n->potential;
        new_force /= 10000.0;
        proc_obj(obj, (Vector) { new_force, 0});
//        printf("\n");
        printf("%d ", x_pos_motor_n->potential);
//        printf("%f ", obj.kin->vel.x);
//        if (t % 10 == 0) printf("%d, ", get_dark(obj));
//        if (t % 100 == 0) printf("%d, ", (int) obj.kin->pos.x);
//        if (t % 10 == 0) printf("%f, ", obj.kin->vel.x);
//        if (t % 10 == 0) printf("%d, ", dark_sensory_n->potential);
//        if (t % 100 == 0) printf("%d, ", get_light(obj));
    }

    des_obj(obj);
}

void test_dk_path(){
    const int num_neurons = 4;
    Neuron n[] = {
            [0 ... 3] = INTERNEURON
    };

    Neuron * prop_n = &n[0];
    Neuron * beta_prop_n = &n[1];
    Neuron * beta2_prop_n = &n[2];
    Neuron * delta_n = &n[3];

    const int num_synapses = 5;
    Synapse s[] = {
            {.weighting = 6000, .axon_n = prop_n, .dendrite_n = beta_prop_n, .ap_buf = (ApBuf []) {NULL, NULL, 100}},
            {false, -40, beta_prop_n, prop_n, .ap_buf =(ApBuf []) {NULL, NULL, 100}},
            {false, 100, beta_prop_n, beta2_prop_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, -60, beta2_prop_n, prop_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 100, prop_n, delta_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}}
    };

    ATTACH_LOGGER(s[0]);

    const int num_timesteps = 1000;
    uint32_t t, i;
    for (t = 0; t < num_timesteps; t++) {
        if (t % 10 == 0) excite_neuron(prop_n, 6000);
        for (i = 0; i < num_neurons; i++) {
            proc_neuron(&n[i]);
        }
        for (i = 0; i < num_synapses; i++) {
            proc_synapse(&s[i]);
        }
//        printf("%d\n", );
    }
}

void test_ap_prop() {
    Neuron n[] = {
            [0 ... 1] = INTERNEURON
    };

    Neuron * n1 = &n[0];
    Neuron * n2 = &n[1];

    Synapse s[] = {
            {.weighting = 1000, .axon_n = n1, .dendrite_n = n2, .ap_buf = (ApBuf []) {NULL, NULL, 20}}
    };

    ATTACH_LOGGER(s[0]);

    const int num_timesteps = 100;
    uint32_t  t, i;
    for (t = 0; t < num_timesteps; t++) {
//        printf("next delay %d\n", s[0].ap_buf->next_delay);
//        printf("%d\n", n2->potential);
        if (t % 5 == 0) excite_neuron(n1, 15000);
        proc_neuron(n1);
        proc_neuron(n2);
        proc_synapse(&s[0]);
    }
}

int main() {
//    test_synapse();
//    test_swim_world_init();
//    test_swim_world_physics();
//    test_swim_world();
    test_dk_path();
//    test_ap_prop();
    return 0;
}
