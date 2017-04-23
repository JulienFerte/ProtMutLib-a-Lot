#ifndef MUTATION_H
#define MUTATION_H

#include "../mysql_connection_common.h"
#include "../configuration.h"

#include "../list.h"

#include "point_mutation.h"

typedef struct {
	size_t id;
	size_t sequence_id;
	char* comment;
	size_t mutated_sequence_id;
	list* point_mutations;
	MYSQL_TIME creation;
} type_mutation;

type_mutation* type_mutation_new( void );
void type_mutation_free( type_mutation* );

type_mutation* mutation_select_from_id( configuration_struct*, size_t );
void mutation_printf( type_mutation* );

#endif
