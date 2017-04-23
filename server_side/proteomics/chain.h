#ifndef CHAIN_H
#define CHAIN_H

#include "../mysql_connection_common.h"
#include "../configuration.h"

typedef struct {
	size_t id;

	size_t structure_id;
	char* name;
	char* sequence;

	MYSQL_TIME creation;
} type_chain;

type_chain* type_chain_new();
type_chain* type_chain_build( size_t, const char*, const char* );
void type_chain_free( type_chain* );

type_chain* chain_select_from_structure_id_name( configuration_struct*, size_t, char* );
size_t chain_insert( configuration_struct*, type_chain* );
void type_chain_printf( const type_chain* );

#endif
