//
// Created by jrwhi on 3/16/2018.
//

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include "swim_world.h"

#define ABS(val)        (((val) > 0) ? (val) : -1 * (val))
#define ALLOW_POS(val)  ((val) > 0 ? (val) : 0)
#define VECTOR_EQ(vec1, vec2)   (((vec1).x == (vec2).x) && ((vec1).y == (vec2).y))

#define PYTHAG(valx, valy)  (sqrt(pow((valx), 2) + pow((valy), 2)))

enum {
    WORLD_SIZE_X    =   30,
    WORLD_SIZE_Y    =   30,
    LIGHT_PEAK      =   100,
    LIGHT_GRAD      =   5
};

const int32_t light[WORLD_SIZE_X][WORLD_SIZE_Y];
Vector current_force = {0.000, 0};
Vector current_vel = {0.01, 0};

void init_world() {
    // make light gradient
    int32_t temp_light[WORLD_SIZE_X][WORLD_SIZE_Y];
    int32_t center_x = WORLD_SIZE_X / 2;
    int32_t center_y = WORLD_SIZE_Y / 2;

    for (uint8_t x = 0; x < WORLD_SIZE_X; x++) {
        for (uint8_t y = 0; y < WORLD_SIZE_Y; y++) {
            temp_light[x][y] = ALLOW_POS(LIGHT_PEAK - ( ABS(PYTHAG((x - center_x), (y - center_y))) * LIGHT_GRAD));
        }
    }

    memcpy(light, temp_light, sizeof(temp_light));
}

ObjToken init_obj() {
    Kinematic  * new_kin = malloc(sizeof(Kinematic));

    // start the object in the center
//    new_kin->pos = (Vector) {WORLD_SIZE_X / 2, WORLD_SIZE_Y / 2};
    new_kin->pos = (Vector) {8.0, 15.0};
    new_kin->vel = (Vector) {0, 0};

    return (ObjToken) {new_kin};
}

void des_obj(ObjToken tok) {
    free(tok.kin);
}

void print_world(ObjToken tok) {
    for (uint8_t x = 0; x < WORLD_SIZE_X; x++) {
        for (uint8_t y = 0; y < WORLD_SIZE_Y; y++) {
            if ((tok.kin->pos).x == ((Vector) {x, y}).x && (tok.kin->pos).y == ((Vector) {x, y}).y){ // TODO: figure out why VECTOR_EQ isnt working here
               printf("+   ");
            } else {
                printf("%d ", light[x][y]);
                if (light[x][y] < 10) printf(" ");
                if (light[x][y] < 100) printf(" ");
            }
        }
        printf("\n");
    }
}

void print_obj_info(ObjToken tok) {
    printf("Light absorption: %d\n", get_light(tok));
    printf("Position X: %f Y: %f\n", tok.kin->pos.x, tok.kin->pos.y);
    printf("Velocity X: %f Y: %f\n", tok.kin->vel.x, tok.kin->vel.y);
}

inline static void resolve_force(Vector * force, Kinematic * obj) {
    obj->vel.x += force->x;
    obj->vel.y += force->y;
}

inline static void resolve_vel(Vector * vel, Kinematic * obj) {
    obj->pos.x += vel->x;
    obj->pos.y += vel->y;
}

void proc_obj(ObjToken tok, Vector force) {
    // resolve current force
    resolve_force(&current_force, tok.kin);

    // resolve current velocity
    resolve_vel(&current_vel, tok.kin);

    // resolve object force
    resolve_force(&force, tok.kin);

    // resolve object velocity
    resolve_vel(&tok.kin->vel, tok.kin);
}

int32_t get_light(ObjToken obj) {
    if (obj.kin->pos.x < WORLD_SIZE_X && obj.kin->pos.y < WORLD_SIZE_Y) {
        return light[(int) floor(obj.kin->pos.x)][(int) floor(obj.kin->pos.y)];
    } else {
        return 0;
    }
}

int32_t get_dark(ObjToken obj) {
    if (obj.kin->pos.x < WORLD_SIZE_X && obj.kin->pos.y < WORLD_SIZE_Y) {
        return LIGHT_PEAK - light[(int) floor(obj.kin->pos.x)][(int) floor(obj.kin->pos.y)];
    } else {
        return 100;
    }
}
