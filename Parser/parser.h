#ifndef _parser_h
#define _parser_h

#include "..\common.h"

typedef enum {
	RETCODE_ENDREACHED = 2,
	RETCODE_NOTFOUND
} eParserRetype;

/**********************************************************
;
;	Name:		ParserReadStringFile
;
;	Purpose:	reads a string to a given buffer until it
;				reaches the stopat char.  Will also return
;				if it reaches a newline
;
;	Input:		the file, buff to fill, size of buff,
;				the stopat char
;
;	Output:		the buff is filled
;
;	Return:		RETCODE_FAILURE if reached end of file...
;				RETCODE_ENDREACHED if stop at reached
;
**********************************************************/
PROTECTED RETCODE ParserReadStringFile(FILE *fp, char *buff, int buffSize, 
									   const char beginat, const char stopat);

/**********************************************************
;
;	Name:		ParserCountStringFile
;
;	Purpose:	reads a string to a given buffer until it
;				reaches the stopat char.  Will also return
;				if it reaches a newline.  This will set the
;				file back to it's original position before
;				returning.
;
;	Input:		the file, numChar to fill number of letters,
;				the stopat char
;
;	Output:		numChar is filled
;
;	Return:		RETCODE_FAILURE if reached end of file...
;				RETCODE_ENDREACHED if stop at reached
;
**********************************************************/
PROTECTED RETCODE ParserCountStringFile(FILE *fp, int *numChar, 
									   const char beginat, const char stopat);

/**********************************************************
;
;	Name:		ParserReadWordFile
;
;	Purpose:	finds a word until it sees a ' ', '\n' or
;				the given char stopat...or if end of file.
;				This function will ensure that the fp will
;				point after the stop at
;
;	Input:		the fp, buff to fill, size of buff, 
;				end char(optional)
;
;	Output:		the buff is filled
;
;	Return:		the char that the ReadWord reached after getting
;				the word...or the stopat.  If it is zero, that
;				means the end of file
;
**********************************************************/
PROTECTED char ParserReadWordFile(FILE *fp, char *buff, int buffSize, const char stopat);

/**********************************************************
;
;	Name:		ParserCountWordFile
;
;	Purpose:	finds a word until it sees a ' ', '\n' or
;				the given char stopat...or if end of file.
;				This function will set the fp back to it's
;				original position after return.
;
;	Input:		the fp, the numChar to fill, 
;				end char(optional)
;
;	Output:		the numChar is filled with number of letters
;
;	Return:		the char that the ReadWord reached after getting
;				the word...or the stopat.  If it is zero, that
;				means the end of file
;
**********************************************************/
PROTECTED char ParserCountWordFile(FILE *fp, int *numChar, const char stopat);

/**********************************************************
;
;	Name:		ParserSkipCharFile
;
;	Purpose:	This will just set the fp after the given char,
;				if it finds it...
;				If it finds something else, then it returns Failure
;
;	Input:		FILE fp, const char thing
;
;	Output:		fp will point at the end of the char
;
;	Return:		SUCCESS if we found the 'thing', FAILURE otherwise...
;
**********************************************************/
PROTECTED RETCODE ParserSkipCharFile(FILE *fp, const char thing, bool returnFalseOnOthers=false);

typedef enum {
	dataINT,
	dataSHORT,
	dataFLOAT
} eDataType;

/**********************************************************
;
;	Name:		ParserReadDataFile
;
;	Purpose:	reads data from the given file and
;				stuff it inside the dest
;
;	Input:		the file, int to fill, and stop at
;
;	Output:		fp points after stop at and dest is filled
;
;	Return:		SUCCESS if we reached stopAt, FAILURE otherwise...
;
**********************************************************/
PROTECTED RETCODE ParserReadDataFile(FILE *fp, void *data, eDataType dType, const char stopAt);

/**********************************************************
;
;	Name:		
;
;	Purpose:	
;
;	Input:		
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC bool ParserSkipString(FILE *fp, const char beginAt, const char stopAt, const char *string);

//
// string buffer version
//

/**********************************************************
;
;	Name:		ParserReadStringBuff
;
;	Purpose:	reads a string to a given buffer until it
;				reaches the stopat char.  Will also return
;				if it reaches a newline
;
;	Input:		the buff, buff to fill, size of buff,
;				the stopat char
;
;	Output:		the buff is filled
;
;	Return:		RETCODE_FAILURE if reached end of file...
;				RETCODE_ENDREACHED if stop at reached
;
**********************************************************/
PROTECTED RETCODE ParserReadStringBuff(char **theBuff, char *buff, int buffSize, 
									   const char beginat, const char stopat);

/**********************************************************
;
;	Name:		ParserCountStringBuff
;
;	Purpose:	reads a string to a given buffer until it
;				reaches the stopat char.  Will also return
;				if it reaches a newline
;
;	Input:		the buff, numChar to fill number of letters,
;				the stopat char
;
;	Output:		the numChar is filled
;
;	Return:		RETCODE_FAILURE if reached end of file...
;				RETCODE_ENDREACHED if stop at reached
;
**********************************************************/
PROTECTED RETCODE ParserCountStringBuff(const char *theBuff, int *numChar, 
									   const char beginat, const char stopat);

/**********************************************************
;
;	Name:		ParserReadWordBuff
;
;	Purpose:	finds a word until it sees a ' ', '\n' or
;				the given char stopat...or if end of file.
;				This function will ensure that the fp will
;				point after the stop at
;
;	Input:		the buff to read, buff to fill, size of buff, 
;				end char(optional)
;
;	Output:		the buff is filled
;
;	Return:		the char that the ReadWord reached after getting
;				the word...or the stopat.  If it is zero, that
;				means the end of file
;
**********************************************************/
PROTECTED char ParserReadWordBuff(char **theBuff, char *buff, int buffSize, const char stopat);

/**********************************************************
;
;	Name:		ParserCountWordBuff
;
;	Purpose:	finds a word until it sees a ' ', '\n' or
;				the given char stopat...or if end of file.
;				This function will ensure that the fp will
;				point after the stop at
;
;	Input:		the buff to read, buff to fill, size of buff, 
;				end char(optional)
;
;	Output:		numChar is filled with number of letters
;
;	Return:		the char that the ReadWord reached after getting
;				the word...or the stopat.  If it is zero, that
;				means the end of file
;
**********************************************************/
PROTECTED char ParserCountWordBuff(const char *theBuff, int *numChar, const char stopat);

/**********************************************************
;
;	Name:		ParserSkipCharBuff
;
;	Purpose:	This will just set the fp after the given char,
;				if it finds it...
;				If it finds something else, then it returns Failure
;
;	Input:		char **theBuff, const char thing
;
;	Output:		theBuff will point at the end of the char
;
;	Return:		SUCCESS if we found the 'thing', FAILURE otherwise...
;
**********************************************************/
PROTECTED RETCODE ParserSkipCharBuff(char **theBuff, const char thing, bool returnFalseOnOthers=false);

// This function returns the file portion of the filename.  If there is
// no filename (ie; if PathName is an empty string, or if PathName appears
// to name a directory (ie; final character is a backslash)) then it returns
// a pointer to the null character at the end of the filename (ie; a null
// string).  If there are no backslashes then it is assumed that the entire
// name represents a filename, so the entire string is returned. This function
// will never return zero. You can remove the filename from any path by
// writing zero to the char pointed at by the return value.

PUBLIC char *GetFilePart(const char *PathName);

// This function returns the file extension, pointing at the period, if there
// is one.  If there is no file extension, it points to the null character at
// the end of the filename (ie; a null string).

PUBLIC char *GetExtension(const char *PathName);

#endif