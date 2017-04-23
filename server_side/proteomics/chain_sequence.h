#ifndef CHAIN_SEQUENCE_H
#define CHAIN_SEQUENCE_H

#include "../mysql_connection_common.h"
#include "../configuration.h"

typedef struct {
	size_t chain_id;
	size_t sequence_id;

	size_t chain_offset_begin;
	size_t chain_offset_end;
	char author_offset_begin[256];
	char author_offset_end[256];
	size_t sequence_offset_begin;
	size_t sequence_offset_end;

	char* aligned_chain;
	char* aligned_sequence;
	double score;

	MYSQL_TIME creation;
} type_chain_sequence;

type_chain_sequence* type_chain_sequence_new();
type_chain_sequence* type_chain_sequence_build( size_t, size_t, const char*, size_t, size_t, const char*, size_t, size_t, char*, char* );
void type_chain_sequence_free( type_chain_sequence* );

type_chain_sequence* type_chain_sequence_select( configuration_struct*, size_t, size_t );
bool type_chain_sequence_insert( configuration_struct*, type_chain_sequence* );
void type_chain_sequence_printf( const type_chain_sequence* );

#endif
