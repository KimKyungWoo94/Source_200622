#ifndef _HEXDUMP_H_
#define _HEXDUMP_H_


#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 16
#endif

/* hexdump.c */
void hexdump(void *mem, unsigned int len);

#endif
