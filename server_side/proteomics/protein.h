#ifndef PROTEIN_H
#define PROTEIN_H

#include "../mysql_connection_common.h"
#include "../configuration.h"

typedef struct {
	size_t id;
	char* entry_name;
	char* name;
	bool is_wild_type;
	char* comment;
	MYSQL_TIME creation;
} type_protein;

type_protein* type_protein_new( void );
void type_protein_free( type_protein* );

type_protein* protein_select_from_name( configuration_struct*, char* );
type_protein* protein_select_from_id( configuration_struct*, size_t );
size_t protein_insert( configuration_struct*, type_protein* );
void type_protein_printf( type_protein* );

#endif
