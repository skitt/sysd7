volatile struct mc68230fm * pit = (void *)0x05000000;

void initAudio(void);
void setsample(unsigned char sample);
unsigned char getsample(void);

