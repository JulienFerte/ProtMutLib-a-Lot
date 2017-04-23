#include "chain_sequence.h"

#include "../log_record.h"

type_chain_sequence* type_chain_sequence_new() {
	type_chain_sequence* result = malloc( sizeof( type_chain_sequence ) );

	result->chain_id = 0;
	result->sequence_id = 0;

	result->chain_offset_begin = 0;
	result->chain_offset_end = 0;
	memset( result->author_offset_begin, '\0', 256 );
	memset( result->author_offset_end, '\0', 256 );
	result->sequence_offset_begin = 0;
	result->sequence_offset_end = 0;

	result->aligned_chain = NULL;
	result->aligned_sequence = NULL;
	result->score = 0.;

	return result;
}

type_chain_sequence* type_chain_sequence_build( size_t chain_id, size_t sequence_id, const char* chain, size_t chain_offset_begin, size_t chain_offset_end, const char* sequence, size_t sequence_offset_begin, size_t sequence_offset_end, char* author_offset_begin, char* author_offset_end ) {
	type_chain_sequence* result = type_chain_sequence_new();

	result->chain_id = chain_id;
	result->sequence_id = sequence_id;

	result->chain_offset_begin = chain_offset_begin;
	result->chain_offset_end = chain_offset_end;

	size_t author_offset_begin_length = strlen( author_offset_begin );
	author_offset_begin_length = ( author_offset_begin_length > 255 ? 255 : author_offset_begin_length );
	strncpy( result->author_offset_begin, author_offset_begin, author_offset_begin_length );
	result->author_offset_begin[ author_offset_begin_length + 1 ] = '\0';

	size_t author_offset_end_length = strlen( author_offset_end );
	author_offset_end_length = ( author_offset_end_length > 255 ? 255 : author_offset_end_length );
	strncpy( result->author_offset_end, author_offset_end, author_offset_end_length );
	result->author_offset_end[ author_offset_end_length + 1 ] = '\0';

	result->sequence_offset_begin = sequence_offset_begin;
	result->sequence_offset_end = sequence_offset_end;

//*** The actual_chain replaces the multiple choices, like (CSP), into '-' before aligning.
//*** The chain offsets are computed on the actual_chain.
	size_t actual_chain_length = strlen( chain );
	char* actual_chain = (char*) malloc( sizeof( char ) * ( actual_chain_length + 1 ) );
	memset( actual_chain, '*', actual_chain_length );

	const char* current = chain;
	char* actual_current = actual_chain;
	bool copy = true;
	while( *current != '\0' ) {
		if( *current == '(' ) {
			copy = false;
			*actual_current = '-';
			++actual_current;
		}
		else if ( *current == ')' ) {
			copy = true;
		}
		else if ( copy ) {
			*actual_current = *current;
			++actual_current;
		}
		++current;
	}
	actual_chain[ actual_chain_length ] = '\0';

	needleman_wunsch( actual_chain + ( chain_offset_begin - 1 ), chain_offset_end - chain_offset_begin + 1, &(result->aligned_chain),
						sequence + ( sequence_offset_begin - 1 ), sequence_offset_end - sequence_offset_begin + 1, &(result->aligned_sequence),
						&(result->score) );

	free( actual_chain );

	return result;
}

void type_chain_sequence_free( type_chain_sequence* chain_sequence ) {
	if( chain_sequence != NULL ) {
		if( chain_sequence->aligned_chain ) {
			free( chain_sequence->aligned_chain );
		}
		if( chain_sequence->aligned_sequence ) {
			free( chain_sequence->aligned_sequence );
		}
		free( chain_sequence );
	}
}

type_chain_sequence* type_chain_sequence_select( configuration_struct* configuration, size_t chain_id, size_t sequence_id ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	type_chain_sequence* result = NULL;

	execute_query( configuration, &database_connection,
									"select "
										"CHAIN_SEQUENCE_CHAIN_ID,"
										"CHAIN_SEQUENCE_SEQUENCE_ID,"
										"CHAIN_SEQUENCE_CHAIN_OFFSET_BEGIN,"
										"CHAIN_SEQUENCE_CHAIN_OFFSET_END,"
										"CHAIN_SEQUENCE_AUTHOR_OFFSET_BEGIN,"
										"CHAIN_SEQUENCE_AUTHOR_OFFSET_END,"
										"CHAIN_SEQUENCE_SEQUENCE_OFFSET_BEGIN,"
										"CHAIN_SEQUENCE_SEQUENCE_OFFSET_END,"
										"CHAIN_SEQUENCE_ALIGNED_CHAIN,"
										"CHAIN_SEQUENCE_ALIGNED_SEQUENCE,"
										"CHAIN_SEQUENCE_SCORE,"
										"CHAIN_SEQUENCE_CREATION"
									" from TB_CHAIN_SEQUENCE where CHAIN_SEQUENCE_CHAIN_ID=%z and CHAIN_SEQUENCE_SEQUENCE_ID=%z limit 1", chain_id, sequence_id );
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	if( mysql_num_rows( stored_result ) > 0 ) {
		MYSQL_ROW first_row = mysql_fetch_row( stored_result );

		result = type_chain_sequence_new();

		parse_row( configuration, &first_row, mysql_fetch_lengths( stored_result ), "%z %z %z %z %s %s %z %z %s %s %lf %D",
						&(result->chain_id), &(result->sequence_id),
						&(result->chain_offset_begin), &(result->chain_offset_end),
						&(result->author_offset_begin), &(result->author_offset_end),
						&(result->sequence_offset_begin), &(result->sequence_offset_end),
						&(result->aligned_chain), &(result->aligned_sequence),
						&(result->score), &(result->creation) );
	}

	mysql_free_result( stored_result );
	disconnect_database( &database_connection );

	return result;
}
bool type_chain_sequence_insert( configuration_struct* configuration, type_chain_sequence* chain_sequence ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	execute_query( configuration, &database_connection,
									"insert into TB_CHAIN_SEQUENCE("
											"CHAIN_SEQUENCE_CHAIN_ID,"
											"CHAIN_SEQUENCE_SEQUENCE_ID,"
											"CHAIN_SEQUENCE_CHAIN_OFFSET_BEGIN,"
											"CHAIN_SEQUENCE_CHAIN_OFFSET_END,"
											"CHAIN_SEQUENCE_AUTHOR_OFFSET_BEGIN,"
											"CHAIN_SEQUENCE_AUTHOR_OFFSET_END,"
											"CHAIN_SEQUENCE_SEQUENCE_OFFSET_BEGIN,"
											"CHAIN_SEQUENCE_SEQUENCE_OFFSET_END,"
											"CHAIN_SEQUENCE_ALIGNED_CHAIN,"
											"CHAIN_SEQUENCE_ALIGNED_SEQUENCE,"
											"CHAIN_SEQUENCE_SCORE"
										") values ("
											"%z,%z,"
											"%z,%z,"
											"'%q','%q',"
											"%z,%z,"
											"'%q','%q',%f"
										")",
									chain_sequence->chain_id, chain_sequence->sequence_id,
									chain_sequence->chain_offset_begin, chain_sequence->chain_offset_end,
									chain_sequence->author_offset_begin, chain_sequence->author_offset_end,
									chain_sequence->sequence_offset_begin, chain_sequence->sequence_offset_end,
									chain_sequence->aligned_chain, chain_sequence->aligned_sequence, chain_sequence->score );

	size_t chain_id = (size_t) mysql_insert_id( &database_connection );

	disconnect_database( &database_connection );

	return chain_id;
}
void type_chain_sequence_printf( const type_chain_sequence* chain_sequence ) {
	printf( "ChainSequence:\n\tchain id = %zd\n\tsequence id = %zd\n\tchain offset begin = %zd\n\tchain offset end = %zd\n\tauthor offset begin = *%s*\n\tauthor offset end = *%s*\n\tsequence offset begin = %zd\n\tsequence offset end = %zd\n\taligned chain = *%s*\n\taligned sequence = *%s*\n\tscore = %f\n\tcreated at %04d-%02d-%02d %02d:%02d:%02d\n",
		chain_sequence->chain_id,
		chain_sequence->sequence_id,
		chain_sequence->chain_offset_begin,
		chain_sequence->chain_offset_end,
		chain_sequence->author_offset_begin,
		chain_sequence->author_offset_end,
		chain_sequence->sequence_offset_begin,
		chain_sequence->sequence_offset_end,
		chain_sequence->aligned_chain,
		chain_sequence->aligned_sequence,
		chain_sequence->score,
		chain_sequence->creation.year, chain_sequence->creation.month, chain_sequence->creation.day, chain_sequence->creation.hour, chain_sequence->creation.minute, chain_sequence->creation.second );
}
