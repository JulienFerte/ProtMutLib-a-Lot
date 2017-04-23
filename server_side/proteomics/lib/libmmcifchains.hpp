#ifndef LIBMMCIFCHAINS_HPP
#define LIBMMCIFCHAINS_HPP

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		unsigned int name_count;
		char** name;
		char* sequence;
	} mmcif_chain;

	mmcif_chain* mmcif_chain_new( const char*, const char* );
	void mmcif_chain_free( mmcif_chain* );

	typedef struct {
		unsigned int count;
		mmcif_chain** chains;
	} mmcif_sized_array_chain;

	mmcif_sized_array_chain* mmcif_sized_array_chain_new( unsigned int );
	void mmcif_sized_array_chain_free( mmcif_sized_array_chain* );

	mmcif_sized_array_chain* mmcif_extract_file( const char* );

	void mmcif_print( const mmcif_sized_array_chain* );
#ifdef __cplusplus
};
#endif

#endif
