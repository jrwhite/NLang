//
// Created by jrwhi on 3/16/2018.
//

#ifndef SPIKENET_SWIM_WORLD_H
#define SPIKENET_SWIM_WORLD_H

#include <stdint.h>

typedef struct Vector {
    double x;
    double y;
} Vector;

typedef struct Kinematic {
    Vector pos;
    Vector vel;
} Kinematic;

typedef struct ObjToken {
    Kinematic * kin;
} ObjToken;

extern void init_world(void);
extern void proc_obj(ObjToken obj, Vector force);
extern ObjToken init_obj(void);
extern void des_obj(ObjToken tok);

extern int32_t get_light(ObjToken obj);
extern int32_t get_dark(ObjToken obj);

extern void print_world(ObjToken tok);
extern void print_obj_info(ObjToken tok);

#endif //SPIKENET_SWIM_WORLD_H
