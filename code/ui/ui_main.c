// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

USER INTERFACE MAIN

=======================================================================
*/


#include "ui_local.h"


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .qvm file
================
*/
DLLEXPORT intptr_t vmMain( int command, int arg0, int arg1 ) {
	switch ( command ) {
	case UI_GETAPIVERSION:
		return UI_API_VERSION;

	case UI_INIT:
		UI_Init();
		return 0;

	case UI_SHUTDOWN:
		UI_Shutdown();
		return 0;

	case UI_KEY_EVENT:
		UI_KeyEvent( arg0, arg1 );
		return 0;

	case UI_MOUSE_EVENT:
		UI_MouseEvent( arg0, arg1 );
		return 0;

	case UI_REFRESH:
		UI_Refresh( arg0 );
		return 0;

	case UI_IS_FULLSCREEN:
		return UI_IsFullscreen();

	case UI_SET_ACTIVE_MENU:
		UI_SetActiveMenu( arg0 );
		return 0;

	case UI_CONSOLE_COMMAND:
		return UI_ConsoleCommand( arg0 );

	case UI_DRAW_CONNECT_SCREEN:
		UI_DrawConnectScreen( arg0 );
		return 0;

	case UI_HASUNIQUECDKEY:	// mod authors need to observe this
		return qtrue;		// change this to qfalse for mods!
	}

	return -1;
}


/*
================
cvars
================
*/

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
} cvarTable_t;

#define DECLARE_UI_CVAR
	#include "ui_cvar.h"
#undef DECLARE_UI_CVAR

// bk001129 - made static to avoid aliasing.
static cvarTable_t		cvarTable[] = {

#define UI_CVAR_LIST
	#include "ui_cvar.h"
#undef UI_CVAR_LIST

};

// bk001129 - made static to avoid aliasing
static int cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);


/*
=================
UI_RegisterCvars
=================
*/
void UI_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
	}
}

/*
=================
UI_UpdateCvars
=================
*/
void UI_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Update( cv->vmCvar );
	}
}


/*
=================
UI_VideoCheck
=================
*/
void UI_VideoCheck( int time ) 
{
	if ( abs( time - uis.lastVideoCheck ) > 1000 ) {
		
		int oldWidth, oldHeight;
		oldWidth = uis.glconfig.vidWidth;
		oldHeight = uis.glconfig.vidHeight;

		trap_GetGlconfig( &uis.glconfig );

		if ( uis.glconfig.vidWidth != oldWidth || uis.glconfig.vidHeight != oldHeight ) {
			uis.biasY = 0.0;
			uis.biasX = 0.0;
			// for 640x480 virtualized screen
			if ( uis.glconfig.vidWidth * 480 > uis.glconfig.vidHeight * 640 ) {
				// wide screen, scale by height
				uis.scale = uis.glconfig.vidHeight * (1.0/480.0);
				uis.biasX = 0.5 * ( uis.glconfig.vidWidth - ( uis.glconfig.vidHeight * (640.0/480.0) ) );
			} else {
				// no wide screen, scale by width
				uis.scale = uis.glconfig.vidWidth * (1.0/640.0);
				uis.biasY = 0.5 * ( uis.glconfig.vidHeight - ( uis.glconfig.vidWidth * (480.0/640) ) );
			}

			uis.screenXmin = 0.0 - (uis.biasX / uis.scale);
			uis.screenXmax = 640.0 + (uis.biasX / uis.scale);

			uis.screenYmin = 0.0 - (uis.biasY / uis.scale);
			uis.screenYmax = 480.0 + (uis.biasY / uis.scale);

			uis.cursorScaleR = 1.0 / uis.scale;
			if ( uis.cursorScaleR < 0.5 ) {
				uis.cursorScaleR = 0.5;
			}
		}

		uis.lastVideoCheck = time;
	}
}
