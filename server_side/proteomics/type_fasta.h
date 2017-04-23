#ifndef TYPE_FASTA
#define TYPE_FASTA

#include "../list.h"

#include "protein.h"

typedef struct {
	type_protein* protein;
	list* list_of_isoforms;
} type_fasta;

type_fasta* type_fasta_new( void );
void type_fasta_printf( type_fasta* );
void type_fasta_free( type_fasta* );

#endif
