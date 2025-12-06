#include "tools.h"

/*
 * method setup for signalization about error
 */
char merror[20];

void serror(char *message)
{
  strcpy(merror, message); 
   
}

/* support functions */
void strcpy(char* dest, const char* src)
{
  for(; *src; dest++, src++)
    *dest = *src;
  *dest = 0;
}
