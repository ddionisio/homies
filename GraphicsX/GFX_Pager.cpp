#include "gdix.h"
#include "gdix_i.h"

//page stuff
#define	NUMPP			32			//number of obj per page
#define GFX_SPOT_ONE	(NUMPP/32)	// Flag indicating spot one on page is used

#define GFX_PREV_SPOT	(NUMPP/32)	// Amount to decrement mask to move to last spot
#define GFX_NEXT_SPOT	(NUMPP/32)	// Amount to increment mask to move to next spot

#define GFX_PAGE_EMPTY	 0	// Flag indicating empty page
#define GFX_PAGE_FULL	~0	// Flag indicating full page

typedef struct _gfxItem {
	void *data;
	LONGLONG counter;
} gfxItem;

typedef struct _gfxPage {
	FLAGS status;		//stuff it
	gfxItem stuff[NUMPP];
} gfxPage;

typedef struct _gfxList {
	DWORD nStuff;				// number of stuff
	int nPages, nMaxPages;	// Count of object pages; maximum possible pages
	gfxPage **Pages;		// Vector of pages of registered objs.
} gfxList;

LONGLONG g_uCounter=1;	//unique counter

gfxList	 g_gfxList={0};

PRIVATE void _GFXPageAllocVector()
{
	gfxPage **newVec;	// Vector of pages to reallocate

	int index;	// Loop variable

	MemAlloc ((void **) &newVec, ++g_gfxList.nMaxPages * sizeof(gfxPage*), 0);

	for (index = 0; index < g_gfxList.nPages; ++index)	// Loop through current vector
		newVec[index] = g_gfxList.Pages[index];	// Copy pointer data

	if (g_gfxList.nPages != 0)	// Check whether pages exist
		MemFree ((void **) &g_gfxList.Pages);	// Release current page vector

	g_gfxList.Pages = newVec;	// Install new page vector
}

PRIVATE gfxPage * _GFXPageAdd()
{
	gfxPage *Page;	// Client page to allocate

	// Allocate memory for new page
	MemAlloc ((void **) &Page, sizeof(gfxPage), 0);

	if (g_gfxList.nPages == g_gfxList.nMaxPages)	// Check whether page vector is full
		_GFXPageAllocVector();	// Reallocate page vector

	g_gfxList.Pages[g_gfxList.nPages++] = Page;	// Install page in page vector

	Page->status = GFX_PAGE_EMPTY;	// Indicate that all page blocks are available

	return Page;// Return newly allocated page
}

PRIVATE void _GFXPageClean(gfxPage *page)
{
	gfxItem *obj = page->stuff, *nextObj;	// Reference to objs on page

	if (page->status == GFX_PAGE_EMPTY) return;	// Trivial success if page is empty

	while(page->status != GFX_PAGE_EMPTY)	// Scan through all objects
	{
		nextObj = obj; nextObj++;
		if (GETFLAGS(page->status,GFX_SPOT_ONE))	// Check whether block is occupied
		{
			// destroy obj?
			obj->counter=0;
			obj->data=0;
		}

		obj=nextObj;

		page->status >>= GFX_PREV_SPOT;	// Update mask for next block
	}
}

PRIVATE void _GFXPageRemoveLast()
{
	gfxPage *page = g_gfxList.Pages[--g_gfxList.nPages];	// Obtain final page from manager

	// Perform cleanup on page
	_GFXPageClean(page);

	// Release page memory
	MemFree ((void **) &page);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void GFXPageClear()
{
	while(g_gfxList.nPages != 0)		// Iterate until all pages are gone
			_GFXPageRemoveLast();	// Remove final page

	if(g_gfxList.nMaxPages != 0)	// Check whether client pages exist
		MemFree((void **) &g_gfxList.Pages);	// Release page vector

	memset(&g_gfxList, 0, sizeof(gfxList));
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void GFXPageAdd(void *item, gfxID *pID)
{
	assert(item);	// Verify that Data points to valid memory

	DWORD pIndex, bIndex;	// Index of client page; index of client block

	gfxPage *page;

	FLAGS Mask = GFX_SPOT_ONE;	// Mask used to test page availability

	if (g_gfxList.nPages * NUMPP == g_gfxList.nStuff)	// Check whether all pages are full
	{
		pIndex = g_gfxList.nPages;	// Get index of new page

		page = _GFXPageAdd();	// Append a new page to the manager
	}

	else	// Pages are available
	{
		for (pIndex = 0; pIndex < g_gfxList.nPages; ++pIndex)	// Loop through all pages
			if (g_gfxList.Pages [pIndex]->status != GFX_PAGE_FULL) break;	// Break if page has openings

		page = g_gfxList.Pages [pIndex];	// Obtain refence to page
	}

	for (bIndex = 0; bIndex < NUMPP; ++bIndex)	// Scan block occupancy flags
	{
		if (!GETFLAGS(page->status,Mask)) break;	// Break if block is open

		Mask <<= GFX_NEXT_SPOT;	// Update mask for next block
	}

	SETFLAG(page->status,Mask);	// Indicate block is in use

	page->stuff[bIndex].data = item;	// set obj ptr

	pID->ID = pIndex * NUMPP + bIndex;	// Obtain obj ID
	page->stuff[bIndex].counter = pID->counter = ++g_uCounter;

	++g_gfxList.nStuff;	// Increment item count
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void GFXPageRemove(gfxID *pID)
{
	gfxPage *page;	//page where obj resides

	int pIndex = pID->ID / NUMPP;	// Page index

	// Ascertain that ID is within the messager's pages
	ASSERT_MSG(pIndex < g_gfxList.nPages,"Invalid ID","GFXPageRemove");

	page = g_gfxList.Pages [pIndex];	// Obtain client page

	pID->ID %= NUMPP;	// Convert client ID to index

	CLEARFLAG(page->status,GFX_SPOT_ONE << pID->ID);	// Mark block as available

	//if(page->status == GFX_PAGE_EMPTY && pIndex + 1 == g_gfxList.nPages)	// Check whether last page is empty
	//	_GFXPageRemoveLast();

	--g_gfxList.nStuff;	// Decrement obj count
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void * GFXPageGet(DWORD ID, LONGLONG *pCounter)
{
	gfxPage *page;	//page where obj resides

	int pIndex = ID / NUMPP;	// Page index
	int bIndex = ID % NUMPP;	// Block index

	if (pIndex >= g_gfxList.nPages) return NULL;	// Trivially fail if client cannot exist

	page = g_gfxList.Pages [pIndex];	// Refer to client page

	if (GETFLAGS(page->status,GFX_SPOT_ONE << bIndex))	// Check whether block is in use
	{
		if(pCounter)
			*pCounter=page->stuff [bIndex].counter;
		return page->stuff [bIndex].data;	// If so, return client data
	}

	return NULL;// Client is unavailable
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void * GFXPageQuery(const gfxID *key)
{
	void *data;
	LONGLONG counter;

	assert(key);// Verify that Key points to valid memory

	data = GFXPageGet(key->ID, &counter);	// Attempt to obtain client

	if (data != NULL && counter == key->counter)	// Check whether key matches
		return data;	// If so, return client

	return NULL;// Client is unavailable
}