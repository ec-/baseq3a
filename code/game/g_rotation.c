// Public Domain

#include "g_local.h"

qboolean G_MapExist( const char *map ) 
{
	fileHandle_t fh;
	int len;

	if ( !map || !*map )
		return qfalse;

	len = trap_FS_FOpenFile( va( "maps/%s.bsp", map ), &fh, FS_READ );

	if ( len < 0 )
		return qfalse;

	trap_FS_FCloseFile( fh );

	return ( len >= 144 ) ? qtrue : qfalse ;
}


void G_LoadMap( const char *map ) 
{
	char cmd[ MAX_CVAR_VALUE_STRING ];
	char ver[ 16 ];
	int version;

	trap_Cvar_VariableStringBuffer( "version", ver, sizeof( ver ) );
	if ( !Q_strncmp( ver, "Q3 1.32 ", 8 ) || !Q_strncmp( ver, "Q3 1.32b ", 9 ) ||
		!Q_strncmp( ver, "Q3 1.32c ", 9 ) ) 
		version = 0; // buggy vanilla binaries
	else
		version = 1;

	if ( !map || !*map || !G_MapExist( map ) || !Q_stricmp( map, g_mapname.string ) ) {
		if ( level.time > 12*60*60*1000 || version == 0 || level.denyMapRestart )
			BG_sprintf( cmd, "map \"%s\"\n", g_mapname.string );
		else
			Q_strcpy( cmd, "map_restart 0\n" );
	} else {
		if ( !G_MapExist( map ) ) // required map doesn't exists, reload existing
			BG_sprintf( cmd, "map \"%s\"\n", g_mapname.string );
		else
			BG_sprintf( cmd, "map \"%s\"\n", map );
	}

	trap_SendConsoleCommand( EXEC_APPEND, cmd );
	level.restarted = qtrue;
}


qboolean ParseMapRotation( void ) 
{
	char buf[ 4096 ];
	char cvar[ 256 ];
	char map[ 256 ];
	char *s;
	fileHandle_t fh;
	int	len;
	char *tk;
	int reqIndex; 
	int curIndex = 0;
	int scopeLevel = 0;

	if ( g_gametype.integer == GT_SINGLE_PLAYER || !g_rotation.string[0] )
		return qfalse;

	len = trap_FS_FOpenFile( g_rotation.string, &fh, FS_READ );
	if ( fh == FS_INVALID_HANDLE ) 
	{
		Com_Printf( S_COLOR_YELLOW "%s: map rotation file doesn't exists.\n", g_rotation.string );
		return qfalse;
	}
	if ( len >= sizeof( buf ) ) 
	{
		Com_Printf( S_COLOR_YELLOW "%s: map rotation file is too big.\n", g_rotation.string );
		len = sizeof( buf ) - 1;
	}
	trap_FS_Read( buf, len, fh );
	buf[ len ] = '\0';
	trap_FS_FCloseFile( fh );
	
	Com_InitSeparators(); // needed for COM_ParseSep()

	reqIndex = trap_Cvar_VariableIntegerValue( SV_ROTATION );
	if ( reqIndex == 0 )
		reqIndex = 1;

__rescan:

	COM_BeginParseSession( g_rotation.string );

	s = buf; // initialize token parsing
	map[0] = '\0';

	while ( 1 ) 
	{
		tk = COM_ParseSep( &s, qtrue );
		if ( tk[0] == '\0' ) 
			break;

		if ( tk[0] == '$' && tk[1] != '\0' ) // cvar name
		{
			 // save cvar name
			strcpy( cvar, tk+1 );
			tk = COM_ParseSep( &s, qfalse );
			// expect '='
			if ( tk[0] == '=' && tk[1] == '\0' ) 
			{
				tk = COM_ParseSep( &s, qtrue );
				if ( !scopeLevel || curIndex == reqIndex ) 
				{
					trap_Cvar_Set( cvar, tk );
				}
				SkipTillSeparators( &s ); 
				continue;
			}
			else 
			{
				COM_ParseWarning( S_COLOR_YELLOW "missing '=' after '%s'", cvar );
				SkipRestOfLine( &s );
				continue;
			}

		}
		else if ( tk[0] == '{' && tk[1] == '\0' ) 
		{
			if ( scopeLevel == 0 && curIndex ) 
			{
				scopeLevel++;
				continue;
			}
			else 
			{
				COM_ParseWarning( S_COLOR_YELLOW "unexpected '{'" );
				continue;
			}
		}
		else if ( tk[0] == '}' && tk[1] == '\0' ) 
		{
			if ( scopeLevel == 1 ) 
			{
				scopeLevel--;
				continue;
			}
			else 
			{
				COM_ParseWarning( S_COLOR_YELLOW "unexpected '}'" );
			}
		}
		else if ( G_MapExist( tk ) )
		{
			curIndex++;
			if ( curIndex == reqIndex ) 
			{
				Q_strncpyz( map, tk, sizeof( map ) );
			}
		}
		else 
		{
			COM_ParseWarning( S_COLOR_YELLOW "map '%s' doesn't exists", tk );
			SkipRestOfLine( &s );
			continue;
		}
	}

	if ( curIndex == 0 ) // no maps in rotation file
	{
		Com_Printf( S_COLOR_YELLOW "%s: no maps in rotation file.\n", g_rotation.string );
		trap_Cvar_Set( SV_ROTATION, "1" );
		return qfalse;
	}

	if ( !map[0] ) // map at required index not found?
	{
		if ( reqIndex > 1 ) // try to rescan with lower index
		{
			Com_Printf( S_COLOR_CYAN "%s: map at index %i not found, rescan\n", g_rotation.integer, reqIndex );
			reqIndex = 1;
			goto __rescan;
		}
		trap_Cvar_Set( SV_ROTATION, "1" );
		return qfalse;
	}

	reqIndex++;
	if ( reqIndex > curIndex )
		reqIndex = 1;

	trap_Cvar_Set( SV_ROTATION, va( "%i", reqIndex ) );
	//trap_Cvar_Set( "g_restarted", "1" );
	G_LoadMap( map );

	return qtrue;
}
