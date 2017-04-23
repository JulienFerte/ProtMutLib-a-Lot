#include "fasta.h"

#include "protein.h"
#include "sequence.h"
#include "../sized_string.h"

#include "../log_record.h"

type_fasta* type_fasta_parse( configuration_struct* configuration, const char* buffer ) {
	write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Parsing FASTA file %s", buffer );

	type_fasta* result = NULL;

	if( buffer ) {
		result = type_fasta_new();

//*** Initializing the arguments
		result->protein = type_protein_new();
		result->list_of_isoforms = list_new();

//*** Needed for parsing
		const char* fasta_parsing = buffer;
		char* fasta_equal = NULL;
		sized_string* sequence = NULL;

		type_sequence* struct_sequence = NULL;

		unsigned int state = 0;

		bool is_human = true;

		while( is_human && *fasta_parsing ) {
			switch( state ) {
				case 0:;
//*** Reading ">"
						if( *fasta_parsing != '>' ) {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}

						if( struct_sequence != NULL ) {
							list_pushtail( result->list_of_isoforms, struct_sequence );
						}
						struct_sequence = type_sequence_new();

						state = 1;
						++fasta_parsing;
					break;

				case 1:;
//*** Reading the databank id
						if( fasta_parsing[0] == 's' && fasta_parsing[1] == 'p' ) {
							state = 2;
							fasta_parsing += 2;
						}
						else if( fasta_parsing[0] == 't' && fasta_parsing[1] == 'r' ) {
							state = 12;
							fasta_parsing += 2;
						}
						else {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}
//*** now fasta_parsing points to the pipe after the databank id
					break;

				case 2:;
						if( *fasta_parsing != '|' ) {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}

						state = 3;
						++fasta_parsing;
					break;

				case 3:;
						const char* sp_begin_uniprot_id = fasta_parsing;
						size_t sp_uniprot_id_length = 0;
						while( (*fasta_parsing) != '|' && (*fasta_parsing) != '-' ) {
							++fasta_parsing;
							++sp_uniprot_id_length;
						}

						(struct_sequence->uniprot_id) = malloc( sizeof( char ) * ( sp_uniprot_id_length + 1 ) );
						strncpy( struct_sequence->uniprot_id, sp_begin_uniprot_id, sp_uniprot_id_length );
						(struct_sequence->uniprot_id)[ sp_uniprot_id_length ] = '\0';

//printf( "*%s*\n", struct_sequence->uniprot_id );

						if( *fasta_parsing == '|' ) {
							state = 4;
							++fasta_parsing;
						}
						else if( *fasta_parsing == '-' ) {
							state = 5;
							++fasta_parsing;
						}
						else {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}

					break;

				case 4:;
						const char* sp_begin_entryname = fasta_parsing;
						size_t sp_entryname_length = 0;
						while( (*fasta_parsing) != '_' ) {
							++fasta_parsing;
							++sp_entryname_length;
						}

						result->protein->entry_name = malloc( sizeof( char ) * ( sp_entryname_length + 1 ) );
						strncpy( result->protein->entry_name, sp_begin_entryname, sp_entryname_length );
						result->protein->entry_name[ sp_entryname_length ] = '\0';

//printf( "*%s*\n", result->protein->entry_name );

						++fasta_parsing;
						const char* sp_begin_species_id = fasta_parsing;
						size_t sp_species_id_length = 0;
						while( (*fasta_parsing) != ' ' ) {
							++fasta_parsing;
							++sp_species_id_length;
						}

						char* sp_species_id = malloc( sizeof( char ) * ( sp_species_id_length + 1 ) );
						strncpy( sp_species_id, sp_begin_species_id, sp_species_id_length );
						sp_species_id[ sp_species_id_length ] = '\0';

//printf( "*%s*\n", sp_species_id );

						is_human = ( strcmp( sp_species_id, "HUMAN" ) == 0 );
						if( ! is_human ) {
							type_sequence_free( struct_sequence );
							struct_sequence = NULL;
							type_fasta_free( result );
							result = NULL;
						}

						free( sp_species_id );

						if( (*fasta_parsing) != ' ' ) {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}

						state = 21;
						++fasta_parsing;
						break;

				case 5:;
						const char* sp_begin_isoform_number = fasta_parsing;
						size_t sp_isoform_number_string_length = 0;
						while( (*fasta_parsing) != '|' ) {
							++fasta_parsing;
							++sp_isoform_number_string_length;
						}

						char* sp_isoform_number_string = malloc( sizeof( char ) * ( sp_isoform_number_string_length + 1 ) );
						strncpy( sp_isoform_number_string, sp_begin_isoform_number, sp_isoform_number_string_length );
						sp_isoform_number_string[ sp_isoform_number_string_length ] = '\0';

//printf( "*%s*\n", sp_isoform_number_string );

						struct_sequence->isoform_number = (size_t) atoi( sp_isoform_number_string );

//printf( "n° %u\n", struct_sequence->isoform_number );

						free( sp_isoform_number_string );

						if( *fasta_parsing != '|' ) {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}

						state = 4;
						++fasta_parsing;
					break;

				case 6:;
						const char* sp_begin_protein_name = fasta_parsing;
						size_t sp_protein_name_length = 0;
						while( !(fasta_parsing[0] == ' ' && fasta_parsing[1] == 'O' && fasta_parsing[2] == 'S' && fasta_parsing[3] == '=') ) {
							++fasta_parsing;
							++sp_protein_name_length;
						}

						result->protein->name = malloc( sizeof( char ) * ( sp_protein_name_length + 1 ) );
						strncpy( result->protein->name, sp_begin_protein_name, sp_protein_name_length );
						result->protein->name[ sp_protein_name_length ] = '\0';

//printf( "*%s*\n", result->protein->name );

						if( fasta_parsing[0] != ' ' && fasta_parsing[1] != 'O' && fasta_parsing[2] != 'S' && fasta_parsing[3] != '=' ) {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}

						state = 7;
						fasta_parsing += 4;
					break;

				case 7:;
						const char* sp_begin_organism_name = fasta_parsing;
						size_t sp_organism_name_length = 0;
						while( !(fasta_parsing[0] == ' ' && ((fasta_parsing[1] == 'P' && fasta_parsing[2] == 'E')
									|| (fasta_parsing[1] == 'G' && fasta_parsing[2] == 'N')) && fasta_parsing[3] == '=') && *fasta_parsing != '\n' ) {
							++fasta_parsing;
							++sp_organism_name_length;
						}

						char* sp_organism_name = malloc( sizeof( char ) * ( sp_organism_name_length + 1 ) );
						strncpy( sp_organism_name, sp_begin_organism_name, sp_organism_name_length );
						sp_organism_name[ sp_organism_name_length ] = '\0';

//printf( "*%s*\n", sp_organism_name );

						free( sp_organism_name );

						if( fasta_parsing[1] == 'P' ) {
							state = 8;
							fasta_parsing += 4;
						}
						else if( fasta_parsing[1] == 'G' ) {
							state = 9;
							fasta_parsing += 4;
						}
						else if( *fasta_parsing == '\n' ) {
							state = 11;
							++fasta_parsing;
						}
						else {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}
					break;

				case 8:;
						const char* sp_begin_protein_existence = fasta_parsing;
						size_t sp_protein_existence_length = 0;
						while( !(fasta_parsing[0] == ' ' && fasta_parsing[1] == 'S' && fasta_parsing[2] == 'V' && fasta_parsing[3] == '=') ) {
							++fasta_parsing;
							++sp_protein_existence_length;
						}

						char* sp_protein_existence = malloc( sizeof( char ) * ( sp_protein_existence_length + 1 ) );
						strncpy( sp_protein_existence, sp_begin_protein_existence, sp_protein_existence_length );
						sp_protein_existence[ sp_protein_existence_length ] = '\0';

//printf( "*%s*\n", sp_protein_existence );

						free( sp_protein_existence );

						state = 10;
						fasta_parsing += 4;
					break;

				case 9:;
						const char* sp_begin_gene_name = fasta_parsing;
						size_t sp_gene_name_length = 0;
						while( !(fasta_parsing[0] == ' ' && fasta_parsing[1] == 'P' && fasta_parsing[2] == 'E' && fasta_parsing[3] == '=')
										&& *fasta_parsing!= '\n' ) {
							++fasta_parsing;
							++sp_gene_name_length;
						}

						char* sp_gene_name = malloc( sizeof( char ) * ( sp_gene_name_length + 1 ) );
						strncpy( sp_gene_name, sp_begin_gene_name, sp_gene_name_length );
						sp_gene_name[ sp_gene_name_length ] = '\0';

//printf( "*%s*\n", sp_gene_name );

						free( sp_gene_name );

						if( *fasta_parsing == '\n' ) {
							state = 11;
							++fasta_parsing;
						}
						else {
							state = 8;
							fasta_parsing += 4;
						}
					break;

				case 10:;
						const char* sp_begin_sequence_version = fasta_parsing;
						size_t sp_sequence_version_length = 0;
						while( !(fasta_parsing[0] == '\n') ) {
							++fasta_parsing;
							++sp_sequence_version_length;
						}

						char* sp_sequence_version = malloc( sizeof( char ) * ( sp_sequence_version_length + 1 ) );
						strncpy( sp_sequence_version, sp_begin_sequence_version, sp_sequence_version_length );
						sp_sequence_version[ sp_sequence_version_length ] = '\0';

//printf( "*%s*\n", sp_sequence_version );

						free( sp_sequence_version );

						state = 11;
						++fasta_parsing;
					break;

				case 11:;
						const char* sp_begin_sequence_line = fasta_parsing;
						size_t sp_sequence_line_length = 0;
						while( !((*fasta_parsing) == '\n') ) {
							++fasta_parsing;
							++sp_sequence_line_length;
						}

						sized_string_concatn( struct_sequence->residues, sp_begin_sequence_line, sp_sequence_line_length );

//printf( "*%s*\n", "sequence" );

						++fasta_parsing;
						if( *fasta_parsing == '>' || *fasta_parsing == '\0' ) {
							state = 0;// when *fasta_parsing == '\0', the state doesn't matter anyway
						}
						else if( 'A' <= *fasta_parsing && *fasta_parsing <= 'Z' ) {
							//state = 11;
						}
						else {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}
					break;

				case 12:;
						if( *fasta_parsing != '|' ) {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}

						state = 13;
						++fasta_parsing;
					break;

				case 13:;
						const char* tr_begin_uniprot_id = fasta_parsing;
						size_t tr_uniprot_id_length = 0;
						while( (*fasta_parsing) != '|' ) {
							++fasta_parsing;
							++tr_uniprot_id_length;
						}

						struct_sequence->uniprot_id = malloc( sizeof( char ) * ( tr_uniprot_id_length + 1 ) );
						strncpy( struct_sequence->uniprot_id, tr_begin_uniprot_id, tr_uniprot_id_length );
						struct_sequence->uniprot_id[ tr_uniprot_id_length ] = '\0';

//printf( "*%s*\n", struct_sequence->uniprot_id );

						if( *fasta_parsing == '|' ) {
							state = 14;
							++fasta_parsing;
						}
						else {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}
					break;

				case 14:;
						const char* tr_begin_entryname = fasta_parsing;
						size_t tr_entryname_length = 0;
						while( (*fasta_parsing) != '_' ) {
							++fasta_parsing;
							++tr_entryname_length;
						}

						result->protein->entry_name = malloc( sizeof( char ) * ( tr_entryname_length + 1 ) );
						strncpy( result->protein->entry_name, tr_begin_entryname, tr_entryname_length );
						result->protein->entry_name[ tr_entryname_length ] = '\0';

//printf( "*%s*\n", result->protein->entry_name );

						++fasta_parsing;
						const char* tr_begin_species_id = fasta_parsing;
						size_t tr_species_id_length = 0;
						while( (*fasta_parsing) != ' ' ) {
							++fasta_parsing;
							++tr_species_id_length;
						}

						char* tr_species_id = malloc( sizeof( char ) * ( tr_species_id_length + 1 ) );
						strncpy( tr_species_id, tr_begin_species_id, tr_species_id_length );
						tr_species_id[ tr_species_id_length ] = '\0';

//printf( "*%s*\n", tr_species_id );

						is_human = ( strcmp( tr_species_id, "HUMAN" ) == 0 );
						if( ! is_human ) {
							type_sequence_free( struct_sequence );
							struct_sequence = NULL;
							type_fasta_free( result );
							result = NULL;
						}

						free( tr_species_id );

						if( (*fasta_parsing) != ' ' ) {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}

						state = 15;
						++fasta_parsing;
					break;

				case 15:;
						const char* tr_begin_protein_name = fasta_parsing;
						size_t tr_protein_name_length = 0;
						while( !(fasta_parsing[0] == ' ' && fasta_parsing[1] == 'O' && fasta_parsing[2] == 'S' && fasta_parsing[3] == '=') ) {
							++fasta_parsing;
							++tr_protein_name_length;
						}

						result->protein->name = malloc( sizeof( char ) * ( tr_protein_name_length + 1 ) );
						strncpy( result->protein->name, tr_begin_protein_name, tr_protein_name_length );
						result->protein->name[ tr_protein_name_length ] = '\0';

//printf( "*%s*\n", result->protein->name );

						if( fasta_parsing[0] != ' ' && fasta_parsing[1] != 'O' && fasta_parsing[2] != 'S' && fasta_parsing[3] != '=' ) {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}

						state = 16;
						fasta_parsing += 4;
					break;

				case 16:;
						const char* tr_begin_organism_name = fasta_parsing;
						size_t tr_organism_name_length = 0;
						while( !(fasta_parsing[0] == ' ' && ((fasta_parsing[1] == 'P' && fasta_parsing[2] == 'E')
									|| (fasta_parsing[1] == 'G' && fasta_parsing[2] == 'N')) && fasta_parsing[3] == '=') && *fasta_parsing != '\n' ) {
							++fasta_parsing;
							++tr_organism_name_length;
						}

						char* tr_organism_name = malloc( sizeof( char ) * ( tr_organism_name_length + 1 ) );
						strncpy( tr_organism_name, tr_begin_organism_name, tr_organism_name_length );
						tr_organism_name[ tr_organism_name_length ] = '\0';

//printf( "*%s*\n", tr_organism_name );

						is_human = (strcmp( tr_organism_name, "Homo sapiens" ) == 0);
						if( !is_human ) {
							type_sequence_free( struct_sequence );
							struct_sequence = NULL;
							type_fasta_free( result );
							result = NULL;
						}

						free( tr_organism_name );

						if( fasta_parsing[1] == 'P' ) {
							state = 17;
							fasta_parsing += 4;
						}
						else if( fasta_parsing[1] == 'G' ) {
							state = 18;
							fasta_parsing += 4;
						}
						else {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}
					break;

				case 17:;
						const char* tr_begin_protein_existence = fasta_parsing;
						size_t tr_protein_existence_length = 0;
						while( !(fasta_parsing[0] == ' ' && fasta_parsing[1] == 'S' && fasta_parsing[2] == 'V' && fasta_parsing[3] == '=') ) {
							++fasta_parsing;
							++tr_protein_existence_length;
						}

						char* tr_protein_existence = malloc( sizeof( char ) * ( tr_protein_existence_length + 1 ) );
						strncpy( tr_protein_existence, tr_begin_protein_existence, tr_protein_existence_length );
						tr_protein_existence[ tr_protein_existence_length ] = '\0';

//printf( "*%s*\n", tr_protein_existence );

						free( tr_protein_existence );

						state = 19;
						fasta_parsing += 4;
					break;

				case 18:;
						const char* tr_begin_gene_name = fasta_parsing;
						size_t tr_gene_name_length = 0;
						while( !(fasta_parsing[0] == ' ' && fasta_parsing[1] == 'P' && fasta_parsing[2] == 'E' && fasta_parsing[3] == '=') ) {
							++fasta_parsing;
							++tr_gene_name_length;
						}

						char* tr_gene_name = malloc( sizeof( char ) * ( tr_gene_name_length + 1 ) );
						strncpy( tr_gene_name, tr_begin_gene_name, tr_gene_name_length );
						tr_gene_name[ tr_gene_name_length ] = '\0';

//printf( "*%s*\n", tr_gene_name );

						free( tr_gene_name );

						state = 17;
						fasta_parsing += 4;
					break;

				case 19:;
						const char* tr_begin_sequence_version = fasta_parsing;
						size_t tr_sequence_version_length = 0;
						while( !(fasta_parsing[0] == '\n') ) {
							++fasta_parsing;
							++tr_sequence_version_length;
						}

						char* tr_sequence_version = malloc( sizeof( char ) * ( tr_sequence_version_length + 1 ) );
						strncpy( tr_sequence_version, tr_begin_sequence_version, tr_sequence_version_length );
						tr_sequence_version[ tr_sequence_version_length ] = '\0';

//printf( "*%s*\n", tr_sequence_version );

						free( tr_sequence_version );

						state = 22;
						++fasta_parsing;
					break;

				case 20:;
						const char* sp_begin_isoform_name = fasta_parsing;
						size_t sp_isoform_name_length = 0;
						while( !(fasta_parsing[0] == ' ' && fasta_parsing[1] == 'o' && fasta_parsing[2] == 'f' && fasta_parsing[3] == ' ') ) {
							++fasta_parsing;
							++sp_isoform_name_length;
						}

						struct_sequence->isoform_name = malloc( sizeof( char ) * ( sp_isoform_name_length + 1 ) );
						strncpy( struct_sequence->isoform_name, sp_begin_isoform_name, sp_isoform_name_length );
						struct_sequence->isoform_name[ sp_isoform_name_length ] = '\0';

//printf( "*%s*\n", struct_sequence->isoform_name );

						state = 6;
						fasta_parsing += 4;
					break;

				case 21:;
						if( fasta_parsing[0] == 'I' && fasta_parsing[1] == 's' && fasta_parsing[2] == 'o' && fasta_parsing[3] == 'f'
							&& fasta_parsing[4] == 'o' && fasta_parsing[5] == 'r' && fasta_parsing[6] == 'm' && fasta_parsing[7] == ' ' ) {
							state = 20;
							fasta_parsing += 8;
						}
						else {
							state = 6;
						}
					break;

				case 22:;
						const char* tr_begin_sequence_line = fasta_parsing;
						size_t tr_sequence_line_length = 0;
						while( !((*fasta_parsing) == '\n') ) {
							++fasta_parsing;
							++tr_sequence_line_length;
						}

						sized_string_concatn( struct_sequence->residues, tr_begin_sequence_line, tr_sequence_line_length );

//printf( "*%s*\n", "sequence" );

						++fasta_parsing;
						if( *fasta_parsing == '>' || *fasta_parsing == '\0' ) {
							state = 0;// when *fasta_parsing == '\0' the state doesn't matter anyway
						}
						else if( 'A' <= *fasta_parsing && *fasta_parsing <= 'Z' ) {
							//state = 22;
						}
						else {
							write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: %s at %u", __FILE__, __FUNCTION__, __LINE__ );
							exit( EXIT_FAILURE );
						}
					break;

				default:;
					break;
			}
		}

		if( struct_sequence != NULL ) {
			list_pushtail( result->list_of_isoforms, struct_sequence );
		}
	}

	return result;
}
