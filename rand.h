// random.h
#ifndef RANDOM_H
#define RANDOM_H

// Sätt startvärde (seed) för slumpgeneratorn
void srandom(unsigned int s);

// Generera ett nytt slumptal (0–32767)
int random(void);

#endif // RANDOM_H
