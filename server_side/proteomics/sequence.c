#include "sequence.h"

#include "../log_record.h"

type_sequence* type_sequence_new( void ) {
	type_sequence* result = malloc( sizeof( type_sequence ) );
	if( result == NULL ) {
		fprintf( stderr, "Error in %s: can't malloc\n", __FUNCTION__ );
		exit( EXIT_FAILURE );
	}

	result->id = 0;
	result->residues = sized_string_new();
	result->uniprot_id = NULL;
	result->protein_id = 0;
	result->isoform_number = 0;
	result->isoform_name = NULL;

	return result;
}
void type_sequence_free( type_sequence* element ) {
	if( element != NULL ) {
		if( element->residues != NULL ) {
			sized_string_free( element->residues );
		}
		if( element->uniprot_id != NULL ) {
			free( element->uniprot_id );
		}
		free( element );
	}
}

type_sequence* sequence_select_from_uniprot_id_and_isoform_number( configuration_struct* configuration, char* uniprot_id, unsigned int isoform ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	type_sequence* result = NULL;

	execute_query( configuration, &database_connection, "select SEQUENCE_ID, SEQUENCE_RESIDUES, SEQUENCE_UNIPROT_ID, SEQUENCE_PROTEIN_ID, SEQUENCE_ISOFORM_NUMBER, SEQUENCE_ISOFORM_NAME, SEQUENCE_CREATION from TB_SEQUENCE where SEQUENCE_UNIPROT_ID='%q' and SEQUENCE_ISOFORM_NUMBER=%u limit 1", uniprot_id, isoform );
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	if( mysql_num_rows( stored_result ) > 0 ) {
		MYSQL_ROW first_row = mysql_fetch_row( stored_result );

		result = type_sequence_new();

		parse_row( configuration, &first_row, mysql_fetch_lengths( stored_result ),
					"%z %s %s %z %z %s %D",
					&(result->id), &(result->residues->string), &(result->uniprot_id), &(result->protein_id),
					&(result->isoform_number), &(result->isoform_name), &(result->creation) );
		result->residues->length = strlen( result->residues->string );
	}

	mysql_free_result( stored_result );
	disconnect_database( &database_connection );

	return result;
}
type_sequence* sequence_select_from_id( configuration_struct* configuration, size_t id ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	type_sequence* result = NULL;

	execute_query( configuration, &database_connection, "select SEQUENCE_ID, SEQUENCE_RESIDUES, SEQUENCE_UNIPROT_ID, SEQUENCE_PROTEIN_ID, SEQUENCE_ISOFORM_NUMBER, SEQUENCE_ISOFORM_NAME, SEQUENCE_CREATION from TB_SEQUENCE where SEQUENCE_ID=%z limit 1", id );
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	if( mysql_num_rows( stored_result ) > 0 ) {
		MYSQL_ROW first_row = mysql_fetch_row( stored_result );

		result = type_sequence_new();

		parse_row( configuration, &first_row, mysql_fetch_lengths( stored_result ),
					"%z %s %s %z %z %s %D",
					&(result->id), &(result->residues->string), &(result->uniprot_id), &(result->protein_id),
					&(result->isoform_number), &(result->isoform_name), &(result->creation) );
		result->residues->length = strlen( result->residues->string );
	}

	mysql_free_result( stored_result );
	disconnect_database( &database_connection );

	return result;
}
size_t sequence_insert( configuration_struct* configuration, type_sequence* sequence ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	execute_query( configuration, &database_connection, "insert into TB_SEQUENCE("
												"SEQUENCE_RESIDUES,"
												"SEQUENCE_UNIPROT_ID,"
												"SEQUENCE_PROTEIN_ID,"
												"SEQUENCE_ISOFORM_NUMBER,"
												"SEQUENCE_ISOFORM_NAME"
											")"
											"values"
											"("
												"'%q',"
												"'%q',"
												"%z,"
												"%z,"
												"'%q'"
											")",
							sequence->residues->string, sequence->uniprot_id, sequence->protein_id, sequence->isoform_number, sequence->isoform_name );

	size_t sequence_id = (size_t) mysql_insert_id( &database_connection );

	disconnect_database( &database_connection );

	return sequence_id;
}
void type_sequence_printf( type_sequence* sequence ) {
	printf( "Sequence:\n\tid = %zd\n\tresidues = '%s'\n\tuniprot_id = %s\n\tprotein_id = '%zd'\n\tisoform number = %zd\n\tisoform name = '%s'\n\tcreated at %04d-%02d-%02d %02d:%02d:%02d\n",
		sequence->id,
		sequence->residues->string,
		sequence->uniprot_id,
		sequence->protein_id,
		sequence->isoform_number,
		sequence->isoform_name,
		sequence->creation.year, sequence->creation.month, sequence->creation.day, sequence->creation.hour, sequence->creation.minute, sequence->creation.second );
}

void mutated_sequence( configuration_struct* configuration, size_t sequence_id, size_t mutation_id, char** result ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	type_sequence* sequence = sequence_select_from_id( configuration, sequence_id );

	if( sequence != NULL ) {

//type_sequence_printf( sequence );

		*result = malloc( sizeof( char ) * ( sequence->residues->length + 1 ) );
		strcpy( *result, sequence->residues->string );
		(*result)[ sequence->residues->length  ] = '\0';

		if( mutation_id != 0 ) {
			type_mutation* mutation = mutation_select_from_id( configuration, mutation_id );

			if( mutation != NULL ) {

//mutation_printf( mutation );

				if( sequence_id == mutation->sequence_id ) {

//*** Perform the point mutations on the sequence copied into *result
					list_aux* current = mutation->point_mutations->head;
					list_aux* next = NULL;
					type_point_mutation* point_mutation = NULL;
					while( current != NULL ) {
						next = current->next;
						point_mutation = (type_point_mutation*) current->content;
						(*result)[point_mutation->offset - 1] = point_mutation->residue;
						current = next;
					}
				}
				else {
					free( *result );
					*result = NULL;
					write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s(%s) at %u: the mutation of id %zd does not belong to the sequence of id %zd\n", __FILE__, __FUNCTION__, __LINE__, mutation_id, sequence_id );
				}

				type_mutation_free( mutation );
			}
			else {
				free( *result );
				*result = NULL;
			}

			type_sequence_free( sequence );
		}
	}
	else {
		*result = NULL;
	}

	disconnect_database( &database_connection );
}
