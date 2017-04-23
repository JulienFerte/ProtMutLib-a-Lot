#include "needleman_wunsch.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


void needleman_wunsch( const char* sequence_a, size_t length_a, char** alignment_a, const char* sequence_b, size_t length_b, char** alignment_b, double* score ) {

	size_t row, column;

	const uint8_t NEEDLEMAN_WUNSCH_NONE		= 0x00;
	const uint8_t NEEDLEMAN_WUNSCH_LEFT		= 0x01;
	const uint8_t NEEDLEMAN_WUNSCH_TOP		= 0x02;
	const uint8_t NEEDLEMAN_WUNSCH_TOP_LEFT	= 0x04;

	double** grid = (double**) malloc( sizeof( double* ) * ( length_a + 1 ) );
	for( row = 0; row <= length_a; ++row ) {
		grid[row] = (double*) malloc( sizeof( double ) * ( length_b + 1 ) );
	}
	for( row = 0; row <= length_a; ++row ) {
		for( column = 0; column <= length_b; ++column ) {
			grid[row][column] = 0;
		}
	}

	uint8_t** choices = (uint8_t**) malloc( sizeof( uint8_t* ) * ( length_a + 1 ) );
	for( row = 0; row <= length_a; ++row ) {
		choices[row] = (uint8_t*) malloc( sizeof( uint8_t ) * ( length_b + 1 ) );
	}
	for( row = 0; row <= length_a; ++row ) {
		for( column = 0; column <= length_b; ++column ) {
			choices[row][column] = NEEDLEMAN_WUNSCH_NONE;
		}
	}


	for( row = 0; row <= length_a; ++row ) {
		grid[row][0] = - ((double) row);
		choices[row][0] = NEEDLEMAN_WUNSCH_TOP;
	}
	for( column = 0; column <= length_b; ++column ) {
		grid[0][column] = - ((double) column);
		choices[0][column] = NEEDLEMAN_WUNSCH_LEFT;
	}

	double left = 0.0;
	double topleft = 0.0;
	double top = 0.0;
	double max = 0.0;
	uint8_t flags = NEEDLEMAN_WUNSCH_NONE;;

	for( row = 1; row <= length_a; ++row ) {
		for( column = 1; column <= length_b; ++column ) {
			left = grid [row] [column-1] - 1.0;
			top = grid [row-1] [column] - 1.0;
			topleft = grid [row-1] [column-1] + ( sequence_a [row-1] == sequence_b [column-1] ? 1.0 : -1.0 );

			max = 0.0;
			flags = NEEDLEMAN_WUNSCH_NONE;
			if( topleft <= top && left <= top ) {
				max = top;
				flags |= NEEDLEMAN_WUNSCH_TOP;
			}
			if( topleft <= left && top <= left ) {
				max = left;
				flags |= NEEDLEMAN_WUNSCH_LEFT;
			}
			if( left <= topleft && top <= topleft ) {
				max = topleft;
				flags |= NEEDLEMAN_WUNSCH_TOP_LEFT;
			}

			grid [row] [column] = max;
			choices [row] [column] = flags;
		}
	}


	*score = grid [length_a] [length_b];

	size_t total_length = length_a + length_b;

	char* result_a = (char*) malloc( sizeof( char ) * ( total_length + 1 ) );
	memset( result_a, ' ', total_length );
	result_a[ total_length ] = '\0';

	char* result_b = (char*) malloc( sizeof( char ) * ( total_length + 1 ) );
	memset( result_b, ' ', total_length );
	result_b[ total_length ] = '\0';

	char* current_a = result_a + total_length;
	char* current_b = result_b + total_length;

	row = length_a;
	column = length_b;

	size_t length_results = 0;

	while( row != 0 && column != 0 ) {
		--current_a;
		--current_b;
		++length_results;

//*** top means indel from sequence_a because the transition stays in the same column
		if( column > 0 && ( choices [row] [column] & NEEDLEMAN_WUNSCH_LEFT ) ) {
			*current_a = '-';
			*current_b = sequence_b[ column - 1 ];

			--column;
		}

//*** left means indel from sequence_b because the transition stays in the same row
		else if( row > 0 && ( choices [row] [column] & NEEDLEMAN_WUNSCH_TOP ) ) {
			*current_a = sequence_a[ row - 1 ];
			*current_b = '-';

			--row;
		}

//*** topleft means two residues are in correspondance, if they are the same there is conservation, otherwise there is replacement because the transition decreases in both the row and the column
		else if( row > 0 && column > 0 && ( choices [row] [column] & NEEDLEMAN_WUNSCH_TOP_LEFT ) ) {
			*current_a = sequence_a[ row - 1 ];
			*current_b = sequence_b[ column - 1 ];

			--row;
			--column;
		}
	}

	(*alignment_a) = (char*) malloc( sizeof( char ) * ( length_results + 1 ) );
	strncpy( (*alignment_a), current_a, length_results );
	(*alignment_a)[ length_results ] = '\0';

	(*alignment_b) = (char*) malloc( sizeof( char ) * ( length_results + 1 ) );
	strncpy( (*alignment_b), current_b, length_results );
	(*alignment_b)[ length_results ] = '\0';

	free( result_a );
	free( result_b );
	for( row = 0; row <= length_a; ++row ) {
		free( grid[row] );
	}
	free( grid );
	for( row = 0; row <= length_a; ++row ) {
		free( choices[row] );
	}
	free( choices );
}
