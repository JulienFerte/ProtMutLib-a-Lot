#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "../mysql_connection_common.h"
#include "../configuration.h"

#include "mutation.h"

typedef struct {
	size_t id;
	sized_string* residues;
	char* uniprot_id;
	size_t protein_id;
	size_t isoform_number;
	char* isoform_name;
	MYSQL_TIME creation;
} type_sequence;

type_sequence* type_sequence_new( void );
void type_sequence_free( type_sequence* );

type_sequence* sequence_select_from_uniprot_id_and_isoform_number( configuration_struct*, char*, unsigned int );
type_sequence* sequence_select_from_id( configuration_struct*, size_t );
size_t sequence_insert( configuration_struct*, type_sequence* );
void type_sequence_printf( type_sequence* );

void mutated_sequence( configuration_struct*, size_t, size_t, char** );

#endif
