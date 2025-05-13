#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

#define DO 261
#define RE 293
#define MI 329
#define FA 349
#define FASharp 370
#define SOL 392
#define LA 440
#define SI 493

#define DO2 523
#define RE2 587
#define MI2 659
#define FA2 698
#define SOL2 784
#define LA2 880
#define SI2 987
#define DO3 1046

void play_sound(uint32_t nFrequence);
void nosound();

void beep(/* int freq, int duration */);

void playNote(int freq, int duration, int waitAfter);

void playSimpsons();

#endif /* SOUND_H */