	/********************************************************************
	*																	*
	*							Includes								*
	*																	*
	********************************************************************/

	#include "i_memory.h"

	/********************************************************************
	*																	*
	*							Internals								*
	*																	*
	********************************************************************/

	PRIVATE MEMORY MemMgr;	// Memory manager; linkage restricted to memory library

	/********************************************************************************
	*																				*
	*								MemInit											*
	*																				*
	********************************************************************************/	

	// Purpose:	Initializes a memory manager
	// Input:   No input
	// Return:  A code indicating the results of the initialization 

	PUBLIC RETCODE MemInit (CONFIG * config)
	{
		Dword PoolSize;	// Size to allot for memory frame

		assert(config);	// Verify that config points to valid memory

		PoolSize = config->PoolSize;	// Get requested size of memory frame

		//////////////////////////////////////////////////////////////////
		// Step one: Configure memory manager with external information //
		//////////////////////////////////////////////////////////////////

		PoolSize = ALIGNED(PoolSize);	// Align pool size

		// Check for meaningless configurations
		ASSERT_MSG(PoolSize != 0,"Invalid pool size: no operation","MemInit");

		///////////////////////////////////////////////
		// Step two: Allocate memory pool to manager //
		///////////////////////////////////////////////

		MemMgr.Pool = (PMEMBLOCK) malloc (PoolSize);
		// Allocate memory for manager object and pool

		//////////////////////////////////////////////////////////
		// Step three: Verify that memory pool could be created //
		//////////////////////////////////////////////////////////

		if (MemMgr.Pool == NULL)	// Ascertain that malloc succeeded
		{
			MESSAGE_BOX("Unable to allocate memory","MemInit");
			return RETCODE_FAILURE;	// Return failure
		}

		//////////////////////////////////////////////
		// Step four: Initialize the memory manager //
		//////////////////////////////////////////////
		
		MemMgr.FrameBytes = PoolSize;	// Load up amount of frame memory
		MemMgr.FreeBytes  = PoolSize;	// Load up amount of available memory

		MemMgr.Pool->Type = MEMTYPE_UNUSED;				// Prepare pool for use
		MemMgr.Pool->Size = PoolSize - sizeof(MEMBLOCK);// Set pool amount available

		MemMgr.Pool->Prev = MemMgr.Pool;// Loop pool backward
		MemMgr.Pool->Next = MemMgr.Pool;// Loop pool forward

		MemInsertAmongUnusedBlocks (MemMgr.Pool);	// Initialize pool

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemTerm											*
	*																				*
	********************************************************************************/	

	// Purpose:	Deinitializes a memory manager
	// Input:   Optional name of a log file
	// Return:  A code indicating the results of the termination

	PUBLIC RETCODE MemTerm (char const * LogFile)
	{		
		if (LogFile != NULL)	// User requests diagnostics
		{
			PMEMBLOCK MemBlock = MemMgr.Pool;	// Refer to first memory block

			FILE * fpLog;	// File used to log debug info

			char Pattern [9] = {0};	// Buffer used to retrieve memory patterns

			fpLog = fopen (LogFile, "wt");	// Create a log file

			if (fpLog != NULL)	// Log diagnostics if file creation was successful
			{
				// Output used entry/byte information
				fprintf (fpLog, "%d unfreed entries\n", MemMgr.numUsed);
				fprintf (fpLog, "%d unfreed bytes\n",   MemMgr.FrameBytes - MemMgr.FreeBytes);

				if (MemMgr.numUsed != 0) do {	// If memory is unfreed, list instances
					if (MemBlock->Type == MEMTYPE_USED)	// Check if memory block is not freed
					{
						// Retrieve memory pattern
						MemGetPattern (&MemBlock [BASE_EXTENT], Pattern);

						if (Pattern [0] == '\0')	// Check for unlabeled blocks
							fprintf (fpLog, "No pattern\n");	// Print message

						else	// Labeled blocks
							fprintf (fpLog, "Entry: %s\n", Pattern);// Print pattern

						fprintf (fpLog, "Bytes used: %u\n", MemBlock->Size);// Print memory consumption

						Pattern [0] = '\0';	// Reset to prepare next block
					}

					MemBlock = MemBlock->Next;	// Go to next block in memory chain
				} while (MemBlock != MemMgr.Pool);	// Loop through all blocks

				fclose (fpLog);	// Close the log file
			}
		}

		free (MemMgr.Pool), MemMgr.Pool = NULL;
		// Deallocate and deactivate memory manager object and pool

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemAlloc										*
	*																				*
	********************************************************************************/	
	
	// Purpose:	Used to allocate memory of a given size to a particular datum
	// Input:   Pointer to context to attach memory to, block size, and options
	// Return:  A memory datum indicating private memory information

	PUBLIC RETCODE MemAlloc (void ** memory, Dword numBytes, FLAGS Options)
	{
		assert(memory);	// Verify that memory points to valid memory

		//////////////////////////////////////////////////////////////////
		// Step one: int-align memory request to prevent fragmentations //
		//////////////////////////////////////////////////////////////////

		// Verify meaningful allocation request
		ASSERT_MSG(numBytes != 0,"No operation: MemAlloc failed","Error");

		numBytes = ALIGNED(numBytes);
		// Align requested amount

		/////////////////////////////////////////
		// Step two: Request a block of memory //
		/////////////////////////////////////////

		*memory = MemGrabBlock (numBytes, Options);
		// Allocate requested amount of memory

#ifndef NDEBUG	// Debug check
		if (*memory == NULL)	// Check whether memory was not allocated
			return RETCODE_FAILURE;	// Allocation failed
#endif // NDEBUG

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemFree											*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to release memory of a particular datum
	// Input:   Pointer to context to release from
	// Return:  A code indicating the results of the release

	PUBLIC RETCODE MemFree (void ** memory)
	{
		assert(memory);	// Verify that memory points to valid memory

		// Verify that attempt is made to free unreleased memory
		ASSERT_MSG(*memory != NULL,"Attempt to free invalid memory: MemFree failed","Error");

		///////////////////////////////////////
		// Step one: Release block of memory //
		///////////////////////////////////////

		MemReleaseBlock (*memory);
		// Deallocate memory

		///////////////////////////////////////////////
		// Step two: Nullify memory to prevent reuse //
		///////////////////////////////////////////////

		*memory = NULL;	// Invalidate context

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemGetPattern									*
	*																				*
	********************************************************************************/

	// Purpose:	Used to retrieve a pattern used to identify memory
	// Input:   Memory variable associated with pattern, and a buffer to load
	// Return:  A code indicating the results of getting the pattern

	PUBLIC RETCODE MemGetPattern (void * memory, char Pattern [])
	{
		PMEMBLOCK MemBlock;	// Owner memory block

		int index;			// Loop variable

		assert(memory), assert(Pattern);// Verify that memory and Pattern point to valid memory

		MemBlock = (PMEMBLOCK) ((Pbyte) memory - sizeof(MEMBLOCK));
		// Refer to memory's owning block

		for (index = 0; index < 8; ++index)	// Loop through pattern
		{
			Pattern [index] = MemBlock->Pattern [index];	// Get byte

			if (MemBlock->Pattern [index] == '\0')	// Check for null bytes
				break;	// Pattern is complete
		}

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemSetPattern									*
	*																				*
	********************************************************************************/

	// Purpose:	Used to associate a pattern to a memory variable
	// Input:   Memory variable to associate with pattern, and pattern to assign
	// Return:  A code indicating the results of setting the pattern

	PUBLIC RETCODE MemSetPattern (void * memory, char Pattern [])
	{
		PMEMBLOCK MemBlock;	// Owner memory block

		int index;			// Loop variable

		assert(memory), assert(Pattern);// Verify that memory and Pattern point to valid memory

		MemBlock = (PMEMBLOCK)((Pbyte) memory - sizeof(MEMBLOCK));
		// Refer to memory's owning block

		for (index = 0; index < 8; ++index)	// Loop through pattern
		{
			MemBlock->Pattern [index] = Pattern [index];	// Set byte

			if (MemBlock->Pattern [index] == '\0')	// Check for null bytes
				break;	// Pattern is complete
		}

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemGrabBlock									*
	*																				*
	********************************************************************************/

	// Purpose:	Used to grab a memory block for use by outside data
	// Input:	Byte size of block to grab, and option data
	// Return:	A pointer to a block of memory

	PRIVATE void * MemGrabBlock (Dword numBytes, FLAGS Options)
	{
		PMEMBLOCK MemBlock = MemMgr.Cache;	// Current memory block

		Dword Padding;						// Padding amount
		Dword NumBlocks = MemMgr.numUnused;	// Number of unused blocks in manager

		////////////////////////////////////////////////////////////////////////////
		// Step two: Scan through memory pool in search of adequately large block //
		////////////////////////////////////////////////////////////////////////////

		// Low-level operation: Scan pool for adequately large memory block 
		_asm {
			mov esi, dword ptr [MemBlock];	// Store address of previous block
			mov edi, dword ptr [MemBlock];	// Store address of next block
			mov eax, dword ptr [numBytes];	// Store size of request
			mov ecx, dword ptr [NumBlocks];	// Store block count
			shr ecx, 1;	// Cut amount in half
			adc ecx, 1;	// Correct for odd amounts (and first element)
$findMem:	// Loop through unused memory blocks
			cmp dword ptr [esi].O_SIZE, eax;	// Check whether previous block is at least of requested capacity
			jae $memPrev;	// If block is adequate, break out of loop
			cmp dword ptr [edi].O_SIZE, eax;	// Check whether next block is at least of requested capacity
			jae $memNext;	// If block is adequate, break out of loop
		 	mov esi, dword ptr [esi].O_TPREV;	// Regress previous block
			mov edi, dword ptr [edi].O_TNEXT;	// Advance next block
			dec ecx;// Reduce search count
			jnz $findMem;	// Iterate again
			mov dword ptr [MemBlock], 0;	// Set block to NULL
			jmp $memDone;	// Search failed: complete
$memPrev:	// Grab previous block
			mov dword ptr [MemBlock], esi;	// Store previous block as valid block
			jmp $memDone;	// Branch to memory resolution
$memNext:	// Grab next block
			mov dword ptr [MemBlock], edi;	// Store next block as valid block
$memDone:	// Resolve pool search
		}

#ifndef NDEBUG	// Debug check
		if (MemBlock == NULL)	// Check for search failure
			return NULL;// Return null block
#endif	// NDEBUG

		////////////////////////////////////
		// Step three: Restructure memory //
		////////////////////////////////////

		MemExtractUnusedBlock (MemBlock);	// Detach memory block from unused block section

		Padding	= MemBlock->Size - numBytes;// Get difference between block size and request
										
		MemBlock->Size = numBytes;		// Set alloted block's size
		MemBlock->Type = MEMTYPE_USED;	// Set alloted block's type

		if (Padding > sizeof(MEMBLOCK))	// Check whether room exists for a new data block
		{
			PMEMBLOCK NewBlock = (PMEMBLOCK) PTR_OFF(&MemBlock [BASE_EXTENT],numBytes);
			// Grab a new block at the beginning of the padding area

			NewBlock->Size = Padding - sizeof(MEMBLOCK);// Set size of new memory block
			NewBlock->Type = MEMTYPE_UNUSED;			// Set type of new memory block

			NewBlock->Prev = MemBlock;			// Bind new block back
			NewBlock->Next = MemBlock->Next;	// Bind new block ahead

			NewBlock->Prev->Next = NewBlock;	// Bind previous block ahead to new block
			NewBlock->Next->Prev = NewBlock;	// Bind next block back to new block

			MemInsertAmongUnusedBlocks (NewBlock);	// Attach new block to unused block section
		}

		else if (Padding > 0)			// Check for 0 < Padding <= block size
			MemBlock->Size += Padding;	// Accumulate padding

		assert(MemAllotBytes (MemBlock->Size + sizeof(MEMBLOCK)) == RETCODE_SUCCESS);
		// DEBUG: Allot memory for the memory block

		++MemMgr.numUsed;	// Document addition of used block

		////////////////////////////////////////
		// Step four: Initialize memory block //
		////////////////////////////////////////

		// Low-level operation: Zero-fill all bytes in data chunk
		if (GETFLAGS(Options,M_ZERO)) _asm {	// Check for request to zero memory
			cld;	// Guarantee forward string operation
			xor	eax, eax;					// Set fill value to 0
			mov	esi, dword ptr [MemBlock];	// Set fill context to MemBlock
			mov ecx, dword ptr [esi].O_SIZE;// Set fill count to numBytes
			shr	ecx, 2;						// Adjust fill count from bytes to dwords
			lea edi, dword ptr [esi].O_DATA;// Set fill region to MemBlock's data section
			rep stosd;						// Blast dwords to MemBlock's data section
		}

		MemBlock->Pattern [0] = '\0';	// Zero out byte for pattern usage

		// Verify that a used block is being given to the user
		ASSERT_MSG(MemBlock->Type == MEMTYPE_USED,"Invalid type: MemGrabBlock failed","Error");

		return &MemBlock [BASE_EXTENT];
		// Return pointer to data chunk
	}

	/********************************************************************************
	*																				*
	*								MemGetDiagnostics								*
	*																				*
	********************************************************************************/

	// Purpose:	Used to acquire memory diagnostics
	// Input:   Pointer to a diagnostics structure
	// Return:  A code indicating the results of acquiring diagnostics

	PUBLIC RETCODE MemGetDiagnostics (MEMDATA * MemData)
	{
		assert(MemData);// Verify that MemData points to valid memory

		MemData->BytesUsed = MemMgr.FrameBytes - MemMgr.FreeBytes;	// Acquire used byte count
		MemData->BytesFree = MemMgr.FreeBytes;						// Acquire free byte count

		MemData->numUnused = MemMgr.numUnused;	// Acquire unused block count
		MemData->numUsed   = MemMgr.numUsed;	// Acquire used block count

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemReleaseBlock									*
	*																				*
	********************************************************************************/

	// Purpose:	Used to release a memory block used by outside data
	// Input:	A data block to release
	// Return:	A code indicating the results of the block release operation

	PRIVATE RETCODE MemReleaseBlock (void * data)
	{
		PMEMBLOCK MemBlock;	// Current memory block

		/////////////////////////////////////////////////////////////////////////
		// Step one: Scan through memory pool in search of data's owning block //
		/////////////////////////////////////////////////////////////////////////

		MemBlock = (PMEMBLOCK) ((Pbyte) data - sizeof(MEMBLOCK));	// Grab memory block from list

		// Verify that a used block is to be released
		ASSERT_MSG(MemBlock->Type == MEMTYPE_USED,"Invalid type: MemReleaseBlock failed","Error");

		assert(MemCollectBytes (MemBlock->Size + sizeof(MEMBLOCK)) == RETCODE_SUCCESS);
		// DEBUG: Collect memory from the memory block

		//////////////////////////////////
		// Step two: Restructure memory //
		//////////////////////////////////

		--MemMgr.numUsed;	// Document removal of used block

		MemInsertAmongUnusedBlocks (MemBlock);	// Attach memory block to unused block section

		MemBlock->Type = MEMTYPE_UNUSED;	// Document memory block's removal

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemAllotBytes									*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:	Used to allot bytes of memory from the memory pool
	// Input:   A number of bytes to allot
	// Return:	A code indicating the results of the byte allotment

	PRIVATE RETCODE MemAllotBytes (Dword numBytes)
	{
		if (numBytes == 0)	// Check for meaningless allotments
		{
			MESSAGE_BOX("No operation: MemAllotBytes failed","Error");
			return RETCODE_FAILURE;	// Return failure
		}

		if (MemMgr.FreeBytes < numBytes)	// Check whether allotment may be processed
		{
			MESSAGE_BOX("Out of memory: MemAllotBytes failed","Error");
			return RETCODE_FAILURE;	// Return failure
		}

		MemMgr.FreeBytes -= numBytes;	// Decrement count of free bytes

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemCollectBytes									*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:	Used to collect alloted bytes of memory into the memory pool
	// Input:   A number of bytes to collect
	// Return:  A code indicating the results of the byte collection

	PRIVATE RETCODE MemCollectBytes (Dword numBytes)
	{
		if (numBytes == 0)	// Check for meaningless collections
		{
			MESSAGE_BOX("No operation: MemCollectBytes failed","Error");
			return RETCODE_FAILURE;	// Return failure
		}

		if (MemMgr.FreeBytes + numBytes > MemMgr.FrameBytes)	// Check whether collection may be processed
		{
			MESSAGE_BOX("Overflow: MemCollectBytes failed","Error");
			return RETCODE_FAILURE;	// Return failure
		}

		MemMgr.FreeBytes += numBytes;	// Increment count of free bytes

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemExtractUnusedBlock							*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to extract a block from the unused blocks
	// Input:	Pointer to a memory block
	// Return:	A code indicating the results of the extraction

	PRIVATE RETCODE MemExtractUnusedBlock (MEMBLOCK * MemBlock)
	{
		// Verify that valid type of memory is being extracted
		ASSERT_MSG(MemBlock->Type == MEMTYPE_UNUSED,"Invalid type: MemExtractUnusedBlock failed","Error");
		// Verify that unused blocks exist
		ASSERT_MSG(MemMgr.numUnused != 0,"No unused blocks: MemExtractUnusedBlock failed","Error");

		if (MemBlock == MemMgr.Cache)	// Check whether cache is to be extracted
		{
			if (MemMgr.numUnused == BASE_AMOUNT)	// Check whether extraction will empty cache
				MemMgr.Cache = NULL;			// Nullify cache

			else									// Check for other cases
				MemMgr.Cache = MemBlock->tNext;	// Reassign cache
		}

		MemBlock->tPrev->tNext = MemBlock->tNext;	// Remove memory block's typed back reference
		MemBlock->tNext->tPrev = MemBlock->tPrev;	// Remove memory block's typed forward reference
		
		--MemMgr.numUnused;	// Decrease unused block count

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemInsertAmongUnusedBlocks						*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to place a block within the unused blocks
	// Input:	Pointer to a memory block
	// Return:	A code indicating the results of the insertion

	PRIVATE RETCODE MemInsertAmongUnusedBlocks (MEMBLOCK * MemBlock)
	{
		if (MemMgr.numUnused == 0)		// Check whether cache is empty
		{
			MemMgr.Cache = MemBlock;	// If so, load cache

			MemBlock->tPrev = MemMgr.Cache;	// Refer memory block back to cache
			MemBlock->tNext = MemMgr.Cache;	// Refer memory block forward to cache
		}

		else							// Check for other cases
		{
			if (MemAdjoinBlocks (MemBlock) != M_DISJOINT)	// Check whether block may adjoin to memory
				return RETCODE_SUCCESS;	// Return trivial success

			MemBlock->tPrev = MemMgr.Cache;			// Refer memory block back
			MemBlock->tNext = MemMgr.Cache->tNext;	// Refer memory block forward
		}

		MemBlock->tPrev->tNext = MemBlock;	// Refer memory block's previous unused block ahead to memory block
		MemBlock->tNext->tPrev = MemBlock;	// Refer memory block's next unused block back to memory block
						
		++MemMgr.numUnused;// Increment unused block count

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								MemAdjoinBlocks									*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to adjoin memory
	// Input:	Address of pointer to a memory block
	// Return:	Amount of non-adjoinments

	PRIVATE long MemAdjoinBlocks (MEMBLOCK * MemBlock)
	{
		PMEMBLOCK PrevBlock;	// Pointer to previous memory block
		PMEMBLOCK NextBlock;	// Pointer to next memory block

		long UnjoinedEnds = 0;	// Counter for memory block's unjoined ends

		PrevBlock = MemBlock->Prev;	// Retrieve previous block in chain
		NextBlock = MemBlock->Next;	// Retrieve next block in chain

		if (NextBlock->Type == MEMTYPE_UNUSED && NextBlock > MemBlock)	// Check whether memory block may join with upper memory block
		{
			MemBlock->Size += NextBlock->Size + sizeof(MEMBLOCK);	// Accumulate next block's size

			if (MemMgr.numUnused == BASE_AMOUNT)	// Check whether a single node is to be joined to
			{
				MemBlock->tPrev = MemBlock;	// Refer memory block back to self
				MemBlock->tNext = MemBlock;	// Refer memory block forward to self
			}

			else									// Check for other cases
			{
				MemBlock->tPrev = NextBlock->tPrev;	// Refer memory block back to next block's previous typed block	
				MemBlock->tNext = NextBlock->tNext;	// Refer memory block forward to next block's next typed block

				MemBlock->tPrev->tNext = MemBlock;	// Refer memory block's typed previous block ahead to memory block
				MemBlock->tNext->tPrev = MemBlock;	// Refer memory block's typed next block back to memory block
			}
			
			MemBlock->Next		 = NextBlock->Next;	// Link memory block ahead to block beyond next block
			MemBlock->Next->Prev = MemBlock;		// Link next block back to memory block

			MemMgr.Cache = MemBlock;// Restart cache
		}

		else									// Check for other cases
			++UnjoinedEnds;	// Increment joined indicator

		if (PrevBlock->Type == MEMTYPE_UNUSED && PrevBlock < MemBlock)	// Check whether lower memory block may join with memory block
		{
			PrevBlock->Size += MemBlock->Size + sizeof(MEMBLOCK);	// Accumulate memory block's size

			if (UnjoinedEnds == 0)	// Check whether memory block has been joined ahead
			{
				--MemMgr.numUnused;	// Document adjoinment of PrevBlock, MemBlock, and NextBlock

				if (MemMgr.numUnused > BASE_AMOUNT)	// Check whether more than one unused node remains
				{
					MemBlock->tPrev->tNext = MemBlock->tNext;	// Bind memory block's previous block forward
					MemBlock->tNext->tPrev = MemBlock->tPrev;	// Bind memory block's next block back
				}
			}

			PrevBlock->Next		  = MemBlock->Next;	// Link previous block ahead to data block
			PrevBlock->Next->Prev = PrevBlock;		// Link data block back to previous block

			MemMgr.Cache = PrevBlock;	// Restart cache
		}

		else									// Check for other cases
			++UnjoinedEnds;	// Increment joined indicator

		return UnjoinedEnds;	// Return number of unjoined ends
	}