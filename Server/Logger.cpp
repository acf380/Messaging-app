#include <stdio.h>

#define LOGGING_ON 1

void LogMessage(const char* content, char kind)
{
    if (LOGGING_ON != 1) return;
    if (kind == 'i') printf("[  i  ]    %s\n", content);
    else if (kind == 'd') printf("[  +  ]    %s\n", content);
    else if (kind == 'e') printf("[  !  ]    %s\n", content);
}