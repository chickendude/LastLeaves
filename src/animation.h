#ifndef LASTLEAVES_ANIMATION_H
#define LASTLEAVES_ANIMATION_H

typedef struct Animation
{
    const unsigned int *sprite_data;
    const int num_frames;
    /** How many cycles to show each frame in the animation. */
    const char frame_cycles[10];
} Animation;

#endif //LASTLEAVES_ANIMATION_H
