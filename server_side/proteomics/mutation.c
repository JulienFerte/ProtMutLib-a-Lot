#include "mutation.h"

#include "../log_record.h"

type_mutation* type_mutation_new( void ) {
	type_mutation* result = malloc( sizeof( type_mutation ) );
	if( result == NULL ) {
//		write_log( LOG_PROTMUTLIBALOT_ERROR, "Error in %s: can't malloc\n", __FUNCTION__ );
		exit( EXIT_FAILURE );
	}

	result->comment = NULL;
	result->point_mutations = NULL;

	return result;
}
void type_mutation_free( type_mutation* mutation ) {
	if( mutation != NULL ) {
		if( mutation->comment != NULL ) {
			free( mutation->comment );
		}

		if( mutation->point_mutations != NULL ) {
			list_aux* current = mutation->point_mutations->head;
			list_aux* next = NULL;
			while( current != NULL ) {
				next = current->next;
				type_point_mutation_free( (type_point_mutation*) current->content );
				current = next;
			}
		}

		free( mutation );
	}
}

type_mutation* mutation_select_from_id( configuration_struct* configuration, size_t mutation_id ) {
	MYSQL database_connection;
	connect_database( configuration, &database_connection );

	type_mutation* result = NULL;

	execute_query( configuration, &database_connection, "select MUTATION_ID,MUTATION_SEQUENCE_ID,MUTATION_COMMENT,MUTATION_MUTATED_SEQUENCE_ID,MUTATION_CREATION from TB_MUTATION where MUTATION_ID = %z", mutation_id );
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	if( mysql_num_rows( stored_result ) > 0 ) {
		MYSQL_ROW first_row = mysql_fetch_row( stored_result );

		result = type_mutation_new();

		parse_row( configuration, &first_row, mysql_fetch_lengths( stored_result ),
					"%z %z %s %z %D",
					&(result->id), &(result->sequence_id), &(result->comment), &(result->mutated_sequence_id), &(result->creation) );

		mysql_free_result( stored_result );

		execute_query( configuration, &database_connection, "select POINT_MUTATION_ID, POINT_MUTATION_MUTATION_ID, POINT_MUTATION_OFFSET, POINT_MUTATION_RESIDUE, POINT_MUTATION_CREATION from TB_POINT_MUTATION where POINT_MUTATION_MUTATION_ID = %z order by POINT_MUTATION_OFFSET asc", mutation_id );

		MYSQL_RES* stored_result_point_mutations = mysql_store_result( &database_connection );

		size_t number_of_point_mutations = mysql_num_rows( stored_result_point_mutations );

		result->point_mutations = list_new();

		int i = 0;
		for(; i < number_of_point_mutations; ++i ) {
			MYSQL_ROW row_point_mutation = mysql_fetch_row( stored_result_point_mutations );

			type_point_mutation* point_mutation = point_mutation_new();

			parse_row( configuration, &row_point_mutation, mysql_fetch_lengths( stored_result_point_mutations ),
						"%z %z %z %c %D",
						&(point_mutation->id), &(point_mutation->mutation_id), &(point_mutation->offset), &(point_mutation->residue), &(point_mutation->creation) );

			list_pushtail( result->point_mutations, point_mutation );
		}

		mysql_free_result( stored_result_point_mutations );
	}
	else {
		mysql_free_result( stored_result );
	}

	disconnect_database( &database_connection );

	return result;
}
void mutation_printf( type_mutation* mutation ) {
	printf( "Mutation\n\tid = %zd\n\tsequence id = %zd\n\tcomment '%s'\n\tmutated sequence id = %zd",
			mutation->id, mutation->sequence_id, mutation->comment, mutation->mutated_sequence_id );

	list_aux* current = mutation->point_mutations->head;
	list_aux* next = NULL;
	while( current != NULL ) {
		next = current->next;
		printf( "\n\toffset = %zd\tresidue = '%c'", ((type_point_mutation*) current->content)->offset, ((type_point_mutation*) current->content)->residue );
		current = next;
	}

	printf( "\n\tcreated at %04d-%02d-%02d %02d:%02d:%02d\n",
			mutation->creation.year, mutation->creation.month, mutation->creation.day, mutation->creation.hour, mutation->creation.minute, mutation->creation.second );
}
