#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "neuron.h"
#include "synapse.h"
#include "swim_world.h"

#define INTERNEURON     {0, 10000, false, 0}
#define SYNAPSE(ax, dend) {false, .weighting = 12000, .axon_n = (ax), .dendrite_n = (dend), .ap_buf = (ApBuf []) {NULL, NULL, 21}}
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

    const int num_neurons = 15;
    Neuron n[] = {
            [0 ... 14] = INTERNEURON
    };

    Neuron * diff_n = &n[1];
    Neuron * acc_n = &n[3];

    Neuron * move_n = &n[2];
    Neuron * mvl_exp_n = &n[4];
    Neuron * mvr_exp_n = &n[5];
    Neuron * mvl_th_n = &n[6];
    Neuron * mvr_th_n = &n[7];

    Neuron * light_sensory_n = &n[0];
    Neuron * dark_sensory_n = &n[10];

    Neuron * x_pos_motor_n = &n[9];
    Neuron * x_neg_motor_n = &n[8];


    const int32_t ictrl_w = 500;

    const int num_synapses = 18;
    Synapse s[] = {
            // sensory processing
            {false, 15000, dark_sensory_n, move_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 10000, diff_n, move_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            // motor control
            {false, 8000, mvl_th_n, &n[13], .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 8000, mvr_th_n, &n[14], .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 9000, move_n, &n[13], .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 9000, move_n, &n[14], .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 100, &n[13], x_neg_motor_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 100, &n[14], x_pos_motor_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 10000, &n[13], mvl_exp_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 10000, &n[14], mvr_exp_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            // move direction ideation
            {false, 10000, mvl_exp_n, &n[11], .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 10000, mvr_exp_n, &n[12], .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 10000, acc_n, &n[12], .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 10000, acc_n, &n[11], .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 10000, &n[11], mvr_th_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, -10000, &n[12], mvr_th_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 10000, &n[11], mvl_th_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, -10000, &n[12], mvl_th_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}}
    };

//    ATTACH_LOGGER(s[3]);
    ATTACH_LOGGER(s[8]);

    const int num_timesteps = 10000;
    int t = 0;
    int i;

    int dark_diff;
    int last_dark;
    int last_diff;

    double new_force = 0;
    excite_neuron(mvr_th_n, 15000);

    for (t = 0; t < num_timesteps; t++) {
//        print_obj_info(obj);

        // proc sensory neurons
        excite_neuron(light_sensory_n, 100 * get_light(obj));
        excite_neuron(dark_sensory_n, 27 * get_dark(obj));

        // do first and second diffs
        dark_diff = (27 * get_dark(obj)) - last_dark;
        last_dark = 27 * get_dark(obj);
        excite_neuron(diff_n, 500 * dark_diff);
        excite_neuron(acc_n, 500 * (dark_diff - last_diff));
        last_diff = dark_diff;

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
//        new_force = obj.kin->pos.x <= 15 ? 0.1 : -0.1;
        new_force *= (double) x_pos_motor_n->potential - (double) x_neg_motor_n->potential;
        new_force /= 100.0;
//        printf("%f, ", new_force);
        proc_obj(obj, (Vector) { new_force, 0});
//        printf("\n");
        printf("%d ", x_pos_motor_n->potential);
//        printf("%f ", obj.kin->vel.x);
//        if (t % 10 == 0) printf("%d, ", get_dark(obj));
//        if (t % 100 == 0) printf("%f, ", obj.kin->pos.x);
//        if (t % 10 == 0) printf("%f, ", obj.kin->vel.x);
//        if (t % 10 == 0) printf("%d, ", dark_sensory_n->potential);
//        if (t % 100 == 0) printf("%d, ", get_light(obj));
    }

//    des_obj(obj);
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
            {false, .weighting = 40000, .axon_n = prop_n, .dendrite_n = beta_prop_n, .ap_buf = (ApBuf []) {NULL, NULL, 1000}},
            {false, -60000, beta_prop_n, prop_n, .ap_buf =(ApBuf []) {NULL, NULL, 10}},
            {false, 80000, beta_prop_n, beta2_prop_n, .ap_buf = (ApBuf []) {NULL, NULL, 1001}},
            {false, -60000, beta2_prop_n, prop_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}},
            {false, 100, prop_n, delta_n, .ap_buf = (ApBuf []) {NULL, NULL, 20}}
    };

    ATTACH_LOGGER(s[0]);

    const int num_timesteps = 100000;
    uint32_t t, i;
    for (t = 0; t < num_timesteps; t++) {
        if (t % 5 == 0) excite_neuron(prop_n, 10000);
//        if (t % 10 == 0) printf("%d\n", prop_n->potential);
        for (i = 0; i < num_neurons; i++) {
            proc_neuron(&n[i]);
        }
        for (i = 0; i < num_synapses; i++) {
            proc_synapse(&s[i]);
        }
//        printf("%d\n", );
    }
}

void test_dk_loop() {
    const int num_neurons = 10;
    Neuron n[] = {
            [0 ... 9] = INTERNEURON
    };

    const int num_synapses = 14;
    Synapse s[] = {
            // neurons 3 - 9 are loop neurons
            SYNAPSE(&n[3], &n[4]),
            SYNAPSE(&n[4], &n[5]),
            SYNAPSE(&n[5], &n[6]),
            SYNAPSE(&n[6], &n[7]),
            SYNAPSE(&n[7], &n[8]),
            SYNAPSE(&n[8], &n[3]),
            // loop-check synapses
            {false, 30, &n[8], &n[2], .ap_buf =  (ApBuf []) {NULL, NULL, 100}},
            {false, -30, &n[2], &n[3], .ap_buf = (ApBuf []) {NULL, NULL, 100}},
            {false, -30, &n[1], &n[2], .ap_buf = (ApBuf []) {NULL, NULL, 100}},
            // loop entry
            {false, 15000, &n[1], &n[3], .ap_buf =  (ApBuf []) {NULL, NULL, 30}},
            {false, 15000, &n[1], &n[3], .ap_buf =  (ApBuf []) {NULL, NULL, 90}},
            // loop exit
            {false, -5000, &n[5], &n[0], .ap_buf =  (ApBuf []) {NULL, NULL, 120}},
            // output
            {false, 30, &n[0], &n[9], .ap_buf =  (ApBuf []) {NULL, NULL, 120}},
            {false, 8000, &n[0], &n[1], .ap_buf =  (ApBuf []) {NULL, NULL, 620}},
    };

   ATTACH_LOGGER(s[12]);

    const int num_timesteps = 100000;
    uint32_t t, i;
    uint32_t k = 5;
    uint32_t l = 1;
    for (t = 0; t < num_timesteps; t++) {
        if (t % k == 0) excite_neuron(&n[0], 6000);
        if (t % 1000 == 0) k;
        if (t % 5000 == 0) l;
        if (t > 50000) k;
//        if (t % 10 == 0) printf("%d\n", prop_n->potential);
        for (i = 0; i < num_neurons; i++) {
            proc_neuron(&n[i]);
        }
        for (i = 0; i < num_synapses; i++) {
            proc_synapse(&s[i]);
        }
//        printf("%d\n", );
    }
}

void test_loop() {
    const int num_neurons = 10;

    Neuron n[] = {
            [0 ... 9] = INTERNEURON
    };

    const int num_synapses = 11;

    Synapse s[] = {
           SYNAPSE(&n[0], &n[1]),
           SYNAPSE(&n[1], &n[2]),
           SYNAPSE(&n[2], &n[3]),
           SYNAPSE(&n[3], &n[4]),
           SYNAPSE(&n[4], &n[5]),
           SYNAPSE(&n[5], &n[6]),
           SYNAPSE(&n[6], &n[7]),
           SYNAPSE(&n[7], &n[8]),
           SYNAPSE(&n[8], &n[9]),
           SYNAPSE(&n[9], &n[0]),

           {false, 16000, &n[0], &n[1], .ap_buf = (ApBuf []) {NULL, NULL, 400}}
    };

    ATTACH_LOGGER(s[10]);

    excite_neuron(&n[0], 12000);
    const int num_timesteps = 10000;
    uint32_t t, i;
    for (t = 0; t < num_timesteps; t++) {
//        if (t % 10 == 0) printf("%d\n", prop_n->potential);
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

//    ATTACH_LOGGER(s[0]);

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
    test_swim_world();
//    test_dk_path();
//    test_loop();
//    test_dk_loop();
//    test_ap_prop();
    return 0;
}
