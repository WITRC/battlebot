/**
 * @file motor.h
 * @brief Polymorphic motor base interface.
 *
 * Defines the vtable-based "base class" used by all motor types (bidirectional,
 * omni, etc.). Derived structs must place `motor_t` as their first field.
 */
#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

typedef struct motor motor_t;

/** Virtual function table shared by all motor implementations. */
typedef struct motor_vtbl {
    void (*init)(motor_t *m);                /**< Initialize hardware (PWM, GPIO). */
    void (*set_speed)(motor_t *m, int speed); /**< Set speed in [-100, 100]. */
    void (*stop)(motor_t *m);                /**< Coast to stop. */
} motor_vtbl_t;

/** Base motor object — must be the first field of every derived struct. */
struct motor {
    const motor_vtbl_t *v;
};

/** @name Polymorphic motor calls */
/** @{ */
static inline void motor_init(motor_t *m)              { m->v->init(m); }
static inline void motor_set_speed(motor_t *m, int s)  { m->v->set_speed(m, s); }
static inline void motor_stop(motor_t *m)              { m->v->stop(m); }
/** @} */

#endif // MOTOR_H
