/* LZSS encoder-decoder (Haruhiko Okumura; public domain) */
/* Modified by Clyne Sullivan to focus on streamed decompression. */

#ifndef EOF
#define EOF (-1)
#endif

#define EI 8  /* typically 10..13 */
#define EJ  3  /* typically 4..5 */
#define N (1 << EI)  /* buffer size */
#define F ((1 << EJ) + 1)  /* lookahead buffer size */

static unsigned char buffer[N * 2];
static const unsigned char *inbuffer;
static unsigned int insize, inidx;
static int buf, mask;

/* Prepares decode() to decompress the given data. */
void lzssinit(const unsigned char *inb, unsigned int ins)
{
    inbuffer = inb;
    insize = ins;
    inidx = 0;
    buf = 0;
    mask = 0;
}

int getbit(int n) /* get n bits */
{
    int i, x;

    x = 0;
    for (i = 0; i < n; i++) {
        if (mask == 0) {
            if (inidx >= insize)
                return EOF;
            buf = inbuffer[inidx++];
            mask = 128;
        }
        x <<= 1;
        if (buf & mask) x++;
        mask >>= 1;
    }
    return x;
}

/* handleoutput() receives each decompressed byte, return zero if want more. */
int decode(int (*handleoutput)(int))
{
    int i, j, k, r, c, ret;

    for (i = 0; i < N - F; i++) buffer[i] = ' ';
    r = N - F;
    while ((c = getbit(1)) != EOF) {
        if (c) {
            if ((c = getbit(8)) == EOF) break;
            if ((ret = handleoutput(c)))
                return ret;
            buffer[r++] =(unsigned char) c;  r &= (N - 1);
        } else {
            if ((i = getbit(EI)) == EOF) break;
            if ((j = getbit(EJ)) == EOF) break;
            for (k = 0; k <= j + 1; k++) {
                c = buffer[(i + k) & (N - 1)];
                if ((ret = handleoutput(c)))
                    return ret;
                buffer[r++] = (unsigned char)c;  r &= (N - 1);
            }
        }
    }

    return EOF;
}

