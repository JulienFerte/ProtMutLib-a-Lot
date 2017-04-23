#include "protein.h"

#include "../log_record.h"

type_protein* type_protein_new( void ) {
	type_protein* result = malloc( sizeof( type_protein ) );
	if( result == NULL ) {
//		write_log( LOG_PROTMUTLIBALOT_ERROR, "Error in %s: can't malloc\n", __FUNCTION__ );
		exit( EXIT_FAILURE );
	}

	result->id = 0;
	result->entry_name = NULL;
	result->name = NULL;
	result->is_wild_type = true;
	result->comment = NULL;

	return result;
}
void type_protein_free( type_protein* element ) {
	if( element != NULL ) {
		if( element->entry_name != NULL ) {
			free( element->entry_name );
		}
		if( element->name != NULL ) {
			free( element->name );
		}
		if( element->comment != NULL ) {
			free( element->comment );
		}
		free( element );
	}
}

type_protein* protein_select_from_name( configuration_struct* configuration, char* protein_entry_name ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	type_protein* result = NULL;

	execute_query( configuration, &database_connection, "select PROTEIN_ID, PROTEIN_ENTRY_NAME, PROTEIN_NAME, PROTEIN_IS_WILD_TYPE, PROTEIN_COMMENT, PROTEIN_CREATION from TB_PROTEIN where PROTEIN_ENTRY_NAME='%q' limit 1", protein_entry_name );
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	if( mysql_num_rows( stored_result ) > 0 ) {
		MYSQL_ROW first_row = mysql_fetch_row( stored_result );

		result = type_protein_new();

		parse_row( configuration, &first_row, mysql_fetch_lengths( stored_result ), "%z %s %s %1 %s %D", &(result->id), &(result->entry_name), &(result->name), &(result->is_wild_type), &(result->comment), &(result->creation) );
	}

	mysql_free_result( stored_result );
	disconnect_database( &database_connection );

	return result;
}
type_protein* protein_select_from_id( configuration_struct* configuration, size_t protein_id ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	type_protein* result = NULL;

	execute_query( configuration, &database_connection, "select PROTEIN_ID, PROTEIN_ENTRY_NAME, PROTEIN_NAME, PROTEIN_IS_WILD_TYPE, PROTEIN_COMMENT, PROTEIN_CREATION from TB_PROTEIN where PROTEIN_ID = %z limit 1", protein_id );
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	if( mysql_num_rows( stored_result ) > 0 ) {
		MYSQL_ROW first_row = mysql_fetch_row( stored_result );

		result = type_protein_new();

		parse_row( configuration, &first_row, mysql_fetch_lengths( stored_result ), "%z %s %s %1 %s %D", &(result->id), &(result->entry_name), &(result->name), &(result->is_wild_type), &(result->comment), &(result->creation) );
	}

	mysql_free_result( stored_result );
	disconnect_database( &database_connection );

	return result;
}
size_t protein_insert( configuration_struct* configuration, type_protein* protein ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	execute_query( configuration, &database_connection, "insert into TB_PROTEIN(PROTEIN_ENTRY_NAME,PROTEIN_NAME,PROTEIN_IS_WILD_TYPE,PROTEIN_COMMENT) values ('%q','%q','%u','%q')", protein->entry_name, protein->name, protein->is_wild_type ? 1 : 0, protein->comment );

	size_t protein_id = (size_t) mysql_insert_id( &database_connection );

	disconnect_database( &database_connection );

	return protein_id;
}
void type_protein_printf( type_protein* protein ) {
	printf( "Protein:\n\tid = %zd\n\tentry name = '%s'\n\tname = '%s'\n\twild-typeness = %s\n\tcomment = '%s'\n\tcreated at %04d-%02d-%02d %02d:%02d:%02d\n",
		protein->id,
		protein->entry_name,
		protein->name,
		(protein->is_wild_type ? "true" : "false"),
		protein->comment,
		protein->creation.year, protein->creation.month, protein->creation.day, protein->creation.hour, protein->creation.minute, protein->creation.second );
}
