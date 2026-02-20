// motor.h
#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

#ifndef UNIT_TESTING
#endif

typedef struct motor motor_t;

// Virtual function table (the "interface")
typedef struct motor_vtbl {
    void (*init)(motor_t *m);
    void (*set_speed)(motor_t *m, int speed); // semantic: -100..100 globally
    void (*stop)(motor_t *m);
} motor_vtbl_t;

// Base "class" - MUST be first field in derived structs
struct motor {
    const motor_vtbl_t *v;
};

// Polymorphic calls (work for omni and bi, etc.)
static inline void motor_init(motor_t *m)              { m->v->init(m); }
static inline void motor_set_speed(motor_t *m, int s)  { m->v->set_speed(m, s); }
static inline void motor_stop(motor_t *m)              { m->v->stop(m); }

#endif // MOTOR_H
