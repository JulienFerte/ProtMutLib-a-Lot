#include "structure.h"

type_structure* type_structure_new( void ) {
	type_structure* result = malloc( sizeof( type_structure ) );
	if( result == NULL ) {
		fprintf( stderr, "Error in %s: can't malloc\n", __FUNCTION__ );
		exit( EXIT_FAILURE );
	}

	result->id = 0;
	result->protein_id = 0;
	result->format = STRUCTURE_FORMAT_MMCIF;
	result->databank_id = NULL;
	result->file = NULL;

	return result;
}
void type_structure_free( type_structure* element ) {
	if( element != NULL ) {
		if( element->databank_id != NULL ) {
			free( element->databank_id );
		}
		if( element->file != NULL ) {
			free( element->file );
		}
		free( element );
	}
}

type_structure* structure_select( configuration_struct* configuration, char* databank_id, type_structure_format file_format ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	type_structure* result = NULL;

	char file_format_extension[8];
	switch( file_format ) {
		case STRUCTURE_FORMAT_MMCIF:
				strcpy( file_format_extension, "MMCIF" );
			break;
		case STRUCTURE_FORMAT_PDB:
				strcpy( file_format_extension, "PDB" );
			break;
		case STRUCTURE_FORMAT_MMTF:
				strcpy( file_format_extension, "MMTF" );
			break;
	}

	execute_query( configuration, &database_connection, "select STRUCTURE_ID,STRUCTURE_PROTEIN_ID,STRUCTURE_DATABANK_ID,STRUCTURE_FILE,STRUCTURE_CREATION from TB_STRUCTURE where STRUCTURE_DATABANK_ID='%q' and STRUCTURE_FILE_TYPE='%q' limit 1", databank_id, file_format_extension );
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	if( mysql_num_rows( stored_result ) > 0 ) {
		result = type_structure_new();

		MYSQL_ROW first_row = mysql_fetch_row( stored_result );

		parse_row( configuration, &first_row, mysql_fetch_lengths( stored_result ),
					"%z %z %s %s %D",
					&(result->id), &(result->protein_id), &(result->databank_id), &(result->file), &(result->creation) );
		result->format = file_format;
	}

	mysql_free_result( stored_result );
	disconnect_database( &database_connection );

	return result;
}
size_t structure_insert( configuration_struct* configuration, type_structure* structure ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	char file_format_extension[8];
	switch( structure->format ) {
		case STRUCTURE_FORMAT_MMCIF:
				strcpy( file_format_extension, "MMCIF" );
			break;
		case STRUCTURE_FORMAT_PDB:
				strcpy( file_format_extension, "PDB" );
			break;
		case STRUCTURE_FORMAT_MMTF:
				strcpy( file_format_extension, "MMTF" );
			break;
	}

	execute_query( configuration, &database_connection, "insert into TB_STRUCTURE("
												"STRUCTURE_PROTEIN_ID,"
												"STRUCTURE_FILE_TYPE,"
												"STRUCTURE_DATABANK_ID,"
												"STRUCTURE_FILE"
											")"
											"values"
											"("
												"%z,"
												"'%q',"
												"'%q',"
												"'%q'"
											")",
											structure->protein_id, file_format_extension, structure->databank_id, structure->file );

	size_t structure_id = (size_t) mysql_insert_id( &database_connection );

	disconnect_database( &database_connection );

	return structure_id;
}
