#include "fasta.h"

#include "protein.h"
#include "sequence.h"

type_fasta* type_fasta_new( void ) {
	type_fasta* result = (type_fasta*) malloc( sizeof( type_fasta ) );

	result->protein = NULL;
	result->list_of_isoforms = NULL; 

	return result;
}
void type_fasta_printf( type_fasta* fasta ) {
	printf( "Protein:\n" );
	type_protein_printf( fasta->protein );
	printf( "Sequences:\n" );
	list_aux* current = NULL;
	for( current = fasta->list_of_isoforms->head; current != NULL; current = current->next ) {
		type_sequence_printf( (type_sequence*) (current->content) );
	}
}
void type_fasta_free( type_fasta* fasta ) {
	list_aux* current = fasta->list_of_isoforms->head;
	while( current != NULL ) {
		type_sequence_free( (type_sequence*) current->content );
		current = current->next;
	}
	list_destroy( fasta->list_of_isoforms );
	type_protein_free( fasta->protein );
	free( fasta );
}
