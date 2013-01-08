#ifndef COMMON_H 
#define COMMON_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/
/*#ifndef	_MSC_VER
	// The Borland compiler gives lots of warnings if you redefine the assert
	// macro - I haven't explored how to avoid them so I'll avoid redefining it.
	//yeah whatever, Bruce...
	#define	USEMICROSOFTASSERTDIALOG
#endif
#ifdef	USEMICROSOFTASSERTDIALOG*/
	#include <assert.h>
/*#else
	#undef	assert	// Make sure we've wiped out any previous definitions.
	#ifdef  NDEBUG
		// In release mode, asserts do nothing.
		#define assert(exp)     ((void)0)
	#else
		// In debug mode we need an assert macro and an assert function.
		// _csassert() returns true if the user requests debugging.
		bool _csassert(const char* expr, const char* filename, unsigned int lineno);
		#define assert(exp) \
				do { \
					if (!(exp)) { \
						dprintf("%s(%d): Assertion failed\n%s\n", __FILE__, __LINE__, #exp); \
						if (_csassert(#exp, __FILE__, __LINE__)) \
							DEBUG_BREAK(); \
						} \
				} while (0)
	#endif
#endif*/

#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winuser.h>

// For SelectBitmap(), etc.
#include <windowsx.h>

//STL stuff
#include <iostream>
#include <list>
#include <stack>
#include <vector>
#include <algorithm>

using namespace std;

#pragma warning ( disable : 4630 )
/********************************************************************
*																	*
*							Qualifiers								*
*																	*
********************************************************************/

#define PUBLIC				// Semantic used to indicate publicly exposed methods
#define PROTECTED			// Semantic used to indicate methods shared among implementations
#define PRIVATE		static	// Semantic used to indicate implementation-specific attributes and methods

/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

#define PATH_SIZE	 80
#define MAX_FILES    15
#define MAXCHARBUFF  255
#define DMAXCHARBUFF 512

#define BASE_AMOUNT	1	// Count of elements of base array
#define BASE_EXTENT	1	// Index of element beyond base array

//
// More fun stuff
//
#define MAXPARAM -1
#define MINPARAM -1

#define PI	3.14159

/********************************************************************
*																	*
*							Return codes							*
*																	*
********************************************************************/

#define RETCODE_BREAK	-1	// General-purpose break code
#define RETCODE_FAILURE	 0	// General-purpose failure
#define RETCODE_SUCCESS	 1	// General-purpose success
#define RETCODE_OUTOFBOUND 666 //General-purpose out of bound

#define COMP_LT	-1	// Less than result of compare method
#define COMP_EQ	 0	// Equal result of compare method
#define COMP_GT	 1	// Greater than result of compare method

#define EQ_FALSE 0	// False result of equivalence method
#define EQ_TRUE	 1	// True result of equivalence method

#define SEARCH_NOTFOUND	-1	// Indicates search without a find

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

typedef BYTE  Byte,  * Pbyte;
typedef WORD  Word,  * Pword;
typedef DWORD Dword, * Pdword;

typedef LONGLONG Hyper, * Phyper;

typedef long FLAGS;		// General flags type
typedef long RETCODE;	// Return code type

typedef POINT LOCATION, * PLOCATION;
typedef POINT OFFSET,   * POFFSET;

/********************************************************************
*																	*
*							Graphics Config							*
*																	*
********************************************************************/

///////////////////////////////////////////////////////////////////////
//	_CONFIG: Structure containing information used to format the API //
///////////////////////////////////////////////////////////////////////

typedef struct _CONFIG {
/* General section  */
	HINSTANCE hInst;// Handle to application instance
	HWND hWnd;		// Handle to application window
/* GRAPHICS section */
/* MEMORY section	*/
	Dword PoolSize;	// Size of pool that is allocated to memory
/* SOUND section	*/
} CONFIG, * PCONFIG;

/********************************************************************
*																	*
*							Methods									*
*																	*
********************************************************************/
													
typedef RETCODE (* COMPARE)	(void *, void *);	// Callback function for sorting comparisons 
typedef RETCODE (* EQUIVAL) (void *, void *);	// Callback function for searching comparisons

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define MESSAGE_BOX(Message,Caption) MessageBox (NULL, Message, Caption, MB_OK)	// User messages

#ifdef NDEBUG
#define ASSERT_MSG(Cond,Msg,Caption)
#else // NDEBUG
#define ASSERT_MSG(Cond,Msg,Caption)	\
	if (!(Cond)) {						\
		MESSAGE_BOX((Msg),(Caption));	\
		assert(Cond);					\
	}
#endif // NDEBUG
// Assertion message report

#define ALIGNED(data)	(((int)(data) + sizeof(int) - 1) & ~(sizeof(int) - 1))
// Extract the aligned version of a datum

#define NUM_ELEMENTS(array)	((sizeof array) / (sizeof((array)[0]))) // Get the element count of a static array

#define SETFLAG(data,flag)   (data) |= (flag)	// Set a flag value in a datum
#define CLEARFLAG(data,flag) (data) &= ~(flag)	// Clear a flag value in a datum
#define FLIPFLAG(data,flag)	 (data) ^= (flag)	// Flip a flag value in a datum

#define GETFLAGS(data,mask)	 ((data) & (mask))				// Grab flags from a datum : partial
#define TESTFLAGS(data,mask) (((data) & (mask)) == (mask))	// Grab flags from a datum : total

#define MAX(one,two) (one) > (two) ? (one) : (two)
#define MIN(one,two) (one) < (two) ? (one) : (two)

#define MAKETAG_4(c1,c2,c3,c4) ((Dword)(((Word)(Byte)(c1)) << 24) |	\
									   (((Word)(Byte)(c2)) << 16) |	\
									   (((Word)(Byte)(c3)) << 8)  |	\
									   (((Word)(Byte)(c4))))
// Constructs an integer tag from four characters

//Gets the index of an array as if it was a double array
//Rip-off from the CS170 sliding puzzle assignment, hehehe
#define CELL(grid, row, col, numcols) ((grid)[((row) * (numcols)) + (col)])

#endif // COMMON_H