cmake_minimum_required(VERSION 3.9)
project(spikenet C)

set(CMAKE_C_STANDARD 99)

add_executable(spikenet main.c neuron.c neuron.h synapse.c synapse.h swim_world.c swim_world.h sensory_neuron.c sensory_neuron.h motor_neuron.c motor_neuron.h)