#include "c_header.h"

#include "../log_record.h"
#include "../thread.h"
#include "../thread_context.h"

void* put_sifts_into_database( void* parameter ) {
//*** Treats the parameter
	thread_context* context = (thread_context*) parameter;

	pthread_cleanup_push( tells_finished, context->flag );

//*** Complete the text with blanks to prepare for receiving the next texts "Putting SIFTS into the local blablabla... " within the while loop
	context->title = malloc( sizeof( char ) * 70 );
	strcpy( context->title, "Downloading the SIFTS database file" );

	file_manager_download_sifts( context->configuration );
	FILE* sifts_file = file_manager_open_sifts( context->configuration );
	char* sifts_buffer = file_manager_file_to_char( context->configuration, sifts_file );

	char* lines_sifts = sifts_buffer;
	unsigned long int number_of_lines;
	for( number_of_lines = ( unsigned long int ) 0; *lines_sifts; ++lines_sifts ) number_of_lines += ( *lines_sifts == '\n' );

//*** Variables on each line
	char pdb_id[10 + 1], chain_name[5 + 1], uniprot_id[10 + 1];
	signed long int structure_begin, structure_end, sequence_begin, sequence_end;
	char structure_author_begin[256], structure_author_end[256];

//*** Skip the first two lines
	char* current_line = strchr( strchr( sifts_buffer, '\n' ) + 1, '\n' );
	unsigned long int line_count = ( unsigned long int ) 2;
	char* next_line = NULL;

	FILE* fasta_file = NULL;
	char* fasta_buffer = NULL;

	char buffer[256];

	while( current_line ) {
		++current_line;
		next_line = strchr( current_line, '\n' );
		++line_count;

		snprintf( context->title, 70, "Putting SIFTS into the local database: %6.2f%%: %lu/%lu", 100.f * ((float) (((double) line_count) / ((double) number_of_lines))), line_count, number_of_lines );

		sscanf( current_line, "%s\t%s\t%s\t%ld\t%ld\t%s\t%s\t%ld\t%ld", pdb_id, chain_name, uniprot_id, &structure_begin, &structure_end, structure_author_begin, structure_author_end, &sequence_begin, &sequence_end );

		snprintf( buffer, 255, "Treating\tPDB id: *%s*, chain name: *%s*, UniProt id: *%s*, Sequence begin: *%ld*, Sequence end: *%ld*, Structure author begin: *%s*, Structure author end: *%s*, Structure begin: *%ld*, Structure end: *%ld*", pdb_id, chain_name, uniprot_id, sequence_begin, sequence_end, structure_author_begin, structure_author_end, structure_begin, structure_end );
		write_log( context->configuration, LOG_PROTMUTLIBALOT_EVENT, buffer );
		add_text( context, buffer );

		snprintf( buffer, 255, "Downloading FASTA file %s", uniprot_id );
		add_text( context, buffer );

		file_manager_download_fasta( context->configuration, uniprot_id );
		fasta_file = file_manager_open_fasta( context->configuration, uniprot_id );
		fasta_buffer = file_manager_file_to_char( context->configuration, fasta_file );

		snprintf( buffer, 255, "Parsing FASTA file %s", uniprot_id );
		add_text( context, buffer );

		type_fasta* fasta = type_fasta_parse( context->configuration, fasta_buffer );
		if( fasta == NULL ) {
			file_manager_remove_fasta( context->configuration, uniprot_id );
		}
		else {

//*** Get from the database the protein stored under this name
			type_protein* protein_database = protein_select_from_name( context->configuration, fasta->protein->entry_name );
			if( protein_database == NULL ) {

				snprintf( buffer, 255, "Putting new protein %s in the database", fasta->protein->name );
				add_text( context, buffer );

//*** The protein entry does not exist, the protein entry is created
				fasta->protein->id = protein_insert( context->configuration, fasta->protein );
			}
			else {

//*** The protein is stored in the database, the id of the protein is updated.
				fasta->protein->id = protein_database->id;
				type_protein_free( protein_database );
			}

//*** Treat each isoform gotten from the FASTA file
			list_aux* current_isoform = NULL;
			for( current_isoform = fasta->list_of_isoforms->head; current_isoform != NULL; current_isoform = current_isoform->next ) {
				type_sequence* sequence = (type_sequence*) (current_isoform->content);
				sequence->protein_id = fasta->protein->id;

				snprintf( buffer, 255, "Putting the isoform %u", (unsigned int) sequence->isoform_number );
				add_text( context, buffer );

//*** Get from the database the protein stored under this UniProt id and isoform number
				type_sequence* sequence_database = sequence_select_from_uniprot_id_and_isoform_number( context->configuration, uniprot_id, (unsigned int) sequence->isoform_number );
				if( sequence_database == NULL ) {
					sequence->id = sequence_insert( context->configuration, sequence );
				}
				else {
					sequence->id = sequence_database->id;
					type_sequence_free( sequence_database );
				}
			}

//*** Get from the database the structure stored under this id
			type_structure* struct_structure = structure_select( context->configuration, pdb_id, STRUCTURE_FORMAT_MMCIF );

//*** Download CIF file anyway
			snprintf( buffer, 255, "Downloading CIF file %s", pdb_id );
			add_text( context, buffer );

			file_manager_download_mmcif( context->configuration, pdb_id );

			if( struct_structure == NULL ) {

				snprintf( buffer, 255, "Putting new structure %s in the database", pdb_id );
				add_text( context, buffer );

//*** Build a type_structure struct and fill the fields with the values
				struct_structure = type_structure_new();

				struct_structure->protein_id = fasta->protein->id;

				struct_structure->format = STRUCTURE_FORMAT_MMCIF;

				size_t pdb_id_length = strlen( pdb_id );
				struct_structure->databank_id = malloc( sizeof( char ) * ( pdb_id_length + 1 ) );
				strcpy( struct_structure->databank_id, pdb_id );
				struct_structure->databank_id[ pdb_id_length ] = '\0';

				size_t mmcif_file_name_length = strlen( "./files/mmcif/" ) + pdb_id_length + strlen( ".cif" );
				struct_structure->file = malloc( sizeof( char ) * ( mmcif_file_name_length + 1 ) );
				sprintf( struct_structure->file, "./files/mmcif/%s.cif", pdb_id );
				struct_structure->file[ mmcif_file_name_length ] = '\0';

//*** Put the structure in the database
				struct_structure->id = structure_insert( context->configuration, struct_structure );
			}

//*** Get the sized array of chains from the cif file
			mmcif_sized_array_chain* mmcif_chains = mmcif_extract_file( struct_structure->file );
//*** Scan the object mmcif_sized_array_chain to fetch the chain ids related to uniprot_id
			mmcif_chain* current_chain = NULL;
			int i, j;
			for( i = 0; current_chain == NULL && i < mmcif_chains->count; ++i ) {
				for( j = 0; current_chain == NULL && j < mmcif_chains->chains[i]->name_count; ++j ) {
					if( strcmp( mmcif_chains->chains[i]->name[j], chain_name ) == 0 ) {
						current_chain = mmcif_chains->chains[i];
					}
				}
			}

			type_chain* chain_database = chain_select_from_structure_id_name( context->configuration, struct_structure->id, chain_name );
			if( chain_database == NULL ) {
				snprintf( buffer, 255, "Creating chain %s", chain_name );
				add_text( context, buffer );

				chain_database = type_chain_build( struct_structure->id, chain_name, current_chain->sequence );
				chain_database->id = chain_insert( context->configuration, chain_database );
			}

			type_sequence* current_sequence = NULL;
			list_aux* current_sequence_container = NULL;
			for( current_sequence_container = fasta->list_of_isoforms->head; current_sequence == NULL && current_sequence_container != NULL; current_sequence_container = current_sequence_container->next ) {
				if( ((type_sequence*) (current_sequence_container->content))->isoform_number == 0 ) {
					current_sequence = (type_sequence*) (current_sequence_container->content);
				}
			}

			type_chain_sequence* chain_sequence_database = type_chain_sequence_select( context->configuration, chain_database->id, current_sequence->id );
			if( chain_sequence_database == NULL ) {
				snprintf( buffer, 255, "Creating correspondance for chain %s and sequence %s", current_chain->name[0], current_sequence->uniprot_id );
				add_text( context, buffer );

				chain_sequence_database = type_chain_sequence_build( chain_database->id, current_sequence->id, current_chain->sequence, structure_begin, structure_end, current_sequence->residues->string, sequence_begin, sequence_end, structure_author_begin, structure_author_end );
				type_chain_sequence_insert( context->configuration, chain_sequence_database );
			}

//*** Clean up
			type_chain_sequence_free( chain_sequence_database );
			type_chain_free( chain_database );
			mmcif_sized_array_chain_free( mmcif_chains );
			type_structure_free( struct_structure );
			type_fasta_free( fasta );
		}

		free( fasta_buffer );
		file_manager_close( fasta_file );

		current_line = next_line;
	}

	pthread_cleanup_pop( 1 );
}
