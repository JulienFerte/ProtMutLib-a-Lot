#include "point_mutation.h"

#include "../log_record.h"

type_point_mutation* point_mutation_new( void ) {
	type_point_mutation* result = (type_point_mutation*) malloc( sizeof( type_point_mutation ) );
	if( result == NULL ) {
//		write_log( LOG_PROTMUTLIBALOT_ERROR, "Error in %s: can't malloc\n", __FUNCTION__ );
		exit( EXIT_FAILURE );
	}

	return result;
}
void type_point_mutation_free( type_point_mutation* point_mutation ) {
	if( point_mutation != NULL ) {
		free( point_mutation );
	}
}

void point_mutation_printf( type_point_mutation* point_mutation ) {
	printf( "Have %zd %zd %zd %c %04d-%02d-%02d %02d:%02d:%02d\n",
			point_mutation->id, point_mutation->mutation_id, point_mutation->offset, point_mutation->residue,
			point_mutation->creation.year, point_mutation->creation.month, point_mutation->creation.day,
			point_mutation->creation.hour, point_mutation->creation.minute, point_mutation->creation.second );
}
