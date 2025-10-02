

static unsigned int seed = 1;  // startvärde

void srand(unsigned int s) {
    seed = s;
}

int rand(void) {
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;       // returnera 15 slumpbitar
}
