// Copyright (C) 1999-2000 Id Software, Inc.
//
// q_shared.c -- stateless support routines that are included in each code dll
#include "q_shared.h"

float Com_Clamp( float min, float max, float value ) {
	if ( value < min ) {
		return min;
	}
	if ( value > max ) {
		return max;
	}
	return value;
}


/*
============
COM_SkipPath
============
*/
char *COM_SkipPath (char *pathname)
{
	char	*last;
	
	last = pathname;
	while (*pathname)
	{
		if (*pathname=='/')
			last = pathname+1;
		pathname++;
	}
	return last;
}

/*
============
COM_StripExtension
============
*/
void COM_StripExtension( const char *in, char *out, int destsize ) {
	int             length;

	Q_strncpyz(out, in, destsize);

	length = strlen(out)-1;
	while (length > 0 && out[length] != '.')
	{
		length--;
		if (out[length] == '/')
			return;		// no extension
	}
	if ( length )
		out[ length ] = '\0';
}


/*
==================
COM_DefaultExtension
==================
*/
void COM_DefaultExtension (char *path, int maxSize, const char *extension ) {
	char	oldPath[MAX_QPATH];
	char    *src;

//
// if path doesn't have a .EXT, append extension
// (extension should include the .)
//
	src = path + strlen(path) - 1;

	while (*src != '/' && src != path) {
		if ( *src == '.' ) {
			return;                 // it has an extension
		}
		src--;
	}

	Q_strncpyz( oldPath, path, sizeof( oldPath ) );
	Com_sprintf( path, maxSize, "%s%s", oldPath, extension );
}


/*
============================================================================

PARSING

============================================================================
*/

static	char	com_token[MAX_TOKEN_CHARS];
static	char	com_parsename[MAX_TOKEN_CHARS];
static	int		com_lines;
static	int		com_tokenline;
static	int		is_separator[ 256 ];

void COM_BeginParseSession( const char *name )
{
	com_lines = 1;
	com_tokenline = 0;
	Com_sprintf(com_parsename, sizeof(com_parsename), "%s", name);
}


int COM_GetCurrentParseLine( void )
{
	if ( com_tokenline )
	{
		return com_tokenline;
	}

	return com_lines;
}

char *COM_Parse( char **data_p )
{
	return COM_ParseExt( data_p, qtrue );
}


extern int ED_vsprintf( char *buffer, const char *fmt, va_list argptr );


void COM_ParseError( char *format, ... )
{
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	ED_vsprintf (string, format, argptr);
	va_end (argptr);

	Com_Printf("ERROR: %s, line %d: %s\n", com_parsename, com_lines, string);
}


void COM_ParseWarning( char *format, ... )
{
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	ED_vsprintf (string, format, argptr);
	va_end (argptr);

	Com_Printf("WARNING: %s, line %d: %s\n", com_parsename, com_lines, string);
}


/*
==============
COM_Parse

Parse a token out of a string
Will never return NULL, just empty strings

If "allowLineBreaks" is qtrue then an empty
string will be returned if the next token is
a newline.
==============
*/
static char *SkipWhitespace( char *data, qboolean *hasNewLines ) {
	int c;

	while( (c = *data) <= ' ') {
		if( !c ) {
			return NULL;
		}
		if( c == '\n' ) {
			com_lines++;
			*hasNewLines = qtrue;
		}
		data++;
	}

	return data;
}


int COM_Compress( char *data_p ) {
	char *in, *out;
	int c;
	qboolean newline = qfalse, whitespace = qfalse;

	in = out = data_p;
	if (in) {
		while ((c = *in) != 0) {
			// skip double slash comments
			if ( c == '/' && in[1] == '/' ) {
				while (*in && *in != '\n') {
					in++;
				}
			// skip /* */ comments
			} else if ( c == '/' && in[1] == '*' ) {
				while ( *in && ( *in != '*' || in[1] != '/' ) ) 
					in++;
				if ( *in ) 
					in += 2;
                        // record when we hit a newline
                        } else if ( c == '\n' || c == '\r' ) {
                            newline = qtrue;
                            in++;
                        // record when we hit whitespace
                        } else if ( c == ' ' || c == '\t') {
                            whitespace = qtrue;
                            in++;
                        // an actual token
			} else {
                            // if we have a pending newline, emit it (and it counts as whitespace)
                            if (newline) {
                                *out++ = '\n';
                                newline = qfalse;
                                whitespace = qfalse;
                            } else if (whitespace) {
                                *out++ = ' ';
                                whitespace = qfalse;
                            }
                            
                            // copy quoted strings unmolested
                            if (c == '"') {
                                    *out++ = c;
                                    in++;
                                    while (1) {
                                        c = *in;
                                        if (c && c != '"') {
                                            *out++ = c;
                                            in++;
                                        } else {
                                            break;
                                        }
                                    }
                                    if (c == '"') {
                                        *out++ = c;
                                        in++;
                                    }
                            } else {
                                *out = c;
                                out++;
                                in++;
                            }
			}
		}
	}
	*out = 0;
	return out - data_p;
}


char *COM_ParseExt( char **data_p, qboolean allowLineBreaks )
{
	int c = 0, len;
	qboolean hasNewLines = qfalse;
	char *data;

	data = *data_p;
	len = 0;
	com_token[0] = '\0';
	com_tokenline = 0;

	// make sure incoming data is valid
	if ( !data )
	{
		*data_p = NULL;
		return com_token;
	}

	while ( 1 )
	{
		// skip whitespace
		data = SkipWhitespace( data, &hasNewLines );
		if ( !data )
		{
			*data_p = NULL;
			return com_token;
		}
		if ( hasNewLines && !allowLineBreaks )
		{
			*data_p = data;
			return com_token;
		}

		c = *data;

		// skip double slash comments
		if ( c == '/' && data[1] == '/' )
		{
			data += 2;
			while (*data && *data != '\n') {
				data++;
			}
		}
		// skip /* */ comments
		else if ( c == '/' && data[1] == '*' ) 
		{
			data += 2;
			while ( *data && ( *data != '*' || data[1] != '/' ) ) 
			{
				if ( *data == '\n' )
				{
					com_lines++;
				}
				data++;
			}
			if ( *data ) 
			{
				data += 2;
			}
		}
		else
		{
			break;
		}
	}

	com_tokenline = com_lines;

	// handle quoted strings
	if ( c == '"' )
	{
		data++;
		while ( 1 )
		{
			c = *data;
			if ( c == '"' || c == '\0' )
			{
				if ( c == '"' )
					data++;

				com_token[ len ] = '\0';
				*data_p = ( char * ) data;
				return com_token;
			}
			data++;
			if ( c == '\n' )
			{
				com_lines++;
			}
			if ( len < MAX_TOKEN_CHARS-1 )
			{
				com_token[ len ] = c;
				len++;
			}
		}
	}

	// parse a regular word
	do
	{
		if ( len < MAX_TOKEN_CHARS - 1 )
		{
			com_token[ len ] = c;
			len++;
		}
		data++;
		c = *data;
	} while ( c > ' ' );

	com_token[ len ] = '\0';

	*data_p = ( char * ) data;
	return com_token;
}


/*
==================
COM_MatchToken
==================
*/
void COM_MatchToken( char **buf_p, char *match ) {
	char	*token;

	token = COM_Parse( buf_p );
	if ( strcmp( token, match ) ) {
		Com_Error( ERR_DROP, "MatchToken: %s != %s", token, match );
	}
}


/*
=================
SkipBracedSection

The next token should be an open brace.
Skips until a matching close brace is found.
Internal brace depths are properly skipped.
=================
*/
void SkipBracedSection( char **program ) {
	char			*token;
	int				depth;

	depth = 0;
	do {
		token = COM_ParseExt( program, qtrue );
		if( token[1] == 0 ) {
			if( token[0] == '{' ) {
				depth++;
			}
			else if( token[0] == '}' ) {
				depth--;
			}
		}
	} while( depth && *program );
}


/*
=================
SkipRestOfLine
=================
*/
void SkipRestOfLine( char **data ) {
	char	*p;
	int		c;

	p = *data;

	if ( !*p )
		return;

	while ( (c = *p) != '\0' ) {
		p++;
		if ( c == '\n' ) {
			com_lines++;
			break;
		}
	}

	*data = p;
}


void Com_InitSeparators( void )
{
	is_separator['\n']=1;
	is_separator[';']=1;
	is_separator['=']=1;
	is_separator['{']=1;
	is_separator['}']=1;
}


void SkipTillSeparators( char **data )
{
	char	*p;
	int	c;

	p = *data;

	if ( !*p )
		return;

	while ( (c = *p) != '\0' ) 
	{
		p++;
		if ( is_separator[ c ] )
		{
			if ( c == '\n' )
			{
				com_lines++;
			}
			break;
		}
	}

	*data = p;
}


char *COM_ParseSep( char **data_p, qboolean allowLineBreaks )
{
	int c = 0, len;
	qboolean hasNewLines = qfalse;
	char *data;

	data = *data_p;
	len = 0;
	com_token[0] = '\0';
	com_tokenline = 0;

	// make sure incoming data is valid
	if ( !data )
	{
		*data_p = NULL;
		return com_token;
	}

	while ( 1 )
	{
		// skip whitespace
		data = SkipWhitespace( data, &hasNewLines );
		if ( !data )
		{
			*data_p = NULL;
			return com_token;
		}
		if ( hasNewLines && !allowLineBreaks )
		{
			*data_p = data;
			return com_token;
		}

		c = *data;

		// skip double slash comments
		if ( c == '/' && data[1] == '/' )
		{
			data += 2;
			while (*data && *data != '\n') {
				data++;
			}
		}
		// skip /* */ comments
		else if ( c == '/' && data[1] == '*' ) 
		{
			data += 2;
			while ( *data && ( *data != '*' || data[1] != '/' ) ) 
			{
				if ( *data == '\n' )
				{
					com_lines++;
				}
				data++;
			}
			if ( *data ) 
			{
				data += 2;
			}
		}
		else
		{
			break;
		}
	}

	com_tokenline = com_lines;

	// handle quoted strings
	if ( c == '"' )
	{
		data++;
		while ( 1 )
		{
			c = *data;
			if ( c == '"' || c == '\0' )
			{
				if ( c == '"' )
					data++;

				com_token[ len ] = '\0';
				*data_p = ( char * ) data;
				return com_token;
			}
			data++;
			if ( c == '\n' )
			{
				com_lines++;
			}
			if ( len < MAX_TOKEN_CHARS-1 )
			{
				com_token[ len ] = c;
				len++;
			}
		}
	}

	// special case for separators
 	if ( is_separator[ c ]  )  
	{
		com_token[ len ] = c;
		len++;
		data++;
	} 
	else // parse a regular word
	do
	{
		if ( len < MAX_TOKEN_CHARS - 1 )
		{
			com_token[ len ] = c;
			len++;
		}
		data++;
		c = *data;
	} while ( c > ' ' && !is_separator[ c ] );

	com_token[ len ] = '\0';

	*data_p = ( char * ) data;
	return com_token;
}


/*
============
Com_Split
============
*/
int Com_Split( char *in, char **out, int outsz, int delim ) 
{
	int c;
	char **o = out, **end = out + outsz;
	// skip leading spaces
	if ( delim >= ' ' ) {
		while( (c = *in) != '\0' && c <= ' ' ) 
			in++; 
	}
	*out = in; out++;
	while( out < end ) {
		while( (c = *in) != '\0' && c != delim ) 
			in++; 
		*in = '\0';
		if ( !c ) {
			// don't count last null value
			if ( out[-1][0] == '\0' ) 
				out--;
			break;
		}
		in++;
		// skip leading spaces
		if ( delim >= ' ' ) {
			while( (c = *in) != '\0' && c <= ' ' ) 
				in++; 
		}
		*out = in; out++;
	}
	// sanitize last value
	while( (c = *in) != '\0' && c != delim ) 
		in++; 
	*in = '\0';
	c = out - o;
	// set remaining out pointers
	while( out < end ) {
		*out = in; out++;
	}
	return c;
}


void Parse1DMatrix (char **buf_p, int x, float *m) {
	char	*token;
	int		i;

	COM_MatchToken( buf_p, "(" );

	for (i = 0 ; i < x ; i++) {
		token = COM_Parse(buf_p);
		m[i] = atof(token);
	}

	COM_MatchToken( buf_p, ")" );
}

void Parse2DMatrix (char **buf_p, int y, int x, float *m) {
	int		i;

	COM_MatchToken( buf_p, "(" );

	for (i = 0 ; i < y ; i++) {
		Parse1DMatrix (buf_p, x, m + i * x);
	}

	COM_MatchToken( buf_p, ")" );
}

void Parse3DMatrix (char **buf_p, int z, int y, int x, float *m) {
	int		i;

	COM_MatchToken( buf_p, "(" );

	for (i = 0 ; i < z ; i++) {
		Parse2DMatrix (buf_p, y, x, m + i * x*y);
	}

	COM_MatchToken( buf_p, ")" );
}


/*
============================================================================

					LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

int Q_isprint( int c )
{
	if ( c >= 0x20 && c <= 0x7E )
		return ( 1 );
	return ( 0 );
}

int Q_islower( int c )
{
	if (c >= 'a' && c <= 'z')
		return ( 1 );
	return ( 0 );
}


int Q_isupper( int c )
{
	if (c >= 'A' && c <= 'Z')
		return ( 1 );
	return ( 0 );
}


int Q_isalpha( int c )
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return ( 1 );
	return ( 0 );
}


char* Q_strrchr( const char* string, int c )
{
	char cc = c;
	char *s;
	char *sp=(char *)0;

	s = (char*)string;

	while (*s)
	{
		if (*s == cc)
			sp = s;
		s++;
	}
	if (cc == 0)
		sp = s;

	return sp;
}


/*
=============
Q_strncpyz
 
Safe strncpy that ensures a trailing zero
=============
*/
void Q_strncpyz( char *dest, const char *src, int destsize ) 
{
	if ( !dest ) {
		Com_Error( ERR_FATAL, "Q_strncpyz: NULL dest" );
	}

	if ( !src ) {
		Com_Error( ERR_FATAL, "Q_strncpyz: NULL src" );
	}

	if ( destsize < 1 ) {
		Com_Error(ERR_FATAL,"Q_strncpyz: destsize < 1" ); 
	}

	strncpy( dest, src, destsize-1 );
	dest[ destsize-1 ] = '\0';
}

                 
int Q_stricmpn( const char *s1, const char *s2, int n ) {
	int	c1, c2;

	if ( s1 == NULL ) {
		if ( s2 == NULL )
			return 0;
		else
			return -1;
	} else if ( s2 == NULL )
		return 1;
	
	do {
		c1 = *s1; s1++;
		c2 = *s2; s2++;

		if ( !n-- ) {
			return 0; // strings are equal until end point
		}
		
		if ( c1 != c2 ) {
			if (c1 >= 'a' && c1 <= 'z') {
				c1 -= ('a' - 'A');
			}
			if (c2 >= 'a' && c2 <= 'z') {
				c2 -= ('a' - 'A');
			}
			if (c1 != c2) {
				return c1 < c2 ? -1 : 1;
			}
		}
	} while (c1);
	
	return 0;		// strings are equal
}


int Q_strncmp( const char *s1, const char *s2, int n ) {
	int		c1, c2;
	
	do {
		c1 = *s1; s1++;
		c2 = *s2; s2++;

		if (!n--) {
			return 0;		// strings are equal until end point
		}
		
		if (c1 != c2) {
			return c1 < c2 ? -1 : 1;
		}
	} while (c1);
	
	return 0;		// strings are equal
}


int Q_stricmp( const char *s1, const char *s2 ) 
{
	unsigned char c1, c2;

	if ( s1 == NULL ) 
	{
		if ( s2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( s2 == NULL )
		return 1;
	
	do 
	{
		c1 = *s1; s1++;
		c2 = *s2; s2++;

		if ( c1 != c2 ) 
		{
			if ( c1 <= 'Z' && c1 >= 'A' )
				c1 += ('a' - 'A');

			if ( c2 <= 'Z' && c2 >= 'A' )
				c2 += ('a' - 'A');

			if ( c1 != c2 ) 
				return c1 < c2 ? -1 : 1;
		}
	}
	while ( c1 != '\0' );

	return 0;
}


char *Q_strlwr( char *s1 ) 
{
    char	*s;

    s = s1;
	while ( *s ) {
		*s = tolower(*s);
		s++;
	}
    return s1;
}


char *Q_strupr( char *s1 ) {
    char	*s;

    s = s1;
	while ( *s ) {
		*s = toupper(*s);
		s++;
	}
    return s1;
}


// never goes past bounds or leaves without a terminating 0
void Q_strcat( char *dest, int size, const char *src ) {
	int		l1;

	l1 = strlen( dest );
	if ( l1 >= size ) {
		Com_Error( ERR_FATAL, "Q_strcat: already overflowed" );
	}
	Q_strncpyz( dest + l1, src, size - l1 );
}


int Q_PrintStrlen( const char *string ) {
	int			len;
	const char	*p;

	if( !string ) {
		return 0;
	}

	len = 0;
	p = string;
	while( *p ) {
		if( Q_IsColorString( p ) ) {
			p += 2;
			continue;
		}
		p++;
		len++;
	}

	return len;
}


char *Q_CleanStr( char *string ) {
	char*	d;
	char*	s;
	int		c;

	s = string;
	d = string;
	while ((c = *s) != 0 ) {
		if ( Q_IsColorString( s ) ) {
			s++;
		}		
		else if ( c >= ' ' && c <= '~' ) {
			*d = c; d++;
		}
		s++;
	}
	*d = '\0';

	return string;
}


void QDECL Com_sprintf( char *dest, int size, const char *fmt, ... ) {
	va_list		argptr;
	int		len;

	va_start( argptr, fmt );
	len = ED_vsprintf( dest, fmt, argptr );
	va_end( argptr );

	if ( len >= size ) {
		Com_Error( ERR_FATAL, "Com_sprintf: overflow of %i in %i\n", len, size );
	}
}


/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char * QDECL va( const char *format, ... ) 
{
	va_list		argptr;
	static char		string[2][32000];	// in case va is called by nested functions
	static int		index = 0;
	char	*buf;

	buf = string[ index ];
	index ^= 1;

	va_start( argptr, format );
	ED_vsprintf( buf, format, argptr );
	va_end( argptr );

	return buf;
}


/*
=====================================================================

  INFO STRINGS

=====================================================================
*/

/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
FIXME: overflow check?
===============
*/
char *Info_ValueForKey( const char *s, const char *key ) {
	char	pkey[BIG_INFO_KEY];
	static	char value[2][BIG_INFO_VALUE];	// use two buffers so compares
											// work without stomping on each other
	static	int	valueindex = 0;
	char	*o;
	
	if ( !s || !key ) {
		return "";
	}

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_ValueForKey: oversize infostring" );
	}

	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return "";
			*o = *s;
			o++; s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			*o = *s;
			o++; s++;
		}
		*o = 0;

		if (!Q_stricmp (key, pkey) )
			return value[valueindex];

		if (!*s)
			break;
		s++;
	}

	return "";
}


/*
===================
Info_NextPair

Used to itterate through all the key/value pairs in an info string
===================
*/
void Info_NextPair( const char **head, char *key, char *value ) {
	char	*o;
	const char	*s;

	s = *head;

	if ( *s == '\\' ) {
		s++;
	}
	key[0] = 0;
	value[0] = 0;

	o = key;
	while ( *s != '\\' ) {
		if ( !*s ) {
			*o = 0;
			*head = s;
			return;
		}
		*o++ = *s++;
	}
	*o = 0;
	s++;

	o = value;
	while ( *s != '\\' && *s ) {
		*o++ = *s++;
	}
	*o = 0;

	*head = s;
}


/*
===================
Info_RemoveKey
===================
*/
void Info_RemoveKey( char *s, const char *key ) {
	char	*start;
	char	pkey[MAX_INFO_KEY];
	char	value[MAX_INFO_VALUE];
	char	*o;

	if ( strlen( s ) >= MAX_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_RemoveKey: oversize infostring" );
	}

	if (strchr (key, '\\')) {
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			memmove(start, s, strlen(s) + 1); // remove this part
			
			return;
		}

		if (!*s)
			return;
	}

}

/*
===================
Info_RemoveKey_Big
===================
*/
void Info_RemoveKey_Big( char *s, const char *key ) {
	char	*start;
	char	pkey[BIG_INFO_KEY];
	char	value[BIG_INFO_VALUE];
	char	*o;

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_RemoveKey_Big: oversize infostring" );
	}

	if (strchr (key, '\\')) {
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			memmove(start, s, strlen(s) + 1); // remove this part
			return;
		}

		if (!*s)
			return;
	}

}


/*
==================
Info_Validate

Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
qboolean Info_Validate( const char *s ) {
	if ( strchr( s, '"' ) ) {
		return qfalse;
	}
	if ( strchr( s, ';' ) ) {
		return qfalse;
	}
	return qtrue;
}


/*
==================
Info_SetValueForKey

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey( char *s, const char *key, const char *value ) {
	char	newi[MAX_INFO_STRING];

	if ( strlen( s ) >= MAX_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring" );
	}

	if (strchr (key, '\\') || strchr (value, '\\'))
	{
		Com_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr (key, ';') || strchr (value, ';'))
	{
		Com_Printf ("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr (key, '"') || strchr (value, '"'))
	{
		Com_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey (s, key);
	if (!value || !strlen(value))
		return;

	Com_sprintf (newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) >= MAX_INFO_STRING)
	{
		Com_Printf ("Info string length exceeded\n");
		return;
	}

	strcat (newi, s);
	strcpy (s, newi);
}

/*
==================
Info_SetValueForKey_Big

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey_Big( char *s, const char *key, const char *value ) {
	char	newi[BIG_INFO_STRING];

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring" );
	}

	if (strchr (key, '\\') || strchr (value, '\\'))
	{
		Com_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr (key, ';') || strchr (value, ';'))
	{
		Com_Printf ("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr (key, '"') || strchr (value, '"'))
	{
		Com_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey_Big (s, key);
	if ( !value || !*value )
		return;

	Com_sprintf (newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) >= BIG_INFO_STRING)
	{
		Com_Printf ("BIG Info string length exceeded\n");
		return;
	}

	strcat (s, newi);
}
