#include <stdio.h>
#include <ctype.h>
#include <syslog.h>

#include "hexdump.h"

void hexdump(void *mem, unsigned int len)
{
    unsigned int i, j;

    for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
    {
        /* print offset */
        if(i % HEXDUMP_COLS == 0)
        {
            //printf("0x%06X: ", i);
            syslog(LOG_INFO | LOG_LOCAL0, "0x%06X: ", i);
        }

        /* print hex data */
        if(i < len)
        {
            //printf("%02X ", 0xFF & ((char*)mem)[i]);
            syslog(LOG_INFO | LOG_LOCAL0, "%02X ", 0xFF & ((char*)mem)[i]);
        }
        else /* end of block, just aligning for ASCII dump */
        {
            //printf("   ");
            syslog(LOG_INFO | LOG_LOCAL0, "   ");
        }

        /* print ASCII dump */
        if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
        {
            for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
            {
                if(j >= len) /* end of block, not really printing */
                {
                    //putchar(' ');
                    syslog(LOG_INFO | LOG_LOCAL0, " ");
                }
                else if(isprint(((char*)mem)[j])) /* printable char */
                {
                    //putchar(0xFF & ((char*)mem)[j]);        
                    syslog(LOG_INFO | LOG_LOCAL0, "0x%02X", 0xFF & ((char*)mem)[j]);
                }
                else /* other char */
                {
                    //putchar('.');
                    syslog(LOG_INFO | LOG_LOCAL0, ".");
                }
            }
            //putchar('\n');
            syslog(LOG_INFO | LOG_LOCAL0, "\n");
        }
    }
}

