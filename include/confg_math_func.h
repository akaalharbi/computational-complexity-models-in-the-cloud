

#ifndef LONG_MESSAGE_CONFIG_MATH_H
#define LONG_MESSAGE_CONFIG_MATH_H

#define CEILING(X,Y) (((X) + (Y) - 1) / (Y))
#define MIN(X ,Y) (((X) > (Y)) ? (Y) : (X))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (X))


/* https://stackoverflow.com/a/39920811/20495595 */
#define NEEDS_BIT(N, B)     ((N >> B) > 0)
#define BITS_TO_REPRESENT(N)		       \
        (NEEDS_BIT(N,  0) + NEEDS_BIT(N,  1) + \
         NEEDS_BIT(N,  2) + NEEDS_BIT(N,  3) + \
         NEEDS_BIT(N,  4) + NEEDS_BIT(N,  5) + \
         NEEDS_BIT(N,  6) + NEEDS_BIT(N,  7) + \
         NEEDS_BIT(N,  8) + NEEDS_BIT(N,  9) + \
         NEEDS_BIT(N, 10) + NEEDS_BIT(N, 11) + \
         NEEDS_BIT(N, 12) + NEEDS_BIT(N, 13) + \
         NEEDS_BIT(N, 14) + NEEDS_BIT(N, 15) + \
         NEEDS_BIT(N, 16) + NEEDS_BIT(N, 17) + \
         NEEDS_BIT(N, 18) + NEEDS_BIT(N, 19) + \
         NEEDS_BIT(N, 20) + NEEDS_BIT(N, 21) + \
         NEEDS_BIT(N, 22) + NEEDS_BIT(N, 23) + \
         NEEDS_BIT(N, 24) + NEEDS_BIT(N, 25) + \
         NEEDS_BIT(N, 26) + NEEDS_BIT(N, 27) + \
         NEEDS_BIT(N, 28) + NEEDS_BIT(N, 29) + \
         NEEDS_BIT(N, 30) + NEEDS_BIT(N, 31)   \
        )


/* source: chatgpt */

#define LOG2_64BIT(n) ( \
    ((n) & 0xFFFFFFFF00000000ULL ? 32 : 0) + \
    ((n) & 0xFFFF0000FFFF0000ULL ? 16 : 0) + \
    ((n) & 0xFF00FF00FF00FF00ULL ? 8  : 0) + \
    ((n) & 0xF0F0F0F0F0F0F0F0ULL ? 4  : 0) + \
    ((n) & 0xCCCCCCCCCCCCCCCCULL ? 2  : 0) + \
    ((n) & 0xAAAAAAAAAAAAAAAAULL ? 1  : 0) \
)


#endif
