#ifndef CLOCK_H
#define CLOCK_H

unsigned int seconds();
unsigned int minutes();
unsigned int hours();
unsigned int year();
unsigned int day();
unsigned int month();
void timeToStr(char* dest);
void dateToStr(char* dest);

#endif