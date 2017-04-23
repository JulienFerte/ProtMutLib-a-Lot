#ifndef POINT_MUTATION_H
#define POINT_MUTATION_H

#include "../mysql_connection_common.h"
#include "../configuration.h"

typedef struct {
	size_t id;
	size_t mutation_id;
	size_t offset;
	char residue;
	MYSQL_TIME creation;
} type_point_mutation;

type_point_mutation* point_mutation_new( void );
void type_point_mutation_free( type_point_mutation* );

void point_mutation_printf( type_point_mutation* );

#endif
