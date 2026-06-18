#ifndef LASTLEAVES_MATH_H
#define LASTLEAVES_MATH_H

#define FIXED_SHIFT 8

/** Converts an integer to a fixed point value, determined by FIXED_SHIFT. */
inline int fxpt(const int value) { return value << FIXED_SHIFT; }
/** Converts a fixed point value back to an integer, determined by FIXED_SHIFT. */
inline int fxpt_to_int(const int value) { return value >> FIXED_SHIFT; }

#endif //LASTLEAVES_MATH_H
