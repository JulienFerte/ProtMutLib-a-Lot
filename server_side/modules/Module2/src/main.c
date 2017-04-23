#include <stdio.h>
#include <stdlib.h>

int main( int argc, char** argv ) {
	if( argc >= 1 ) {
		printf( "%s\n%s\n", argv[1], argv[1] );
	}
}
