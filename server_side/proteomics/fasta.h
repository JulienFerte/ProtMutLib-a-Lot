#ifndef FASTA_H
#define FASTA_H

#include "type_fasta.h"

#include "../configuration.h"

type_fasta* type_fasta_parse( configuration_struct*, const char* );

#endif
