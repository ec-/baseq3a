// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "ui_local.h"

#define SERVERINFO_FRAMEL	"menu/art/frame2_l"
#define SERVERINFO_FRAMER	"menu/art/frame1_r"
#define SERVERINFO_BACK0	"menu/art/back_0"
#define SERVERINFO_BACK1	"menu/art/back_1"

static char* serverinfo_artlist[] =
{
	SERVERINFO_FRAMEL,	
	SERVERINFO_FRAMER,
	SERVERINFO_BACK0,
	SERVERINFO_BACK1,
	NULL
};

#define ID_ADD	 100
#define ID_BACK	 101

typedef struct
{
	menuframework_s	menu;
	menutext_s		banner;
	menubitmap_s	framel;
	menubitmap_s	framer;
	menubitmap_s	back;
	menutext_s		add;
	menulist_s		list;
	char			info[MAX_INFO_STRING];
} serverinfo_t;

static serverinfo_t	s_serverinfo;

#define MAX_INFO_LINES		64
#define INFO_LINE_WIDTH		51

static	char	*itemnames[MAX_INFO_LINES];
static	char	show_info[MAX_INFO_LINES][INFO_LINE_WIDTH*3];

/*
=================
Favorites_Add

Add current server to favorites
=================
*/
void Favorites_Add( void )
{
	char	adrstr[128];
	char	serverbuff[128];
	int		i;
	int		best;

	trap_Cvar_VariableStringBuffer( "cl_currentServerAddress", serverbuff, sizeof(serverbuff) );
	if (!serverbuff[0])
		return;

	best = 0;
	for (i=0; i<MAX_FAVORITESERVERS; i++)
	{
		trap_Cvar_VariableStringBuffer( va("server%d",i+1), adrstr, sizeof(adrstr) );
		if (!Q_stricmp(serverbuff,adrstr))
		{
			// already in list
			return;
		}
		
		// use first empty or non-numeric available slot
		if ((adrstr[0]  < '0' || adrstr[0] > '9' ) && !best)
			best = i+1;
	}

	if (best)
		trap_Cvar_Set( va("server%d",best), serverbuff);
}


/*
=================
ServerInfo_Event
=================
*/
static void ServerInfo_Event( void* ptr, int event )
{
	switch (((menucommon_s*)ptr)->id)
	{
		case ID_ADD:
			if (event != QM_ACTIVATED)
				break;
		
			Favorites_Add();
			UI_PopMenu();
			break;

		case ID_BACK:
			if (event != QM_ACTIVATED)
				break;

			UI_PopMenu();
			break;
	}
}

/*
=================
ServerInfo_MenuKey
=================
*/
static sfxHandle_t ServerInfo_MenuKey( int key )
{
	return ( Menu_DefaultKey( &s_serverinfo.menu, key ) );
}

/*
=================
ServerInfo_Cache
=================
*/
void ServerInfo_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!serverinfo_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(serverinfo_artlist[i]);
	}
}

/*
=================
UI_ServerInfoMenu
=================
*/
void UI_ServerInfoMenu( void )
{
	const char		*s;
	char			key[MAX_INFO_KEY], *str;
	char			value[MAX_INFO_VALUE], buf[MAX_INFO_VALUE*2];
	int				i, len, max;

	// zero set all our globals
	memset( &s_serverinfo, 0 ,sizeof(serverinfo_t) );

	ServerInfo_Cache();

	s_serverinfo.menu.key        = ServerInfo_MenuKey;
	s_serverinfo.menu.wrapAround = qtrue;
	s_serverinfo.menu.fullscreen = qtrue;

	s_serverinfo.banner.generic.type  = MTYPE_BTEXT;
	s_serverinfo.banner.generic.x	  = 320;
	s_serverinfo.banner.generic.y	  = 16;
	s_serverinfo.banner.string		  = "SERVER INFO";
	s_serverinfo.banner.color	      = color_white;
	s_serverinfo.banner.style	      = UI_CENTER;

	s_serverinfo.framel.generic.type  = MTYPE_BITMAP;
	s_serverinfo.framel.generic.name  = SERVERINFO_FRAMEL;
	s_serverinfo.framel.generic.flags = QMF_INACTIVE;
	s_serverinfo.framel.generic.x	  = 0;  
	s_serverinfo.framel.generic.y	  = 78;
	s_serverinfo.framel.width  	      = 256;
	s_serverinfo.framel.height  	  = 329;

	s_serverinfo.framer.generic.type  = MTYPE_BITMAP;
	s_serverinfo.framer.generic.name  = SERVERINFO_FRAMER;
	s_serverinfo.framer.generic.flags = QMF_INACTIVE;
	s_serverinfo.framer.generic.x	  = 376;
	s_serverinfo.framer.generic.y	  = 76;
	s_serverinfo.framer.width  	      = 256;
	s_serverinfo.framer.height  	  = 334;

	s_serverinfo.add.generic.type	  = MTYPE_PTEXT;
	s_serverinfo.add.generic.flags    = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_serverinfo.add.generic.callback = ServerInfo_Event;
	s_serverinfo.add.generic.id	      = ID_ADD;
	s_serverinfo.add.generic.x		  = 320;
	s_serverinfo.add.generic.y		  = 371;
	s_serverinfo.add.string  		  = "ADD TO FAVORITES";
	s_serverinfo.add.style  		  = UI_CENTER|UI_SMALLFONT;
	s_serverinfo.add.color			  =	color_red;
	if( trap_Cvar_VariableValue( "sv_running" ) ) {
		s_serverinfo.add.generic.flags |= QMF_GRAYED;
	}

	s_serverinfo.back.generic.type	   = MTYPE_BITMAP;
	s_serverinfo.back.generic.name     = SERVERINFO_BACK0;
	s_serverinfo.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_serverinfo.back.generic.callback = ServerInfo_Event;
	s_serverinfo.back.generic.id	   = ID_BACK;
	s_serverinfo.back.generic.x		   = 0;
	s_serverinfo.back.generic.y		   = 480-64;
	s_serverinfo.back.width  		   = 128;
	s_serverinfo.back.height  		   = 64;
	s_serverinfo.back.focuspic         = SERVERINFO_BACK1;

	trap_GetConfigString( CS_SERVERINFO, s_serverinfo.info, MAX_INFO_STRING );

	max = 0;
	s = s_serverinfo.info;
	do {
		s = Info_NextPair( s, key, value );
		if ( key[0] == '\0' ) {
			break;
		}
		len = strlen( key );
		if ( len > max )
			max = len;
	} while ( *s != '\0' );

	s_serverinfo.list.generic.type		= MTYPE_SCROLLLIST;
	s_serverinfo.list.generic.flags		= QMF_PULSEIFFOCUS;
	s_serverinfo.list.generic.id		= 123;
	s_serverinfo.list.generic.x			= 120;
	s_serverinfo.list.generic.y			= 132;
	s_serverinfo.list.width				= INFO_LINE_WIDTH;
	s_serverinfo.list.height			= 14;
	s_serverinfo.list.columns			= 1;
	s_serverinfo.list.scroll			= 1;

	s_serverinfo.list.itemnames = (const char **)itemnames;

	s_serverinfo.list.numitems = 0;
	s = s_serverinfo.info;
	do {
		s = Info_NextPair( s, key, value );
		if ( key[0] == '\0' )
			break;

		str = show_info[s_serverinfo.list.numitems];
		s_serverinfo.list.itemnames[s_serverinfo.list.numitems] = str;
		
		len = strlen( key );
		len = max - len; 

		// align key name
		for ( i = 0; i < len; i++ )
			buf[i] = ' ';
		BG_sprintf( buf + i, "%s ^3%s", key, value );

		// take care about overflow in destination string
		buf[INFO_LINE_WIDTH*3-1] = '\0';
		strcpy( str, buf );
		s_serverinfo.list.numitems++;
		if ( s_serverinfo.list.numitems >= MAX_INFO_LINES )
			break;
	} while ( *s != '\0' );

	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.banner );
	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.framel );
	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.framer );
	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.add );
	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.back );
	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.list );

	UI_PushMenu( &s_serverinfo.menu );
}


