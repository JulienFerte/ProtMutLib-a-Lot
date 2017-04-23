#include <python2.7/Python.h>

#include "../../configuration.h"
#include "../../mysql_connection_common.h"

#include "../../proteomics/mutation.h"
#include "../../proteomics/point_mutation.h"
#include "../../proteomics/protein.h"
#include "../../proteomics/sequence.h"
#include "../../proteomics/structure.h"

#include "../../log_record.h"

bool module1_need( module_context* context, size_t sequence_id, size_t mutation_id, size_t structure_id ) {
	bool result = false;

	write_log_module( context->configuration, context->module_id, LOG_MODULE_EVENT, "Testing if Module1 needed on %zd %zd %zd", sequence_id, mutation_id, structure_id );

	MYSQL database_connection;
	connect_database( context->configuration, &database_connection );
	if( mutation_id != 0 ) {
		execute_query( context->configuration, &database_connection, "select MODULE1_ID from TB_MODULE1 where MODULE1_SEQUENCE_ID=%z and MODULE1_MUTATION_ID=%z limit 1", sequence_id, mutation_id );
	}
	else {
		execute_query( context->configuration, &database_connection, "select MODULE1_ID from TB_MODULE1 where MODULE1_SEQUENCE_ID=%z and MODULE1_MUTATION_ID is null limit 1", sequence_id );
	}
	MYSQL_RES* stored_result = mysql_store_result( &database_connection );

	result = (mysql_num_rows( stored_result ) == 0);

	mysql_free_result( stored_result );
	disconnect_database( &database_connection );

	return result;
}

unsigned long module1_exec( module_context* context, size_t sequence_id, size_t mutation_id, size_t structure_id ) {
	unsigned long result = 0;

	write_log_module( context->configuration, context->module_id, LOG_MODULE_EVENT, "Launching Module1 on %zd %zd %zd", sequence_id, mutation_id, structure_id );

	MYSQL database_connection;
	connect_database( context->configuration, &database_connection );

	char* sequence = NULL;
	mutated_sequence( context->configuration, sequence_id, mutation_id, &sequence );

	if( sequence != NULL ) {

//*** Module1 is executed here.
		unsigned long command_line_length = strlen("/absolute/path/to/module1.py ") + strlen( sequence );
		char* command_line = malloc( sizeof( char ) * (command_line_length + 1) );
		sprintf( command_line, "/absolute/path/to/module1.py %s", sequence );
		command_line[ command_line_length ] = '\0';

//		printf( "Executing %s\n", command_line );

		FILE* output_stream = popen( command_line, "r" );

//*** Parses the string resulting from the command_line executed.
		unsigned long module1_output_length = 8388608;
		char* module1_output = malloc( sizeof( char ) * ( module1_output_length + 1 ) );
		memset( module1_output, '\0', module1_output_length + 1 );
		unsigned long buffer_length = 4096;
		char buffer[buffer_length + 1];
		while( fgets( buffer, buffer_length, output_stream ) != NULL ) {
			buffer[ buffer_length ] = '\0';
			strcat( module1_output, buffer );
		}

//		printf( "Module1 output: %s\n", module1_output );

		char* save_pointer;

//*** Parsing the output from the Python call into the different sought values
		char* module1_result_a = strtok_r( module1_output, "#", &save_pointer );
		char* module1_result_b = strtok_r( NULL, "#", &save_pointer );

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
			sprintf( mutation_key, "%zd", mutation_id );
		}
		mutation_key[ mutation_key_length ] = '\0';

		execute_query( context->configuration, &database_connection,
						"insert into TB_MODULE1"
							"("
								"MODULE1_SEQUENCE_ID,"
								"MODULE1_MUTATION_ID,"
								"MODULE1_RESULT_A,"
								"MODULE1_RESULT_B"
							")"
							"values"
							"("
								"%z,"
								"%s,"
								"\"%Q\","
								"\"%Q\""
							")",
						sequence_id, mutation_key,

						module1_result_a,
						module1_result_b );

		result = (unsigned long) mysql_insert_id( &database_connection );

		free( module1_output );
		free( mutation_key );
		free( sequence );
	}

	disconnect_database( &database_connection );

	return result;
}
