#ifndef ADPCM_H_INCLUDED
#define ADPCM_H_INCLUDED

struct adpcm_state
{
    int valprev;
    int index;
};

void adpcm_coder(short indata[], char outdata[], int len, struct adpcm_state *state);
void adpcm_decoder(char indata[], short outdata[], int len, struct adpcm_state *state);
#endif // ADPCM_H_INCLUDED
