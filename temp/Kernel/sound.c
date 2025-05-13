// codigo de osdev
#include <sound.h>
#include <lib.h>
#include <time.h>
#include <keyboard.h>

//Play sound using built in speaker
void play_sound(uint32_t nFrequence) {
    uint32_t Div;
    uint8_t tmp;

    //Set the PIT to the desired frequency
    Div = 1193180 / nFrequence;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t) (Div) );
    outb(0x42, (uint8_t) (Div >> 8));

    //And play the sound using the PC speaker
    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

//make it shutup
void nosound() {
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}

void playNote(int freq, int duration, int waitAfter) {
    play_sound(freq);
    timer_wait(duration);
    nosound();
    timer_wait(waitAfter);
}

//Make a beep
void beep(/* int freq, int duration */) {
    // Score sound ?
    play_sound(490);
    timer_wait(8);
    nosound();
}

void playSimpsons() {
    playNote(DO, 2, 4);
    playNote(MI, 2, 4);
    playNote(FASharp, 2, 4);
    playNote(LA, 2, 3);

    if (getChar() != 0) {
        getChar(); // Consume release
        return;
    }

    playNote(SOL, 3, 4);
    playNote(MI, 2, 2);
    playNote(DO, 2, 2);
    playNote(LA, 2, 3);

    if (getChar() != 0) {
        getChar();
        return;
    }

    playNote(FASharp, 1, 2);
    playNote(FASharp, 1, 2);
    playNote(FASharp, 1, 2);
    playNote(SOL, 2, 5);

    if (getChar() != 0) {
        getChar();
        return;
    }

    playNote(FASharp, 1, 2);
    playNote(FASharp, 1, 2);
    playNote(FASharp, 1, 2);
    playNote(SOL, 1, 3);

    if (getChar() != 0) {
        getChar();
        return;
    }

    playNote(LA, 6, 6);
    playNote(SI2, 2, 0);
}
