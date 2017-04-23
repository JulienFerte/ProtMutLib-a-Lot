#include "../../configuration.h"
#include "../../mysql_connection_common.h"

#include "../../proteomics/mutation.h"
#include "../../proteomics/point_mutation.h"
#include "../../proteomics/protein.h"
#include "../../proteomics/sequence.h"
#include "../../proteomics/structure.h"

#include "../../log_record.h"

bool libmodule2_need( module_context* context, size_t sequence_id, size_t mutation_id, size_t structure_id ) {
	bool result = false;

	write_log_module( context->configuration, context->module_id, LOG_MODULE_EVENT, "Testing if Module2 needed on %zd %zd %zd", sequence_id, mutation_id, structure_id );

	MYSQL database_connection;
	connect_database( context->configuration, &database_connection );
	if( mutation_id != 0 ) {
		execute_query( context->configuration, &database_connection, "select MODULE2_ID from TB_MODULE2 where MODULE2_SEQUENCE_ID = %z and MODULE2_MUTATION_ID = %z limit 1", sequence_id, mutation_id );
	}
	else {
		execute_query( context->configuration, &database_connection, "select MODULE2_ID from TB_MODULE2 where MODULE2_SEQUENCE_ID = %z and MODULE2_MUTATION_ID is null limit 1", sequence_id );
	}
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	result = (mysql_num_rows( stored_result ) == 0);

	mysql_free_result( stored_result );
	disconnect_database( &database_connection );

	return result;
}

unsigned long libmodule2_exec( module_context* context, size_t sequence_id, size_t mutation_id, size_t structure_id ) {
	unsigned long result = 0;

	write_log_module( context->configuration, context->module_id, LOG_MODULE_EVENT, "Launching Module2 on %zd %zd %zd", sequence_id, mutation_id, structure_id );

	MYSQL database_connection;
	connect_database( context->configuration, &database_connection );

	char* sequence_content = NULL;
	mutated_sequence( context->configuration, sequence_id, mutation_id, &sequence_content );

	if( sequence_content != NULL ) {

//*** Executing Module2
//		printf( "Executing Module2\n" );

		size_t sequence_content_length = strlen( sequence_content );

		unsigned int command_line_length = strlen( "cd /absolute/path/to/Module2/src/ && exec ./module2.run " ) + sequence_content_length;
		char* command_line = malloc( sizeof( char ) * ( command_line_length + 1 ) );
		sprintf( command_line, "cd /absolute/path/to/Module2/src/ && exec ./module2.run %s", sequence_content );
		command_line[ command_line_length ] = '\0';

		unsigned int buffer_length = sequence_content_length;
		char* buffer = malloc( sizeof( char ) * ( buffer_length + 1 ) );

		char* module2_result_a = malloc( sizeof( char ) * ( sequence_content_length + 1 ) );
		char* module2_result_b = malloc( sizeof( char ) * ( sequence_content_length + 1 ) );

		FILE* module2_output = popen( command_line, "r" );
		if( module2_output != NULL ) {

//*** Parse the output from the script executing Module2
			unsigned int state = 0;

			while( fgets( buffer, buffer_length, module2_output ) != NULL ) {
				if( state == 0 ) {
					strncpy( module2_result_a, buffer, sequence_content_length );
					module2_result_a[ sequence_content_length ] = '\0';
					state = 1;
				}
				else if( state == 1 ) {
					strncpy( module2_result_b, buffer, sequence_content_length );
					module2_result_b[ sequence_content_length ] = '\0';
					state = 2;
				}
			}
		}

		fclose( module2_output );
		free( buffer );
		free( command_line );
//*** End of Module2 execution

//*** Putting the new values into the database
		unsigned long mutation_key_length = 0;
		char* mutation_key = NULL;
		if( mutation_id == 0 ) {
			mutation_key_length = 4;
			mutation_key = malloc( sizeof( char ) * ( mutation_key_length + 1 ) );
			strncpy( mutation_key, "NULL", mutation_key_length );
		}
		else {
			mutation_key_length = snprintf( NULL, 0, "%zd", mutation_id );
			mutation_key = malloc( sizeof( char ) * ( mutation_key_length + 1 ) );
			sprintf( mutation_key, "%lu", mutation_id );
		}
		mutation_key[ mutation_key_length ] = '\0';

		execute_query( context->configuration, &database_connection,
						"insert into TB_MODULE2"
										"("
											"MODULE2_SEQUENCE_ID,"
											"MODULE2_MUTATION_ID,"
											"MODULE2_RESULT_A,"
											"MODULE2_RESULT_B"
										")"
										"values"
										"("
											"%z,"
											"%s,"
											"'%q',"
											"'%q'"
										")",
						sequence_id, mutation_key,

						module2_result_a,
						module2_result_b );

		result = (unsigned long) mysql_insert_id( &database_connection );

		free( module2_result_a );
		free( module2_result_b );

		free( mutation_key );
		free( sequence_content );
	}

	disconnect_database( &database_connection );

	return result;
}
