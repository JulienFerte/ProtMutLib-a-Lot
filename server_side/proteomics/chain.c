#include "chain.h"

#include "../log_record.h"
#include "../string.h"
#include "needleman_wunsch.h"

type_chain* type_chain_new() {
	type_chain* result = malloc( sizeof( type_chain ) );
	if( result == NULL ) {
//		write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: can't malloc\n", __FUNCTION__ );
		exit( EXIT_FAILURE );
	}

	result->id = 0;

	result->structure_id = 0;
	result->name = NULL;
	result->sequence = NULL;

	return result;
}
type_chain* type_chain_build( size_t structure_id, const char* name, const char* sequence ) {
	type_chain* result = type_chain_new();

	result->id = 0;

	result->structure_id = structure_id;
	result->name = char_array_copy( name );
	result->sequence = char_array_copy( sequence );

	return result;
}
void type_chain_free( type_chain* chain ) {
	if( chain != NULL ) {
		if( chain->name != NULL ) {
			free( chain->name );
		}
		if( chain->sequence != NULL ) {
			free( chain->sequence );
		}
		free( chain );
	}
}

type_chain* chain_select_from_structure_id_name( configuration_struct* configuration, size_t structure_id, char* name ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	type_chain* result = NULL;

	execute_query( configuration, &database_connection, "select CHAIN_ID, CHAIN_STRUCTURE_ID, CHAIN_NAME, CHAIN_SEQUENCE, CHAIN_CREATION from TB_CHAIN where CHAIN_STRUCTURE_ID=%z and STRCMP(CHAIN_NAME,'%q')=0 limit 1", structure_id, name );
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	if( mysql_num_rows( stored_result ) > 0 ) {
		MYSQL_ROW first_row = mysql_fetch_row( stored_result );

		result = type_chain_new();

		parse_row( configuration, &first_row, mysql_fetch_lengths( stored_result ), "%z %z %s %s %D", &(result->id), &(result->structure_id), &(result->name), &(result->sequence), &(result->creation) );
	}

	mysql_free_result( stored_result );
	disconnect_database( &database_connection );

	return result;
}
size_t chain_insert( configuration_struct* configuration, type_chain* chain ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	execute_query( configuration, &database_connection, "insert into TB_CHAIN(CHAIN_STRUCTURE_ID, CHAIN_NAME, CHAIN_SEQUENCE) values (%z,'%q','%q')", chain->structure_id, chain->name, chain->sequence );

	size_t chain_id = (size_t) mysql_insert_id( &database_connection );

	disconnect_database( &database_connection );

	return chain_id;
}
void type_chain_printf( const type_chain* chain ) {
	printf( "Chain:\n\tid = %zd\n\tstructure id = %zd\n\tname = '%s'\n\tsequence = '%s'\n\tcreated at %04d-%02d-%02d %02d:%02d:%02d\n",
		chain->id,
		chain->structure_id,
		chain->name,
		chain->sequence,
		chain->creation.year, chain->creation.month, chain->creation.day, chain->creation.hour, chain->creation.minute, chain->creation.second );
}
