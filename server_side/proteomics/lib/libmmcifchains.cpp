#include <iostream>
#include <string>

#include "./cifparse-obj-v7.105-prod-src/include/CifFile.h"
#include "./cifparse-obj-v7.105-prod-src/include/CifParserBase.h"
#include "./cifparse-obj-v7.105-prod-src/include/ISTable.h"

#include "libmmcifchains.hpp"

extern "C" {
	mmcif_chain* mmcif_chain_new( const char* names, const char* sequence ) {
		mmcif_chain* result = (mmcif_chain*) malloc( sizeof( mmcif_chain ) );

		result->name_count = 1;// By default, there is at least one chain name
		result->name = NULL;
		result->sequence = NULL;

		const char* name = names;
		while( *name != '\0' ) {
			if( *name == ',' ) {
				++(result->name_count);
			}
			++name;
		}
		result->name = (char**) malloc( sizeof( char* ) * ( result->name_count ) );

		name = names;
		const char* begining_name = name;
		unsigned int name_length = 0;
		unsigned int name_rank = 0;
		while( *name != '\0' ) {
			if( *name == ',' ) {
				result->name[ name_rank ] = (char*) malloc( sizeof( char ) * ( name_length + 1 ) );
				strncpy( result->name[ name_rank ], begining_name, name_length );
				result->name[ name_rank ][ name_length ] = '\0';
				++name_rank;
				++name;
				begining_name = name;
				name_length = 0;
			}
			else {
				++name;
				++name_length;
			}
		}
//*** For the last name in the list
		result->name[ name_rank ] = (char*) malloc( sizeof( char ) * ( name_length + 1 ) );
		strncpy( result->name[ name_rank ], begining_name, name_length );
		result->name[ name_rank ][ name_length ] = '\0';

		unsigned int sequence_length = strlen( sequence );
		result->sequence = (char*) malloc( sizeof( char ) * ( sequence_length + 1 ) );
		strcpy( result->sequence, sequence );
		result->sequence[ sequence_length ] = '\0';

		return result;
	}
	void mmcif_chain_free( mmcif_chain* chain ) {
		int i = 0;
		for(; i < chain->name_count; ++i ) {
			free( chain->name[i] );
		}
		free( chain->name );
		free( chain->sequence );
		free( chain );
	}

	mmcif_sized_array_chain* mmcif_sized_array_chain_new( unsigned int count ) {
		mmcif_sized_array_chain* result = (mmcif_sized_array_chain*) malloc( sizeof( mmcif_sized_array_chain ) );

		result->count = count;
		result->chains = (mmcif_chain**) malloc( sizeof( mmcif_chain* ) * count );

		return result;
	}
	void mmcif_sized_array_chain_free( mmcif_sized_array_chain* element ) {
		int i = 0;
		for( i = 0; i < element->count; ++i ) {
			mmcif_chain_free( (element->chains)[i] );
		}
		free( element->chains );
		free( element );
	}

	mmcif_sized_array_chain* mmcif_extract_file( const char* file_name ) {
		CifFile* cifFile = new CifFile;
		CifParser* cifParserP = new CifParser(cifFile);
		std::string diagnostics;
		cifParserP->Parse(file_name, diagnostics);

//		std::cout << "Diagnostics:\n" << diagnostics << "\nend of diagnostics\n\n";

		delete cifParserP;

		Block& block = cifFile->GetBlock(cifFile->GetFirstBlockName());

		ISTable& entity_poly = block.GetTable("entity_poly");

		unsigned int chain_count = entity_poly.GetNumRows();
		unsigned int real_count = 0;
//Remove the non-protein sequences. Keep only the rows number i where entity_poly(i, "type") == polypeptide(D) or polypeptide(L)
		int i = 0;
		for( i = 0; i < chain_count; ++i ) {
			if( strcmp( entity_poly( i, "type" ).c_str(), "polypeptide(D)" ) == 0 || strcmp( entity_poly( i, "type" ).c_str(), "polypeptide(L)" ) == 0 ) {
				++real_count;
			}
		}

//printf( "I found %u polypeptide chains\n", real_count );

		mmcif_sized_array_chain* chains = mmcif_sized_array_chain_new( real_count );
		int rank = 0;
		for( i = 0; i < chain_count; ++i ) {
			if( strcmp( entity_poly( i, "type" ).c_str(), "polypeptide(D)" ) == 0 || strcmp( entity_poly( i, "type" ).c_str(), "polypeptide(L)" ) == 0 ) {
				chains->chains[rank] = mmcif_chain_new( entity_poly(i, "pdbx_strand_id").c_str(), entity_poly(i, "pdbx_seq_one_letter_code").c_str() );

//*** Remove the occurrences of '\n' in chains->chains[rank]->sequence
				char *s, *d;
				for(s=d=(chains->chains[rank]->sequence);*d=*s;d+=(*s++!='\n'));
				++rank;
			}
		}

		delete cifFile;

		return chains;
	}

	void mmcif_print( const mmcif_sized_array_chain* chains ) {
		int i, j;
		for( i = 0; i < chains->count; ++i ) {
			std::cout << "Chain n°" << i << "\n" << chains->chains[i]->name_count << " names\nNames:\n";
			for( j = 0; j < chains->chains[i]->name_count; ++j ) {
				std::cout << "*" << chains->chains[i]->name[j] << "*\n";
			}
			std::cout << "Sequence\n*" << chains->chains[i]->sequence << "*\n";
		}
	}
};

