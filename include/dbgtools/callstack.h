#ifndef DEBUG_CALLSTACK_H_INCLUDED
#define DEBUG_CALLSTACK_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct
{
	const char*  function; ///< name of function containing address of function.
	const char*  file;     ///< file where symbol is defined, might not work on all platforms.
	unsigned int line;     ///< line in file where symbol is defined, might not work on all platforms.
	unsigned int offset;   ///< offset from start of function where call was made.
} callstack_symbol_t;

/**
 * Generate a callstack from the current location in the code.
 * @param skip_frames number of frames to skip in output to addresses.
 * @param addresses is a pointer to a buffer where to store addresses in callstack.
 * @param num_addresses size of addresses.
 * @return number of addresses in callstack.
 */
int callstack( int skip_frames, void** addresses, int num_addresses );

/**
 * Translate addresses from, for example, callstack to symbol-names.
 * @param addresses list of pointers to translate.
 * @param out_syms list of callstack_symbol_t to fill with translated data, need to fit as many strings as there are ptrs in addresses.
 * @param num_addresses number of addresses in addresses
 * @param memory memory used to allocate strings stored in out_syms.
 * @param mem_size size of addresses.
 * @return number of addresses translated.
 */
int callstack_symbols( void** addresses, callstack_symbol_t* out_syms, int num_addresses, char* memory, int mem_size );

#if defined( __unix__ )

	#include <string.h>
	#include <execinfo.h>

	extern inline __attribute__((always_inline)) int callstack( int skip_frames, void** addresses, int num_addresses )
	{
		// inlined to not return address of callstack function.
		void* trace[256];
		int fetched = backtrace( trace, num_addresses + skip_frames ) - skip_frames;
		memcpy( addresses, trace + skip_frames, (size_t)fetched * sizeof(void*) );
		return fetched;
	}

#elif defined( _MSC_VER )

	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>

	__forceinline int callstack( int skip_frames, void** addresses, int num_addresses )
	{
		return RtlCaptureStackBackTrace( skip_frames, num_addresses, addresses, 0 );
	}

#else

	inline int callstack( int skip_frames, void** addresses, int num_addresses )
	{
		return 0;
	}

#endif

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // DEBUG_CALLSTACK_H_INCLUDED
