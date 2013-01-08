#include "parser.h"
#include "i_parser.h"

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
									   const char beginat, const char stopat)
{
	assert(fp);
	assert(buff);

	memset(buff, 0, sizeof(char)*buffSize);

	char ch;
	int i = 0;

	//get the begin at, if the user specified it
	if(beginat)
	{
		while(1)
		{
			if(feof(fp))				//that means we have reached the end of file
				return RETCODE_FAILURE;

			ch = fgetc(fp);

			//if(ch==0)
			//	return RETCODE_FAILURE;

			if(ch == beginat) //if we found our start, break
				break;
			else if(ch == stopat)		//that means there is nothing to get
				return RETCODE_ENDREACHED;
		}
	}

	while(i < buffSize)
	{
		if(feof(fp))				//that means we have reached the end of file
				return RETCODE_FAILURE;

		ch = fgetc(fp);

		//if(ch==0)
		//		return RETCODE_FAILURE;

		if(ch == stopat)
			return RETCODE_ENDREACHED; //we have reached the stopat.
		else
		{
			//otherwise, copy it to buff
			if(i < buffSize-1)
				buff[i] = ch;
			i++;
		}
	}

	return RETCODE_NOTFOUND;
}

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
									   const char beginat, const char stopat)
{
	assert(fp);
	assert(numChar);

	*numChar = 0;

	fpos_t fPos;

	fgetpos(fp, &fPos);

	char ch;

	//get the begin at, if the user specified it
	if(beginat)
	{
		while(1)
		{
			if(feof(fp))				//that means we have reached the end of file
			{ fsetpos(fp, &fPos); return RETCODE_FAILURE; }

			ch = fgetc(fp);

			//if(ch==0)
			//	return RETCODE_FAILURE;

			if(ch == beginat) //if we found our start, break
				break;
			else if(ch == stopat)		//that means there is nothing to get
			{ fsetpos(fp, &fPos); return RETCODE_ENDREACHED; }
		}
	}

	while(1)
	{
		if(feof(fp))				//that means we have reached the end of file
		{ fsetpos(fp, &fPos); return RETCODE_FAILURE; }

		ch = fgetc(fp);

		//if(ch==0)
		//		return RETCODE_FAILURE;

		if(ch == stopat)
		{ fsetpos(fp, &fPos); return RETCODE_ENDREACHED; } //we have reached the stopat.
		else
			*numChar += 1;
	}

	fsetpos(fp, &fPos);
	return RETCODE_NOTFOUND;
}

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
PROTECTED char ParserReadWordFile(FILE *fp, char *buff, int buffSize, const char stopat)
{
	assert(fp);
	assert(buff);

	memset(buff, 0, sizeof(char)*buffSize);

	char ch;

	//get the first letter
	while(1)
	{
		if(feof(fp))				//that means we have reached the end of file
			return 0;

		ch = fgetc(fp);

		if(ch != ' ' && ch != '\n') //if not any of these, we found our start
		{ buff[0] = ch; break; }
		else if((stopat != ' ' && stopat != '\n') && ch == stopat)		//that means there is nothing to get
			return stopat;
	}

	//now fill in the rest of the word
	int i = 1;

	while(1)
	{
		if(feof(fp) || i >= buffSize)//that means we have reached the end of file or buff size
			return 0;

		ch = fgetc(fp);

		if(ch == stopat)
			return stopat;          //we have reached the stopat.  This is what we want
		else if(ch == '\n')
			break;					//we have reached the new line, now we must find the stopat
		else if(ch == ' ')
			break;					//we have reached a space, now we must find the stopat
		else
		{
			//otherwise, copy it to buff
			buff[i] = ch;
			i++;
		}
	}

	//if we get here, that means we must find the stopat or if we find something else
	//other than '\n' or ' ', return that char instead
	//find it only if the user specified
	if(stopat)
	{
		while(!feof(fp))
		{
			ch = fgetc(fp);

			if(ch == stopat)
				return stopat;				 //we have reached the stopat.  This is what we want
			else if(ch != ' ' && ch != '\n')
				return ch;					 //if not ' ' or '\n', we found something else
		}
	}

	return 0; //we have reached the end of file on the last loop
}

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
PROTECTED char ParserCountWordFile(FILE *fp, int *numChar, const char stopat)
{
	assert(fp);
	assert(numChar);

	*numChar = 0;

	fpos_t fPos;

	fgetpos(fp, &fPos);

	char ch;

	//get the first letter
	while(1)
	{
		if(feof(fp))				//that means we have reached the end of file
		{ fsetpos(fp, &fPos); return 0; }

		ch = fgetc(fp);

		if(ch != ' ' && ch != '\n') //if not any of these, we found our start
		{ *numChar += 1; break; }
		else if((stopat != ' ' && stopat != '\n') && ch == stopat)		//that means there is nothing to get
		{ fsetpos(fp, &fPos); return stopat; }
	}

	//now fill in the rest of the word
	while(1)
	{
		if(feof(fp))//that means we have reached the end of file
		{ fsetpos(fp, &fPos); return 0; }

		ch = fgetc(fp);

		if(ch == stopat)
		{ fsetpos(fp, &fPos); return stopat; }          //we have reached the stopat.  This is what we want
		else if(ch == '\n')
			break;					//we have reached the new line, now we must find the stopat
		else if(ch == ' ')
			break;					//we have reached a space, now we must find the stopat
		else
		{
			*numChar += 1;
		}
	}

	//if we get here, that means we must find the stopat or if we find something else
	//other than '\n' or ' ', return that char instead
	//find it only if the user specified
	if(stopat)
	{
		while(!feof(fp))
		{
			ch = fgetc(fp);

			if(ch == stopat)
			{ fsetpos(fp, &fPos); return stopat; }				 //we have reached the stopat.  This is what we want
			else if(ch != ' ' && ch != '\n')
			{ fsetpos(fp, &fPos); return ch; }					 //if not ' ' or '\n', we found something else
		}
	}

	fsetpos(fp, &fPos);
	return 0; //we have reached the end of file on the last loop
}

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
PROTECTED RETCODE ParserSkipCharFile(FILE *fp, const char thing, bool returnFalseOnOthers)
{
	if(thing)
	{
		char ch;
		//find the thing
		while(1)
		{
			if(feof(fp)) //that means we have reached the end of file :(
				return RETCODE_FAILURE;

			ch = fgetc(fp);
			
			if(ch == thing)
				break; //we are good to go...
			else if(returnFalseOnOthers && (ch != '\n' && ch != ' ')) //that means we found something else...which is bad...
				return RETCODE_FAILURE;
		}
	}

	return RETCODE_SUCCESS;
}


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
PROTECTED RETCODE ParserReadDataFile(FILE *fp, void *dest, eDataType dType, const char stopAt)
{
	char buff[MAXCHARBUFF];
	char retChar;

	retChar = ParserReadWordFile(fp, buff, MAXCHARBUFF, stopAt);

	//if retChar is not equal to stopAt display error
	if(retChar != stopAt)
		return RETCODE_FAILURE;

	switch(dType)
	{
	case dataINT:
		{
			int i;
			sscanf(buff, "%d", &i); //this better be right
			memcpy(dest, &i, sizeof(int));
		}
		break;
	case dataSHORT:
		{
			short i;
			sscanf(buff, "%hd", &i); //this better be right
			memcpy(dest, &i, sizeof(short));
		}
		break;
	case dataFLOAT:
		{
			float i;
			sscanf(buff, "%f", &i); //this better be right
			memcpy(dest, &i, sizeof(float));
		}
		break;
	}

	//
	//if(fread(data, size, numcount, fp) != numcount)
	//{ assert(!"error reading data in ParserReadDataFile"); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;//ParserSkipCharFile(fp, stopAt, true);
}

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
PUBLIC bool ParserSkipString(FILE *fp, const char beginAt, const char stopAt, const char *string)
{
	char buff[MAXCHARBUFF];

	while(1)
	{
		if(ParserReadStringFile(fp, buff, MAXCHARBUFF, beginAt, stopAt) == RETCODE_FAILURE) //we have reached end of file
		{ return false; }

		if(stricmp(buff, string) == 0) //we are there!
			return true;
	}

	return false;
}

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
									   const char beginat, const char stopat)
{
	assert(theBuff);
	assert(buff);

	memset(buff, 0, sizeof(char)*buffSize);

	char ch;
	int i = 0;

	//get the begin at, if the user specified it
	if(beginat)
	{
		while(1)
		{
			if(**theBuff == 0)				//that means we have reached the end of theBuff
				return RETCODE_FAILURE;

			ch = **theBuff;
			*theBuff += 1;

			//if(ch==0)
			//	return RETCODE_FAILURE;

			if(ch == beginat) //if we found our start, break
				break;
			else if(ch == stopat)		//that means there is nothing to get
				return RETCODE_ENDREACHED;
		}
	}

	while(i < buffSize)
	{
		if(**theBuff == 0)				//that means we have reached the end of theBuff
				return RETCODE_FAILURE;

		ch = **theBuff;
		*theBuff += 1;

		//if(ch==0)
		//		return RETCODE_FAILURE;

		if(ch == stopat)
			return RETCODE_ENDREACHED; //we have reached the stopat.
		else
		{
			//otherwise, copy it to buff
			if(i < buffSize-1)
				buff[i] = ch;
			i++;
		}
	}

	return RETCODE_NOTFOUND;
}

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
									   const char beginat, const char stopat)
{
	assert(theBuff);
	assert(numChar);

	*numChar = 0;

	char ch;

	//get the begin at, if the user specified it
	if(beginat)
	{
		while(1)
		{
			if(*theBuff == 0)				//that means we have reached the end of theBuff
				return RETCODE_FAILURE;

			ch = *theBuff;
			theBuff++;

			//if(ch==0)
			//	return RETCODE_FAILURE;

			if(ch == beginat) //if we found our start, break
				break;
			else if(ch == stopat)		//that means there is nothing to get
				return RETCODE_ENDREACHED;
		}
	}

	while(1)
	{
		if(*theBuff == 0)				//that means we have reached the end of theBuff
				return RETCODE_FAILURE;

		ch = *theBuff;
		theBuff++;

		//if(ch==0)
		//		return RETCODE_FAILURE;

		if(ch == stopat)
			return RETCODE_ENDREACHED; //we have reached the stopat.
		else
		{
			*numChar += 1;
		}
	}

	return RETCODE_NOTFOUND;
}

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
PROTECTED char ParserReadWordBuff(char **theBuff, char *buff, int buffSize, const char stopat)
{
	assert(theBuff);
	assert(buff);

	memset(buff, 0, sizeof(char)*buffSize);

	char ch;

	//get the first letter
	while(1)
	{
		if(**theBuff == 0)				//that means we have reached the end of theBuff
			return 0;

		ch = **theBuff;
		*theBuff += 1;

		if(ch != ' ' && ch != '\n') //if not any of these, we found our start
		{ buff[0] = ch; break; }
		else if((stopat != ' ' && stopat != '\n') && ch == stopat)		//that means there is nothing to get
			return stopat;
	}

	//now fill in the rest of the word
	int i = 1;

	while(1)
	{
		if(**theBuff == 0 || i >= buffSize)//that means we have reached the end of theBuff or buff size
			return 0;

		ch = **theBuff;
		*theBuff += 1;

		if(ch == stopat)
			return stopat;          //we have reached the stopat.  This is what we want
		else if(ch == '\n')
			break;					//we have reached the new line, now we must find the stopat
		else if(ch == ' ')
			break;					//we have reached a space, now we must find the stopat
		else
		{
			//otherwise, copy it to buff
			buff[i] = ch;
			i++;
		}
	}

	//if we get here, that means we must find the stopat or if we find something else
	//other than '\n' or ' ', return that char instead
	//find it only if the user specified
	if(stopat)
	{
		while(**theBuff != 0)
		{
			ch = **theBuff;
			*theBuff += 1;

			if(ch == stopat)
				return stopat;				 //we have reached the stopat.  This is what we want
			else if(ch != ' ' && ch != '\n')
				return ch;					 //if not ' ' or '\n', we found something else
		}
	}

	return 0; //we have reached the end of file on the last loop
}

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
PROTECTED char ParserCountWordBuff(const char *theBuff, int *numChar, const char stopat)
{
	assert(theBuff);
	assert(numChar);

	*numChar = 0;

	char ch;

	//get the first letter
	while(1)
	{
		if(*theBuff == 0)				//that means we have reached the end of theBuff
			return 0;

		ch = *theBuff;
		theBuff++;

		if(ch != ' ' && ch != '\n') //if not any of these, we found our start
		{ *numChar += 1; break; }
		else if((stopat != ' ' && stopat != '\n') && ch == stopat)		//that means there is nothing to get
			return stopat;
	}

	//now fill in the rest of the word

	while(1)
	{
		if(*theBuff == 0)//that means we have reached the end of theBuff
			return 0;

		ch = *theBuff;
		theBuff++;

		if(ch == stopat)
			return stopat;          //we have reached the stopat.  This is what we want
		else if(ch == '\n')
			break;					//we have reached the new line, now we must find the stopat
		else if(ch == ' ')
			break;					//we have reached a space, now we must find the stopat
		else
		{
			*numChar += 1;
		}
	}

	//if we get here, that means we must find the stopat or if we find something else
	//other than '\n' or ' ', return that char instead
	//find it only if the user specified
	if(stopat)
	{
		while(*theBuff != 0)
		{
			ch = *theBuff;
			theBuff++;

			if(ch == stopat)
				return stopat;				 //we have reached the stopat.  This is what we want
			else if(ch != ' ' && ch != '\n')
				return ch;					 //if not ' ' or '\n', we found something else
		}
	}

	return 0; //we have reached the end of file on the last loop
}

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
PROTECTED RETCODE ParserSkipCharBuff(char **theBuff, const char thing, bool returnFalseOnOthers)
{
	if(thing)
	{
		char ch;
		//find the thing
		while(1)
		{
			if(**theBuff == 0) //that means we have reached the end of theBuff :(
				return RETCODE_FAILURE;

			ch = **theBuff;
			*theBuff += 1;
			
			if(ch == thing)
				break; //we are good to go...
			else if(returnFalseOnOthers && (ch != '\n' && ch != ' ')) //that means we found something else...which is bad...
				return RETCODE_FAILURE;
		}
	}

	return RETCODE_SUCCESS;
}

// This function returns the file portion of the filename.  If there is
// no filename (ie; if PathName is an empty string, or if PathName appears
// to name a directory (ie; final character is a backslash)) then it returns
// a pointer to the null character at the end of the filename (ie; a null
// string).  If there are no backslashes then it is assumed that the entire
// name represents a filename, so the entire string is returned. This function
// will never return zero. You can remove the filename from any path by
// writing zero to the char pointed at by the return value.

PUBLIC char *GetFilePart(const char *PathName)
{
	assert(PathName);
	char* LastSlash = (char *)strrchr(PathName, '\\');
	if (LastSlash)
		return LastSlash + 1;
	return (char *)PathName;
}

// This function returns the file extension, pointing at the period, if there
// is one.  If there is no file extension, it points to the null character at
// the end of the filename (ie; a null string).

PUBLIC char *GetExtension(const char *PathName)
{
	assert(PathName);
	char* FilePart = GetFilePart(PathName);
	assert(FilePart);
	char* LastPeriod = strrchr(FilePart, '.');
	if (LastPeriod)
		return LastPeriod;
	return FilePart + strlen(FilePart);
}
