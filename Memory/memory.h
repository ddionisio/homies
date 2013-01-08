#ifndef MEMORY_H
#define MEMORY_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "..\common.h"

/********************************************************************
*																	*
*							Flags									*
*																	*
********************************************************************/

#define M_ZERO	0x01	// Used to zero out allocated memory

/********************************************************************
*																	*
*							User types								*
*																	*
********************************************************************/

// _MEMDATA: Structure used to get memory data
typedef struct _MEMDATA {
	Dword BytesUsed, BytesFree;	// Count of used bytes; count of free bytes
	Dword numUnused, numUsed;	// Count of unused entries; count of free entries
} MEMDATA, * PMEMDATA;

/********************************************************************
*																	*
*							Interface								*
*																	*
********************************************************************/

PUBLIC RETCODE MemInit (PCONFIG config);

// Purpose:	Initializes a memory manager
// Input:   Pointer to a configuration structure
// Return:  A code indicating the results of the initialization    

PUBLIC RETCODE MemTerm (char const * LogFile);

// Purpose: Deinitializes a memory manager
// Input:   Optional name of a log file
// Return:  A code indicating the results of the termination

PUBLIC RETCODE MemAlloc (void ** memory, Dword numBytes, FLAGS Options);

// Purpose:	Used to allocate memory of a given size to a particular datum
// Input:   Pointer to context to attach memory to, block size, and options
// Return:  A code indicating the results of the allocation

PUBLIC RETCODE MemFree (void ** memory);

// Purpose: Used to release memory of a particular datum
// Input:   Pointer to context to release from
// Return:  A code indicating the results of the release

PUBLIC RETCODE MemGetPattern (void * memory, char Pattern []);

// Purpose:	Used to retrieve a pattern used to identify memory
// Input:   Memory variable associated with pattern, and a buffer to load
// Return:  A code indicating the results of getting the pattern

PUBLIC RETCODE MemSetPattern (void * memory, char Pattern []);

// Purpose:	Used to associate a pattern to a memory variable
// Input:   Memory variable to associate with pattern, and pattern to assign
// Return:  A code indicating the results of setting the pattern

PUBLIC RETCODE MemGetDiagnostics (PMEMDATA MemData);

// Purpose:	Used to acquire memory diagnostics
// Input:   Pointer to a diagnostics structure
// Return:  A code indicating the results of acquiring diagnostics

#endif // MEMORY_H