#ifndef I_MEMORY_H
#define I_MEMORY_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include <malloc.h>
#include <memory.h>

#include "..\common.h"
#include "memory.h"

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define PTR_OFF(Mem,Off)	((Pbyte)(Mem)+(Off))	// Retrieve a memory unit's location

/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

#define O_TYPE	0x00	// Type offset
#define O_PREV	0x04	// Previous offset
#define O_NEXT	0x08	// Next offset
#define O_TPREV	0x0C	// Typed previous offset
#define O_TNEXT	0x10	// Typed next offset
#define O_SIZE	0x14	// Size offset
#define O_DATA	0x18	// Data offset

#define M_DISJOINT 2	// Used to indicated disjoint memory sections

/********************************************************************
*																	*
*							Enumerated types						*
*																	*
********************************************************************/

typedef enum _MEMTYPE {
	MEMTYPE_USED,		// Used memory
	MEMTYPE_UNUSED,		// Unused memory
	MEMTYPE_NUMTYPES,	// Maximal number of memory configurations
	MEMTYPE_FORCE = 0xFFFFFFFF	// Force MEMTYPE to be 32-bit
} MEMTYPE;

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

///////////////////////////////////////////////////////
// _MEMBLOCK: Container for memory block information //
///////////////////////////////////////////////////////

typedef struct _MEMBLOCK * fMEMBLOCK;	// Forward reference
typedef struct _MEMBLOCK {
	MEMTYPE Type;	// Type of memory block
	fMEMBLOCK Prev;	// Pointer to previous memory block
	fMEMBLOCK Next;	// Pointer to next memory block
	union {	// Shared memory
		struct {	// Unused-block format
			fMEMBLOCK tPrev;	// Pointer to previous typed memory block
			fMEMBLOCK tNext;	// Pointer to next typed memory block
		};	// End-struct
		struct {	// Used-block format
			Byte Pattern [8];	// Pattern used to identify memory
		};	// End-struct
	};	// End-union
	Dword Size;					// Size of memory block in bytes
} MEMBLOCK, * PMEMBLOCK;

//////////////////////////////////////////////////////////
// _MEMORY: Encapsulation of memory control information //
//////////////////////////////////////////////////////////

typedef struct _MEMORY {
	PMEMBLOCK Pool;		// Main memory pool
	PMEMBLOCK Cache;	// Cache of unused memory blocks
	Dword numUnused;	// Count of unused memory blocks
	Dword numUsed;		// Count of used memory blocks
	Dword FreeBytes;	// Count of free memory bytes
	Dword FrameBytes;	// Count of frame memory bytes
} MEMORY, * PMEMORY;

/********************************************************************
*																	*
*							Implementation							*
*																	*
********************************************************************/

PRIVATE void * MemGrabBlock (Dword numBytes, FLAGS Options);

// Purpose:	Used to grab a memory block for use by outside data
// Input:	Byte size of block to grab, and option data
// Return:	A pointer to a block of memory

PRIVATE RETCODE MemReleaseBlock (void * data);

// Purpose:	Used to release a memory block used by outside data
// Input:	A data block to release
// Return:	A code indicating the results of the block release operation

PRIVATE RETCODE MemAllotBytes (Dword numBytes);

// Purpose: Used to allot bytes of memory from the memory pool
// Input:   A number of bytes to allot
// Return:  A code indicating the results of the byte allotment

PRIVATE RETCODE MemCollectBytes (Dword numBytes);

// Purpose: Used to collect alloted bytes of memory into the memory pool
// Input:   A number of bytes to collect
// Return:  A code indicating the results of the byte collection

PRIVATE RETCODE MemExtractUnusedBlock (PMEMBLOCK MemBlock);

// Purpose:	Used to extract a block from the unused blocks
// Input:	Pointer to a memory block
// Return:	A code indicating the results of the extraction

PRIVATE RETCODE MemInsertAmongUnusedBlocks (PMEMBLOCK MemBlock);

// Purpose:	Used to place a block within the unused blocks
// Input:	Pointer to a memory block
// Return:	A code indicating the results of the insertion

PRIVATE long MemAdjoinBlocks (PMEMBLOCK MemBlock);

// Purpose:	Used to adjoin memory
// Input:	Pointer to a memory block
// Return:	Amount of non-adjoinments

#endif // I_MEMORY_H