#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "../mysql_connection_common.h"
#include "../configuration.h"

typedef enum {
	STRUCTURE_FORMAT_MMCIF,
	STRUCTURE_FORMAT_PDB,
	STRUCTURE_FORMAT_MMTF
} type_structure_format;

typedef struct {
	size_t id;
	size_t protein_id;
	type_structure_format format;
	char* databank_id;
	char* file;
	MYSQL_TIME creation;
} type_structure;

type_structure* type_structure_new( void );
void type_structure_free( type_structure* );

type_structure* structure_select( configuration_struct*, char*, type_structure_format );
size_t structure_insert( configuration_struct*, type_structure* );

#endif
