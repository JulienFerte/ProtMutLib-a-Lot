#include "c_header.h"

#include "../log_record.h"

bool file_manager_download_sifts( configuration_struct* configuration ) {
	bool result = false;

	CURL* curl_handle = curl_easy_init();

	if( curl_handle ) {
		FILE* sifts = fopen( "./files/sifts/sifts.lst", "w" );
		fclose( sifts );
		sifts = fopen( "./files/sifts/sifts.lst", "a" );

		curl_easy_setopt( curl_handle, CURLOPT_URL, configuration->path_sifts );
		curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, sifts );

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Getting SIFTS from %s into ./files/sifts/sifts.lst", configuration->path_sifts );

		CURLcode res = curl_easy_perform( curl_handle );

		if(res != CURLE_OK) {
			write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "curl_easy_perform() failed at dowloading the SIFTS file: %s", curl_easy_strerror( res ) );
			curl_easy_cleanup( curl_handle );
		}

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Successfully have SIFTS from %s", configuration->path_sifts );

		curl_easy_cleanup( curl_handle );
		fclose( sifts );
		result = true;
	}

	return result;
}

bool file_manager_download_fasta( configuration_struct* configuration, const char* fasta_num ) {
	bool result = false;

	size_t fasta_url_length = strlen( configuration->path_fasta ) - 1 + strlen( fasta_num );
	char* fasta_url = malloc( sizeof( char ) * ( fasta_url_length + 1 ) );
	snprintf( fasta_url, fasta_url_length, configuration->path_fasta, fasta_num );
	fasta_url[ fasta_url_length ] = '\0';

	CURL* curl_handle = curl_easy_init();
	if( curl_handle ) {
		size_t fasta_file_name_length = strlen( "./files/fasta/" ) + strlen( fasta_num ) + strlen( ".fasta" );
		char* fasta_file_name = malloc( sizeof( char ) * ( fasta_file_name_length + 1 ) );
		sprintf( fasta_file_name, "./files/fasta/%s.fasta", fasta_num );
		fasta_file_name[ fasta_file_name_length ] = '\0';

		FILE* fasta_file = fopen( fasta_file_name, "w" );
		fclose( fasta_file );
		fasta_file = fopen( fasta_file_name, "a" );

		curl_easy_setopt( curl_handle, CURLOPT_URL, fasta_url );
		curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, fasta_file );

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Getting the FASTA file from %s", fasta_url );

		CURLcode res = curl_easy_perform( curl_handle );

		if(res != CURLE_OK) {
			write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "curl_easy_perform() failed at getting the FASTA file %s: %s", fasta_num, curl_easy_strerror( res ) );
			curl_easy_cleanup( curl_handle );
		}

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Successfully downloaded the FASTA file from %s", fasta_url );

		curl_easy_cleanup( curl_handle );
		fclose( fasta_file );
		free( fasta_file_name );

		result = true;
	}

	free( fasta_url );

	return result;
}

bool file_manager_download_mmtf( configuration_struct* configuration, const char* mmtf_num ) {
	bool result = false;

	size_t mmtf_url_length = strlen( configuration->path_mmtf ) - 1 + strlen( mmtf_num );
	char* mmtf_url = malloc( sizeof( char ) * ( mmtf_url_length + 1 ) );
	snprintf( mmtf_url, mmtf_url_length, configuration->path_mmtf, mmtf_num );
	mmtf_url[ mmtf_url_length ] = '\0';

	CURL* curl_handle = curl_easy_init();
	if( curl_handle ) {
		size_t mmtf_file_name_length = strlen( "./files/mmtf/" ) + strlen( mmtf_num ) + strlen( ".mmtf" );
		char* mmtf_file_name = malloc( sizeof( char ) * ( mmtf_file_name_length + 1 ) );
		sprintf( mmtf_file_name, "./files/mmtf/%s.mmtf", mmtf_num );
		mmtf_file_name[ mmtf_file_name_length ] = '\0';

		FILE* mmtf_file = fopen( mmtf_file_name, "w" );
		fclose( mmtf_file );
		mmtf_file = fopen( mmtf_file_name, "a" );

		curl_easy_setopt( curl_handle, CURLOPT_URL, mmtf_url );
		curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, mmtf_file );

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Getting the MMTF file from %s", mmtf_url );

		CURLcode res = curl_easy_perform( curl_handle );

		if(res != CURLE_OK) {
			write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "curl_easy_perform() failed at getting the MMTF file %s: %s", mmtf_num, curl_easy_strerror( res ) );
			curl_easy_cleanup( curl_handle );
		}

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Successfully downloaded the MMTF file from %s", mmtf_url );

		curl_easy_cleanup( curl_handle );
		fclose( mmtf_file );
		free( mmtf_file_name );

		result = true;
	}

	free( mmtf_url );

	return result;
}
bool file_manager_download_pdb( configuration_struct* configuration, const char* pdb_num ) {
	bool result = false;

	size_t pdb_url_length = strlen( configuration->path_pdb ) - 1 + strlen( pdb_num );
	char* pdb_url = malloc( sizeof( char ) * ( pdb_url_length + 1 ) );
	snprintf( pdb_url, pdb_url_length, configuration->path_pdb, pdb_num );
	pdb_url[ pdb_url_length ] = '\0';

	CURL* curl_handle = curl_easy_init();
	if( curl_handle ) {
		size_t pdb_file_name_length = strlen( "./files/pdb/" ) + strlen( pdb_num ) + strlen( ".pdb" );
		char* pdb_file_name = malloc( sizeof( char ) * ( pdb_file_name_length + 1 ) );
		sprintf( pdb_file_name, "./files/pdb/%s.pdb", pdb_num );
		pdb_file_name[ pdb_file_name_length ] = '\0';

		FILE* pdb_file = fopen( pdb_file_name, "w" );
		fclose( pdb_file );
		pdb_file = fopen( pdb_file_name, "a" );

		curl_easy_setopt( curl_handle, CURLOPT_URL, pdb_url );
		curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, pdb_file );

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Getting the PDB file from %s", pdb_url );

		CURLcode res = curl_easy_perform( curl_handle );

		if(res != CURLE_OK) {
			write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "curl_easy_perform() failed at getting the PDB file %s: %s", pdb_num, curl_easy_strerror( res ) );
			curl_easy_cleanup( curl_handle );
		}

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Successfully downloaded the PDB file from %s", pdb_url );

		curl_easy_cleanup( curl_handle );
		fclose( pdb_file );
		free( pdb_file_name );

		result = true;
	}

	free( pdb_url );

	return result;
}
bool file_manager_download_mmcif( configuration_struct* configuration, const char* mmcif_num ) {
	bool result = false;

	size_t mmcif_url_length = strlen( configuration->path_mmcif ) - 1 + strlen( mmcif_num );
	char* mmcif_url = malloc( sizeof( char ) * ( mmcif_url_length + 1 ) );
	snprintf( mmcif_url, mmcif_url_length, configuration->path_mmcif, mmcif_num );
	mmcif_url[ mmcif_url_length ] = '\0';

	CURL* curl_handle = curl_easy_init();
	if( curl_handle ) {
		size_t mmcif_file_name_length = strlen( "./files/mmcif/" ) + strlen( mmcif_num ) + strlen( ".cif" );
		char* mmcif_file_name = malloc( sizeof( char ) * ( mmcif_file_name_length + 1 ) );
		sprintf( mmcif_file_name, "./files/mmcif/%s.cif", mmcif_num );
		mmcif_file_name[ mmcif_file_name_length ] = '\0';

		FILE* mmcif_file = fopen( mmcif_file_name, "w" );
		fclose( mmcif_file );
		mmcif_file = fopen( mmcif_file_name, "a" );

		curl_easy_setopt( curl_handle, CURLOPT_URL, mmcif_url );
		curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, mmcif_file );

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Getting the MMCIF file from %s", mmcif_url );

		CURLcode res = curl_easy_perform( curl_handle );

		if(res != CURLE_OK) {
			write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "curl_easy_perform() failed at getting the MMCIF file %s: %s", mmcif_num, curl_easy_strerror( res ) );
			curl_easy_cleanup( curl_handle );
		}

		write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Successfully downloaded the MMCIF file from %s", mmcif_url );

		curl_easy_cleanup( curl_handle );
		fclose( mmcif_file );
		free( mmcif_file_name );

		result = true;
	}

	free( mmcif_url );

	return result;
}

bool file_manager_exists_sifts( configuration_struct* configuration ) {
	return ( access( "./files/sifts/sifts.lst", F_OK ) != -1 );
}

bool file_manager_exists_fasta( configuration_struct* configuration, const char* fasta_num ) {
	size_t fasta_file_name_length = strlen( "./files/fasta/" ) + strlen( fasta_num ) + strlen( ".fasta" );
	char* fasta_file_name = malloc( sizeof( char ) * ( fasta_file_name_length + 1 ) );
	sprintf( fasta_file_name, "./files/fasta/%s.fasta", fasta_num );
	fasta_file_name[ fasta_file_name_length ] = '\0';

	bool result = ( access( fasta_file_name, F_OK ) != -1 );

	free( fasta_file_name );

	return result;
}

bool file_manager_exists_mmtf( configuration_struct* configuration, const char* mmtf_num ) {
	size_t mmtf_file_name_length = strlen( "./files/mmtf/" ) + strlen( mmtf_num ) + strlen( ".mmtf" );
	char* mmtf_file_name = malloc( sizeof( char ) * ( mmtf_file_name_length + 1 ) );
	sprintf( mmtf_file_name, "./files/mmtf/%s.mmtf", mmtf_num );
	mmtf_file_name[ mmtf_file_name_length ] = '\0';

	bool result = ( access( mmtf_file_name, F_OK ) != -1 );

	free( mmtf_file_name );

	return result;
}
bool file_manager_exists_pdb( configuration_struct* configuration, const char* pdb_num ) {
	size_t pdb_file_name_length = strlen( "./files/pdb/" ) + strlen( pdb_num ) + strlen( ".pdb" );
	char* pdb_file_name = malloc( sizeof( char ) * ( pdb_file_name_length + 1 ) );
	sprintf( pdb_file_name, "./files/pdb/%s.pdb", pdb_num );
	pdb_file_name[ pdb_file_name_length ] = '\0';

	bool result = ( access( pdb_file_name, F_OK ) != -1 );

	free( pdb_file_name );

	return result;
}
bool file_manager_exists_mmcif( configuration_struct* configuration, const char* mmcif_num ) {
	size_t mmcif_file_name_length = strlen( "./files/mmcif/" ) + strlen( mmcif_num ) + strlen( ".cif" );
	char* mmcif_file_name = malloc( sizeof( char ) * ( mmcif_file_name_length + 1 ) );
	sprintf( mmcif_file_name, "./files/mmcif/%s.cif", mmcif_num );
	mmcif_file_name[ mmcif_file_name_length ] = '\0';

	bool result = ( access( mmcif_file_name, F_OK ) != -1 );

	free( mmcif_file_name );

	return result;
}

FILE* file_manager_open_sifts( configuration_struct* configuration ) {
	return fopen( "./files/sifts/sifts.lst", "r" );
}

FILE* file_manager_open_fasta( configuration_struct* configuration, const char* fasta_num ) {
	size_t fasta_file_name_length = strlen( "./files/fasta/" ) + strlen( fasta_num ) + strlen( ".fasta" );
	char* fasta_file_name = malloc( sizeof( char ) * ( fasta_file_name_length + 1 ) );
	sprintf( fasta_file_name, "./files/fasta/%s.fasta", fasta_num );
	fasta_file_name[ fasta_file_name_length ] = '\0';

	FILE* result = fopen( fasta_file_name, "r" );

	write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Opened the FASTA file %s", fasta_file_name );

	free( fasta_file_name );

	return result;
}

FILE* file_manager_open_mmtf( configuration_struct* configuration, const char* mmtf_num ) {
	size_t mmtf_file_name_length = strlen( "./files/mmtf/" ) + strlen( mmtf_num ) + strlen( ".mmtf" );
	char* mmtf_file_name = malloc( sizeof( char ) * ( mmtf_file_name_length + 1 ) );
	sprintf( mmtf_file_name, "./files/mmtf/%s.mmtf", mmtf_num );
	mmtf_file_name[ mmtf_file_name_length ] = '\0';

	FILE* result = fopen( mmtf_file_name, "r" );

	write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Opened the MMTF file %s", mmtf_file_name );

	free( mmtf_file_name );

	return result;
}
FILE* file_manager_open_pdb( configuration_struct* configuration, const char* pdb_num ) {
	size_t pdb_file_name_length = strlen( "./files/pdb/" ) + strlen( pdb_num ) + strlen( ".pdb" );
	char* pdb_file_name = malloc( sizeof( char ) * ( pdb_file_name_length + 1 ) );
	sprintf( pdb_file_name, "./files/pdb/%s.pdb", pdb_num );
	pdb_file_name[ pdb_file_name_length ] = '\0';

	FILE* result = fopen( pdb_file_name, "r" );

	write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Opened the PDB file %s", pdb_file_name );

	free( pdb_file_name );

	return result;
}
FILE* file_manager_open_mmcif( configuration_struct* configuration, const char* mmcif_num ) {
	size_t mmcif_file_name_length = strlen( "./files/mmcif/" ) + strlen( mmcif_num ) + strlen( ".cif" );
	char* mmcif_file_name = malloc( sizeof( char ) * ( mmcif_file_name_length + 1 ) );
	sprintf( mmcif_file_name, "./files/mmcif/%s.cif", mmcif_num );
	mmcif_file_name[ mmcif_file_name_length ] = '\0';

	FILE* result = fopen( mmcif_file_name, "r" );

	write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Opened the mmCIF file %s", mmcif_file_name );

	free( mmcif_file_name );

	return result;
}

char* file_manager_file_to_char( configuration_struct* configuration, FILE* file ) {
	fseek( file, 0, SEEK_END );
	size_t length = ftell( file );
	fseek( file, 0, SEEK_SET );
	char* buffer = (char*) malloc( sizeof( char* ) * ( length + 1 ) );
	if( buffer ) {
		fread( buffer, 1, length, file );
	}
	buffer[ length ] = '\0';

	return buffer;
}

bool file_manager_close( FILE* name ) {
	return (fclose( name ) != EOF);
}

int file_manager_remove_fasta( configuration_struct* configuration, const char* fasta_num ) {
	size_t fasta_file_name_length = strlen( "./files/fasta/" ) + strlen( fasta_num ) + strlen( ".fasta" );
	char* fasta_file_name = malloc( sizeof( char ) * ( fasta_file_name_length + 1 ) );
	sprintf( fasta_file_name, "./files/fasta/%s.fasta", fasta_num );
	fasta_file_name[ fasta_file_name_length ] = '\0';

	int result = remove( fasta_file_name );

	write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Deleted the FASTA file %s", fasta_file_name );

	return result;
}

int file_manager_remove_mmtf( configuration_struct* configuration, const char* mmtf_num ) {
	size_t mmtf_file_name_length = strlen( "./files/mmtf/" ) + strlen( mmtf_num ) + strlen( ".mmtf" );
	char* mmtf_file_name = malloc( sizeof( char ) * ( mmtf_file_name_length + 1 ) );
	sprintf( mmtf_file_name, "./files/mmtf/%s.mmtf", mmtf_num );
	mmtf_file_name[ mmtf_file_name_length ] = '\0';

	int result = remove( mmtf_file_name  );

	write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Deleted the MMTF file %s", mmtf_file_name  );

	return result;
}
int file_manager_remove_pdb( configuration_struct* configuration, const char* pdb_num ) {
	size_t pdb_file_name_length = strlen( "./files/pdb/" ) + strlen( pdb_num ) + strlen( ".pdb" );
	char* pdb_file_name = malloc( sizeof( char ) * ( pdb_file_name_length + 1 ) );
	sprintf( pdb_file_name, "./files/pdb/%s.pdb", pdb_num );
	pdb_file_name[ pdb_file_name_length ] = '\0';

	int result = remove( pdb_file_name  );

	write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Deleted the PDB file %s", pdb_file_name  );

	return result;
}
int file_manager_remove_mmcif( configuration_struct* configuration, const char* mmcif_num ) {
	size_t mmcif_file_name_length = strlen( "./files/mmcif/" ) + strlen( mmcif_num ) + strlen( ".cif" );
	char* mmcif_file_name = malloc( sizeof( char ) * ( mmcif_file_name_length + 1 ) );
	sprintf( mmcif_file_name, "./files/mmcif/%s.cif", mmcif_num );
	mmcif_file_name[ mmcif_file_name_length ] = '\0';

	int result = remove( mmcif_file_name  );

	write_log( configuration, LOG_PROTMUTLIBALOT_EVENT, "Deleted the mmCIF file %s", mmcif_file_name  );

	return result;
}

size_t auxcurl_concatenate_buffer( void* data, size_t size, size_t nmemb, sized_string* buffer ) {
	size_t data_length = size * nmemb;

	size_t buffer_new_length = buffer->length + data_length;

//*** Expanding the size of the buffer string
	buffer->string = realloc( buffer->string, sizeof( char ) * ( buffer_new_length + 1 ) );
	if( buffer->string == NULL ) {
//		write_log( configuration, LOG_PROTMUTLIBALOT_ERROR, "Error in %s: can't reallocate memory", __FUNCTION__ );
		exit( EXIT_FAILURE );
	}

	strncat( buffer->string, data, data_length );
	buffer->string[ buffer_new_length ] = '\0';
	buffer->length = buffer_new_length;

	return data_length;
}
