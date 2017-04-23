#ifndef NEEDLEMAN_WUNSCH_H
#define NEEDLEMAN_WUNSCH_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void needleman_wunsch( const char*, size_t, char**, const char*, size_t, char**, double* );

#endif
