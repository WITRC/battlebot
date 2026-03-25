/**
 * @file utility.h
 * @brief Inline math helpers: clamp, range-map, and deadband.
 */
#ifndef MONSTER_BOOK_UTILITY_H
#define MONSTER_BOOK_UTILITY_H

#include <stdlib.h>
#include "config.h"

// math.h for powf (expo curve)
#ifndef UNIT_TESTING
#include <math.h>
#endif

/** @brief Clamp @p x to [@p min, @p max] (float). */
static float clamp_f(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

/** @brief Clamp @p x to [@p min, @p max] (int). */
static int clamp_int(int x, int min, int max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}


/**
 * Maps a float x from the range [in_min, in_max] to the range [out_min, out_max].
 * Clamps x to the input range before mapping.
 */
static float map_range(float x, float in_min, float in_max,
                       float out_min, float out_max)
{
    if (in_max == in_min)
        return out_min;

    x = clamp_f(x, in_min, in_max);
    return (x - in_min) * (out_max - out_min) /
           (in_max - in_min) + out_min;
}

/**
 * Maps an integer x from the range [in_min, in_max] to the range [out_min, out_max].
 * Clamps x to the input range before mapping.
 * Uses int64_t for intermediate multiplication to prevent overflow.
 */
static int map_range_int(int x, int in_min, int in_max,
                         int out_min, int out_max)
{
    if (in_max == in_min)   // prevent divide by zero
        return out_min;
    x = clamp_int(x, in_min, in_max);

    return (int)(out_min +
           (int64_t)(x - in_min) * (out_max - out_min) /
           (in_max - in_min));
}

/**
 * Applies a deadband to the input value. If the absolute value of the input is
 * less than the specified deadband threshold, returns 0. Otherwise, returns
 * the original value.
 */
static int deadband(int value) {
    if (abs(value) < MOTOR_DEADBAND) {
        return 0;
    }
    return value;
}



/**
 * Apply an expo curve to an input in [-100, 100].
 * output = (1 - expo) * x + expo * x^3  (normalized, then scaled back)
 * expo=0.0 → linear; expo=1.0 → full cubic.
 */
static int apply_expo(int value, float expo) {
    float x = value / 100.0f;           // normalize to [-1, 1]
    float y = (1.0f - expo) * x + expo * x * x * x;
    return (int)(y * 100.0f);           // scale back to [-100, 100]
}

#endif //MONSTER_BOOK_UTILITY_H