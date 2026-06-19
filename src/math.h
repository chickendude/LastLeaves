#ifndef LASTLEAVES_MATH_H
#define LASTLEAVES_MATH_H

#define FIXED_SHIFT 8

/** Converts an integer to a fixed point value, determined by FIXED_SHIFT. */
inline int fxpt(const int value) { return value << FIXED_SHIFT; }
/** Converts a fixed point value back to an integer, determined by FIXED_SHIFT. */
inline int fxpt_to_int(const int value) { return value >> FIXED_SHIFT; }

/**
 * Returns a pseudo-random number between 0 and max (non-inclusive).
 *
 * @param max The max value + 1. E.g. if this is 10, it will return values 0-9.
 * @return
 */
int random(int max);

/**
 * Counts the number of digits in a number and stores the digits in an array.
 * E.g. 5 returns 1 and [1], 5154 returns 4 and [5,1,5,4].
 *
 * @param number The number whose digits should be counted. Maximum of 6 digits.
 * @param digits Array to store the digits.
 * @return The number of digits in the number.
 */
int count_digits(u32 number, int *digits);
#endif //LASTLEAVES_MATH_H
