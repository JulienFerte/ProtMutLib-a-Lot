#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "../h_header.h"

#include "../sized_string.h"

bool file_manager_download_sifts( configuration_struct* );

bool file_manager_download_fasta( configuration_struct*, const char* );

bool file_manager_download_mmtf( configuration_struct*, const char* );
bool file_manager_download_pdb( configuration_struct*, const char* );
bool file_manager_download_mmcif( configuration_struct*, const char* );

bool file_manager_exists_sifts( configuration_struct* );

bool file_manager_exists_fasta( configuration_struct*, const char* );

bool file_manager_exists_mmtf( configuration_struct*, const char* );
bool file_manager_exists_pdb( configuration_struct*, const char* );
bool file_manager_exists_mmcif( configuration_struct*, const char* );

FILE* file_manager_open_sifts( configuration_struct* );

FILE* file_manager_open_fasta( configuration_struct*, const char* );

FILE* file_manager_open_mmtf( configuration_struct*, const char* );
FILE* file_manager_open_pdb( configuration_struct*, const char* );
FILE* file_manager_open_mmcif( configuration_struct*, const char* );

char* file_manager_file_to_char( configuration_struct*, FILE* );

bool file_manager_close( FILE* );

int file_manager_remove_fasta( configuration_struct*, const char* );

int file_manager_remove_mmtf( configuration_struct*, const char* );
int file_manager_remove_pdb( configuration_struct*, const char* );
int file_manager_remove_mmcif( configuration_struct*, const char* );

size_t auxcurl_concatenate_buffer( void*, size_t, size_t, sized_string* );

#endif
