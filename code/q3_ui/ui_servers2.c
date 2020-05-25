// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

MULTIPLAYER MENU (SERVER BROWSER)

=======================================================================
*/


#include "ui_local.h"
#include "../game/q_shared.h"


#define REFRESH_DELAY			10	  // in ms
#define MAX_RESPONSE_TIME		10000 // in ms
#define MAX_GLOBALSERVERS		MAX_GLOBAL_SERVERS
#define MAX_PINGLISTSIZE		MAX_PINGREQUESTS*8
#define MAX_ADDRESSLENGTH		64
#define MAX_HOSTNAMELENGTH		26
#define MAX_MAPNAMELENGTH		11
#define MAX_GAMENAMELENGTH		8
#define MAX_LISTBOXITEMS		512
#define MAX_LOCALSERVERS		512
#define MAX_STATUSLENGTH		64

#define MAX_LISTBOXWIDTH		MAX_HOSTNAMELENGTH + 1 + MAX_MAPNAMELENGTH + 1 + 5 /*players/max*/ + 1 + MAX_GAMENAMELENGTH + 1 + 3 /*netname*/ + 1 + 3 /*ping*/

#define MAX_LISTBOXWIDTH_BUF	MAX_LISTBOXWIDTH + 2 /*color ping */ + 1 /*zero termination*/

#define FILTER_CAPTION_CHARS	13

#define ART_BACK0				"menu/art/back_0"
#define ART_BACK1				"menu/art/back_1"
#define ART_CREATE0				"menu/art/create_0"
#define ART_CREATE1				"menu/art/create_1"
#define ART_SPECIFY0			"menu/art/specify_0"
#define ART_SPECIFY1			"menu/art/specify_1"
#define ART_REFRESH0			"menu/art/refresh_0"
#define ART_REFRESH1			"menu/art/refresh_1"
#define ART_CONNECT0			"menu/art/fight_0"
#define ART_CONNECT1			"menu/art/fight_1"
#define ART_ARROWS0				"menu/art/arrows_vert_0"
#define ART_ARROWS_UP			"menu/art/arrows_vert_top"
#define ART_ARROWS_DOWN			"menu/art/arrows_vert_bot"
#define ART_UNKNOWNMAP			"menu/art/unknownmap"
#define ART_REMOVE0				"menu/art/delete_0"
#define ART_REMOVE1				"menu/art/delete_1"

#define ID_MASTER			10
#define ID_GAMETYPE			11
#define ID_SORTKEY			12
#define ID_SHOW_FULL		13
#define ID_SHOW_EMPTY		14
#define ID_LIST				15
#define ID_SCROLL_UP		16
#define ID_SCROLL_DOWN		17
#define ID_BACK				18
#define ID_REFRESH			19
#define ID_SPECIFY			20
#define ID_CREATE			21
#define ID_CONNECT			22
#define ID_REMOVE			23
#define ID_FILTER			24

#define GR_LOGO				30
#define GR_LETTERS			31

#define SORT_HOST			0
#define SORT_MAP			1
#define SORT_CLIENTS		2
#define SORT_GAME			3
#define SORT_PING			4

#define GAMES_ALL			0
#define GAMES_FFA			1
#define GAMES_TOURNEY		2
#define GAMES_TEAMPLAY		3
#define GAMES_CTF			4

static const char *master_items[] = {
	"Local",
	"Internet",
	"Favorites",
	NULL
};

static const char *servertype_items[] = {
	"All",
	"Free For All",
	"Tournament",
	"Team Deathmatch",
	"Capture the Flag",
	NULL
};

static const char *sortkey_items[] = {
	"Server Name",
	"Map Name",
	"Open Player Spots",
	"Game Type",
	"Ping Time",
	NULL
};

static char* netnames[] = {
	"???",
	"UDP",
	"IPX",
	NULL
};

static char quake3worldMessage[] = "Visit www.quake3world.com - News, Community, Events, Files";

const char* punkbuster_items[] = {
	"Disabled",
	"Enabled",
	NULL
};

const char* punkbuster_msg[] = {
	"PunkBuster will be",
	"disabled the next time",
	"Quake III Arena",
	"is started.",
	NULL
};

typedef struct {
	char	adrstr[MAX_ADDRESSLENGTH];
	int		start;
} pinglist_t;

typedef struct servernode_s {
	char	adrstr[MAX_ADDRESSLENGTH];
	char	hostname[MAX_HOSTNAMELENGTH+1];
	char	mapname[MAX_MAPNAMELENGTH+1];
	int		numclients;
	int		maxclients;
	int		pingtime;
	int		gametype;
	char	gamename[MAX_GAMENAMELENGTH+1];
	int		nettype;
	int		minPing;
	int		maxPing;
} servernode_t; 

typedef struct {
	char			buff[MAX_LISTBOXWIDTH_BUF];
	servernode_t*	servernode;
} table_t;

typedef struct {
	menuframework_s		menu;

	menutext_s			banner;

	menulist_s			master;
	menulist_s			gametype;
	menulist_s			sortkey;
	menuradiobutton_s	showfull;
	menuradiobutton_s	showempty;

	menulist_s			list;
	menubitmap_s		mappic;
	menubitmap_s		arrows;
	menubitmap_s		up;
	menubitmap_s		down;
	menutext_s			status;
	menutext_s			statusbar;

	menubitmap_s		remove;
	menubitmap_s		back;
	menubitmap_s		refresh;
	menubitmap_s		specify;
	menubitmap_s		create;
	menubitmap_s		go;

	menufield_s			filter;

	pinglist_t			pinglist[MAX_PINGLISTSIZE];
	table_t				table[MAX_LISTBOXITEMS];
	char*				items[MAX_LISTBOXITEMS];
	int					numqueriedservers;
	int					*numservers;
	servernode_t		*serverlist;	
	int					currentping;
	qboolean			refreshservers;
	int					nextpingtime;
	int					maxservers;
	int					refreshtime;
	char				favoriteaddresses[MAX_FAVORITESERVERS][MAX_ADDRESSLENGTH];
	int					numfavoriteaddresses;

	char				serverfilter[ MAX_EDIT_LINE ];
} arenaservers_t;

static arenaservers_t	g_arenaservers;

static servernode_t		g_globalserverlist[MAX_GLOBALSERVERS];
static int				g_numglobalservers;
static servernode_t		g_localserverlist[MAX_LOCALSERVERS];
static int				g_numlocalservers;
static servernode_t		g_favoriteserverlist[MAX_FAVORITESERVERS];
static int				g_numfavoriteservers;
static int				g_servertype;
static int				g_gametype;
static int				g_sortkey;
static int				g_emptyservers;
static int				g_fullservers;

static void ArenaServers_UpdateList( void );
static void ArenaServers_UpdatePicture( void );

/*
=================
ArenaServers_DrawFilter
=================
*/
static void ArenaServers_DrawFilter( void *self ) {
	menufield_s		*f;
	qboolean		focus;
	int				style;
	char			*txt;
	char			c;
	float			*color;
	int				basex, x, y;

	f = (menufield_s*)self;
	basex = f->generic.x;
	y = f->generic.y;
	focus = (f->generic.parent->cursor == f->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;

	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawString( basex, y, "Filter Name:", style, color );

	basex += FILTER_CAPTION_CHARS * SMALLCHAR_WIDTH;
	//y += PROP_HEIGHT;
	txt = f->field.buffer;
	x = basex;

	if ( strcmp( g_arenaservers.serverfilter, f->field.buffer) ) {
		strcpy( g_arenaservers.serverfilter, f->field.buffer );
		ArenaServers_UpdateList();
		ArenaServers_UpdatePicture();
	}

	// mark input by color depending from filter result
	if ( !g_arenaservers.list.numitems && f->field.buffer[0] && *g_arenaservers.numservers ) 
		color = g_color_table[ ColorIndex( COLOR_RED ) ];
	else
		color = g_color_table[ ColorIndex( COLOR_WHITE ) ];

	while ( (c = *txt) != '\0' ) {
		UI_DrawChar( x, y, c, style, color );
		x += SMALLCHAR_WIDTH;
		txt++;
	}

	// draw cursor if we have focus
	if ( focus ) {
		if ( trap_Key_GetOverstrikeMode() ) {
			c = 11;
		} else {
			c = 10;
		}

		style &= ~UI_PULSE;
		style |= UI_BLINK;

		UI_DrawChar( basex + f->field.cursor * SMALLCHAR_WIDTH, y, c, style, color );
	}
}


/*
=================
ArenaServers_MaxPing
=================
*/
static int ArenaServers_MaxPing( void ) {
	int		maxPing;

	maxPing = (int)trap_Cvar_VariableValue( "cl_maxPing" );
	if( maxPing < 100 ) {
		maxPing = 100;
	}
	return maxPing;
}


/*
=================
ArenaServers_Compare
=================
*/
static int QDECL ArenaServers_Compare( const void *arg1, const void *arg2 ) {
	float			f1;
	float			f2;
	servernode_t*	t1;
	servernode_t*	t2;
	int			result;

	t1 = (servernode_t *)arg1;
	t2 = (servernode_t *)arg2;

	switch( g_sortkey ) {
	case SORT_HOST:
		return Q_stricmp( t1->hostname, t2->hostname );

	case SORT_MAP:
		result = Q_stricmp( t1->mapname, t2->mapname );
		if ( !result ) {
			if( t1->pingtime < t2->pingtime ) {
				return -1;
			}
			if( t1->pingtime > t2->pingtime ) {
				return 1;
			}				
		}
		return result;
	case SORT_CLIENTS:
		f1 = t1->maxclients - t1->numclients;
		if( f1 < 0 ) {
			f1 = 0;
		}

		f2 = t2->maxclients - t2->numclients;
		if( f2 < 0 ) {
			f2 = 0;
		}

		if( f1 < f2 ) {
			return 1;
		}
		if( f1 == f2 ) {
			return 0;
		}
		return -1;

	case SORT_GAME:
		if( t1->gametype < t2->gametype ) {
			return -1;
		}
		if( t1->gametype == t2->gametype ) {
			return 0;
		}
		return 1;

	case SORT_PING:
		if( t1->pingtime < t2->pingtime ) {
			return -1;
		}
		if( t1->pingtime > t2->pingtime ) {
			return 1;
		}
		return Q_stricmp( t1->hostname, t2->hostname );
	}

	return 0;
}


/*
=================
ArenaServers_Go
=================
*/
static void ArenaServers_Go( void ) {
	const servernode_t* servernode;

	servernode = g_arenaservers.table[ g_arenaservers.list.curvalue ].servernode;
	if ( servernode ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", servernode->adrstr ) );
	}
}


/*
=================
ArenaServers_Dblclick
=================
*/
static void ArenaServers_Dblclick( void * self ) {
	ArenaServers_Go();
}


/*
=================
ArenaServers_UpdatePicture
=================
*/
static void ArenaServers_UpdatePicture( void ) {
	static char		picname[ MAX_QPATH ];
	const servernode_t* servernodeptr;

	if ( !g_arenaservers.list.numitems ) {
		g_arenaservers.mappic.generic.name = NULL;
	}
	else {
		servernodeptr = g_arenaservers.table[ g_arenaservers.list.curvalue ].servernode;
		Com_sprintf( picname, sizeof(picname), "levelshots/%s.tga", servernodeptr->mapname );
		g_arenaservers.mappic.generic.name = picname;
	
	}

	// force shader update during draw
	g_arenaservers.mappic.shader = 0;
}


/*
=================
ArenaServers_UpdateList
=================
*/
static void ArenaServers_UpdateList( void )
{
	int				i;
	int				j;
	int				count;
	servernode_t*	servernodeptr;
	table_t*		tableptr;
	const char		*pingColor;

	// build list box strings - apply culling filters
	servernodeptr = g_arenaservers.serverlist;
	count         = *g_arenaservers.numservers;
	for( i = 0, j = 0; i < count; i++, servernodeptr++ ) {
		tableptr = &g_arenaservers.table[j];
		tableptr->servernode = servernodeptr;

		// can only cull valid results
		if( !g_emptyservers && !servernodeptr->numclients ) {
			continue;
		}

		if( !g_fullservers && ( servernodeptr->numclients == servernodeptr->maxclients ) ) {
			continue;
		}

		if ( g_arenaservers.serverfilter[0] && !Q_stristr( servernodeptr->hostname, g_arenaservers.serverfilter ) ) {
			continue;
		}

		switch( g_gametype ) {
		case GAMES_ALL:
			break;

		case GAMES_FFA:
			if( servernodeptr->gametype != GT_FFA ) {
				continue;
			}
			break;

		case GAMES_TEAMPLAY:
			if( servernodeptr->gametype != GT_TEAM ) {
				continue;
			}
			break;

		case GAMES_TOURNEY:
			if( servernodeptr->gametype != GT_TOURNAMENT ) {
				continue;
			}
			break;

		case GAMES_CTF:
			if ( servernodeptr->gametype != GT_CTF ) {
				continue;
			}
			break;
		}

		if ( servernodeptr->pingtime < servernodeptr->minPing ) {
			pingColor = S_COLOR_BLUE;
		}
		else if ( servernodeptr->maxPing && servernodeptr->pingtime > servernodeptr->maxPing ) {
			pingColor = S_COLOR_BLUE;
		}
		else if ( servernodeptr->pingtime < 200 ) {
			pingColor = S_COLOR_GREEN;
		}
		else if( servernodeptr->pingtime < 400 ) {
			pingColor = S_COLOR_YELLOW;
		}
		else {
			pingColor = S_COLOR_RED;
		}

		Com_sprintf( tableptr->buff, sizeof( tableptr->buff ), "%-*.*s %-*.*s %2d/%2d %-*.*s %3s %s%3d",
			MAX_HOSTNAMELENGTH, MAX_HOSTNAMELENGTH, servernodeptr->hostname,
			MAX_MAPNAMELENGTH, MAX_MAPNAMELENGTH, servernodeptr->mapname,
			servernodeptr->numclients, servernodeptr->maxclients,
			MAX_GAMENAMELENGTH, MAX_GAMENAMELENGTH, servernodeptr->gamename,
			netnames[ servernodeptr->nettype ],
			pingColor, servernodeptr->pingtime );
		j++;
	}

	g_arenaservers.list.numitems = j;
	g_arenaservers.list.curvalue = 0;
	g_arenaservers.list.top      = 0;
}


/*
=================
ArenaServers_UpdateMenu
=================
*/
static void ArenaServers_UpdateMenu( void ) {

	if ( g_arenaservers.numqueriedservers > 0 )
	{
		// servers found
		if ( g_arenaservers.refreshservers && ( g_arenaservers.currentping <= g_arenaservers.numqueriedservers ) ) 
		{
			// show progress
			Com_sprintf( g_arenaservers.status.string, MAX_STATUSLENGTH, "%d of %d Arena Servers.", g_arenaservers.currentping, g_arenaservers.numqueriedservers);
			g_arenaservers.statusbar.string  = "Press SPACE to stop";
			qsort( g_arenaservers.serverlist, *g_arenaservers.numservers, sizeof( servernode_t ), ArenaServers_Compare);
		}
		else 
		{
			// all servers pinged - enable controls
			//g_arenaservers.gametype.generic.flags	&= ~QMF_GRAYED;
			//g_arenaservers.sortkey.generic.flags	&= ~QMF_GRAYED;
			//g_arenaservers.showempty.generic.flags&= ~QMF_GRAYED;
			//g_arenaservers.showfull.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.list.generic.flags		&= ~QMF_GRAYED;
			g_arenaservers.refresh.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.go.generic.flags			&= ~QMF_GRAYED;
			//g_arenaservers.punkbuster.generic.flags &= ~QMF_GRAYED;

			// update status bar
			if ( g_servertype == AS_GLOBAL ) {
				g_arenaservers.statusbar.string = quake3worldMessage;
			} else {
				g_arenaservers.statusbar.string = "";
			}
		}
	}
	else {
		// no servers found
		if( g_arenaservers.refreshservers ) {
			strcpy( g_arenaservers.status.string,"Scanning For Servers." );
			g_arenaservers.statusbar.string = "Press SPACE to stop";

			// disable controls during refresh
			//g_arenaservers.gametype.generic.flags	|= QMF_GRAYED;
			//g_arenaservers.sortkey.generic.flags	|= QMF_GRAYED;
			//g_arenaservers.showempty.generic.flags|= QMF_GRAYED;
			//g_arenaservers.showfull.generic.flags	|= QMF_GRAYED;
			g_arenaservers.list.generic.flags		|= QMF_GRAYED;
			g_arenaservers.refresh.generic.flags	|= QMF_GRAYED;
			g_arenaservers.go.generic.flags			|= QMF_GRAYED;
		}
		else 
		{
			if ( g_arenaservers.numqueriedservers < 0 ) {
				strcpy( g_arenaservers.status.string, "No Response From Master Server." );
			} else {
				strcpy( g_arenaservers.status.string, "No Servers Found." );
			}

			// update status bar
			if ( g_servertype == AS_GLOBAL ) {
				g_arenaservers.statusbar.string = quake3worldMessage;
			} else {
				g_arenaservers.statusbar.string = "";
			}

			// end of refresh - set control state
			g_arenaservers.master.generic.flags		&= ~QMF_GRAYED;
			g_arenaservers.gametype.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.sortkey.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.showempty.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.showfull.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.list.generic.flags		|= QMF_GRAYED;
			g_arenaservers.refresh.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.go.generic.flags			|= QMF_GRAYED;
		}

		// zero out list box
		g_arenaservers.list.numitems = 0;
		g_arenaservers.list.curvalue = 0;
		g_arenaservers.list.top      = 0;

		// update picture
		ArenaServers_UpdatePicture();
		return;
	}

	ArenaServers_UpdateList();

	// update picture
	ArenaServers_UpdatePicture();
}


/*
=================
ArenaServers_Remove
=================
*/
static void ArenaServers_Remove( void )
{
	int				i;
	servernode_t*	servernodeptr;
	table_t*		tableptr;

	if (!g_arenaservers.list.numitems)
		return;

	// remove selected item from display list
	// items are in scattered order due to sort and cull
	// perform delete on list box contents, resync all lists

	tableptr      = &g_arenaservers.table[g_arenaservers.list.curvalue];
	servernodeptr = tableptr->servernode;

	// find address in master list
	for (i=0; i<g_arenaservers.numfavoriteaddresses; i++)
		if (!Q_stricmp(g_arenaservers.favoriteaddresses[i],servernodeptr->adrstr))
				break;

	// delete address from master list
	if (i <= g_arenaservers.numfavoriteaddresses-1)
	{
		if (i < g_arenaservers.numfavoriteaddresses-1)
		{
			// shift items up
			memcpy( &g_arenaservers.favoriteaddresses[i], &g_arenaservers.favoriteaddresses[i+1], (g_arenaservers.numfavoriteaddresses - i - 1)*MAX_ADDRESSLENGTH);
		}
		g_arenaservers.numfavoriteaddresses--;
	}	

	// find address in server list
	for (i=0; i<g_numfavoriteservers; i++)
		if (&g_favoriteserverlist[i] == servernodeptr)
				break;

	// delete address from server list
	if (i <= g_numfavoriteservers-1)
	{
		if (i < g_numfavoriteservers-1)
		{
			// shift items up
			memcpy( &g_favoriteserverlist[i], &g_favoriteserverlist[i+1], (g_numfavoriteservers - i - 1)*sizeof(servernode_t));
		}
		g_numfavoriteservers--;
	}	

	g_arenaservers.numqueriedservers = g_arenaservers.numfavoriteaddresses;
	g_arenaservers.currentping       = g_arenaservers.numfavoriteaddresses;
}


static qboolean UI_CleanStr( char *dst, int size, const char *src ) 
{
	char	*max = dst + size - 1;
	int		c;
	qboolean filtered = qfalse;

	while ( (c = *src) != '\0' ) {
		if ( c <= ' ' ) {
			filtered = qtrue;
			src+=1; continue;
		}
		if ( Q_IsColorString( src ) && *(src+2) <= ' ' ) {
			filtered = qtrue;
			src+=2; continue;
		}
		break;
	}

	while ( (c = *src) != '\0' ) {
		if ( Q_IsColorString( src ) ) {
			filtered = qtrue;
			src += 2;
			continue;
		} else if ( c >= ' ' && c <= '~' ) {
			*dst = c; dst++;
			if ( dst >= max )
				break;
		}
		src++;
	}

	*dst = '\0';

	return filtered;
}


/*
=================
ArenaServers_Insert
=================
*/
static void ArenaServers_Insert( const char *adrstr, const char *info, int pingtime )
{
	servernode_t	*servernodeptr;
	char			*s;
	int				i;

	s = Info_ValueForKey( info, "game" );
	if ( !Q_stricmp( s, "q3ut4" ) ) 
	{
		return; // filter urbanterror servers
	}

	if ( atoi( Info_ValueForKey( info, "punkbuster" ) ) ) 
	{
		return; // filter PunkBuster servers
	}

	if ((pingtime >= ArenaServers_MaxPing()) && (g_servertype != AS_FAVORITES))
	{
		// slow global or local servers do not get entered
		return;
	}

	if ( pingtime < 0 )
		pingtime = 0;
	else if ( pingtime > 999 )
		pingtime = 999;

	if ( *g_arenaservers.numservers >= g_arenaservers.maxservers ) {
		// list full;
		servernodeptr = g_arenaservers.serverlist+(*g_arenaservers.numservers)-1;
	} else {
		// next slot
		servernodeptr = g_arenaservers.serverlist+(*g_arenaservers.numservers);
		(*g_arenaservers.numservers)++;
	}

	Q_strncpyz( servernodeptr->adrstr, adrstr, sizeof( servernodeptr->adrstr ) );

	if ( UI_CleanStr( servernodeptr->hostname, sizeof( servernodeptr->hostname ), Info_ValueForKey( info, "hostname" ) ) ) {
		// some servers abusing color sequences - lets filter them until completely cleaned
		while ( UI_CleanStr( servernodeptr->hostname, sizeof( servernodeptr->hostname ), servernodeptr->hostname ) )
			;
	}
	
	Q_strncpyz( servernodeptr->mapname, Info_ValueForKey( info, "mapname" ), sizeof( servernodeptr->mapname ) );
	Q_CleanStr( servernodeptr->mapname );
	Q_strupr( servernodeptr->mapname );

	servernodeptr->numclients = abs( atoi( Info_ValueForKey( info, "clients") ) );
	servernodeptr->maxclients = abs( atoi( Info_ValueForKey( info, "sv_maxclients") ) );
	servernodeptr->pingtime   = pingtime;
	servernodeptr->minPing    = atoi( Info_ValueForKey( info, "minPing") );
	servernodeptr->maxPing    = atoi( Info_ValueForKey( info, "maxPing") );

	// avoid potential string overflow
	if( servernodeptr->numclients > 99 )
		servernodeptr->numclients = 99;
	if( servernodeptr->maxclients > 99 )
		servernodeptr->maxclients = 99;

	/*
	s = Info_ValueForKey( info, "nettype" );
	for (i=0; ;i++)
	{
		if (!netnames[i])
		{
			servernodeptr->nettype = 0;
			break;
		}
		else if (!Q_stricmp( netnames[i], s ))
		{
			servernodeptr->nettype = i;
			break;
		}
	}
	*/

	servernodeptr->nettype = atoi( Info_ValueForKey( info, "nettype" ) );
	if ( (unsigned)servernodeptr->nettype >= ARRAY_LEN( netnames ) - 1 ) {
		servernodeptr->nettype = 0;
	}

	i = atoi( Info_ValueForKey( info, "gametype" ) );
	if ( (unsigned)i > GT_MAX_GAME_TYPE-1 ) {
		i = GT_MAX_GAME_TYPE-1;
	}
	servernodeptr->gametype = i;

	s = Info_ValueForKey( info, "game" );
	if ( *s )
		Q_strncpyz( servernodeptr->gamename, s, sizeof( servernodeptr->gamename ) );
	else
		Q_strncpyz( servernodeptr->gamename, "unknown", sizeof( servernodeptr->gamename ) );
}


/*
=================
ArenaServers_LoadFavorites

Load cvar address book entries into local lists.
=================
*/
void ArenaServers_LoadFavorites( void )
{
	int				i;
	int				j;
	int				numtempitems;
	char			adrstr[MAX_ADDRESSLENGTH];
	servernode_t	templist[MAX_FAVORITESERVERS];
	qboolean		found;

	found        = qfalse;

	// copy the old
	memcpy( templist, g_favoriteserverlist, sizeof(servernode_t)*MAX_FAVORITESERVERS );
	numtempitems = g_numfavoriteservers;

	// clear the current for sync
	memset( g_favoriteserverlist, 0, sizeof(servernode_t)*MAX_FAVORITESERVERS );
	g_numfavoriteservers = 0;

	// resync existing results with new or deleted cvars
	for ( i = 0; i < MAX_FAVORITESERVERS; i++ )
	{
		trap_Cvar_VariableStringBuffer( va("server%d",i+1), adrstr, MAX_ADDRESSLENGTH );
		if ( !adrstr[0] )
			continue;

		// quick sanity check to avoid slow domain name resolving
		// first character must be numeric
		if ( adrstr[0] < '0' || adrstr[0] > '9' )
			continue;

		// favorite server addresses must be maintained outside refresh list
		// this mimics local and global netadr's stored in client
		// these can be fetched to fill ping list
		strcpy( g_arenaservers.favoriteaddresses[g_numfavoriteservers], adrstr );

		// find this server in the old list
		for (j=0; j<numtempitems; j++)
			if (!Q_stricmp( templist[j].adrstr, adrstr ))
				break;

		if (j < numtempitems)
		{
			// found server - add exisiting results
			memcpy( &g_favoriteserverlist[g_numfavoriteservers], &templist[j], sizeof(servernode_t) );
			found = qtrue;
		}
		else
		{
			// add new server
			Q_strncpyz( g_favoriteserverlist[g_numfavoriteservers].adrstr, adrstr, MAX_ADDRESSLENGTH );
			g_favoriteserverlist[g_numfavoriteservers].pingtime = ArenaServers_MaxPing();
		}

		g_numfavoriteservers++;
	}

	g_arenaservers.numfavoriteaddresses = g_numfavoriteservers;

	if (!found)
	{
		// no results were found, reset server list
		// list will be automatically refreshed when selected
		g_numfavoriteservers = 0;
	}
}


/*
=================
ArenaServers_StopRefresh
=================
*/
static void ArenaServers_StopRefresh( void )
{
	if (!g_arenaservers.refreshservers)
		// not currently refreshing
		return;

	g_arenaservers.refreshservers = qfalse;

	// final tally
	if (g_arenaservers.numqueriedservers >= 0)
	{
		g_arenaservers.currentping       = *g_arenaservers.numservers;
		g_arenaservers.numqueriedservers = *g_arenaservers.numservers; 
	}
	
	// sort
	qsort( g_arenaservers.serverlist, *g_arenaservers.numservers, sizeof( servernode_t ), ArenaServers_Compare);

	ArenaServers_UpdateMenu();
}


/*
=================
ArenaServers_DoRefresh
=================
*/
static void ArenaServers_DoRefresh( void )
{
	int		i;
	int		j;
	int		time;
	int		maxPing;
	char	adrstr[MAX_ADDRESSLENGTH];
	char	info[MAX_INFO_STRING];

	if (uis.realtime < g_arenaservers.refreshtime)
	{
	  if (g_servertype != AS_FAVORITES) {
			if (g_servertype == AS_LOCAL) {
				if (!trap_LAN_GetServerCount(g_servertype)) {
					return;
				}
			}
			if (trap_LAN_GetServerCount(g_servertype) < 0) {
			  // still waiting for response
			  return;
			}
	  }
	} else if (g_servertype == AS_LOCAL) {
		if ( !trap_LAN_GetServerCount(AS_LOCAL)) {
			// no local servers found, check again
			trap_Cmd_ExecuteText( EXEC_APPEND, "localservers\n" );
			g_arenaservers.refreshtime = uis.realtime + 5000;
			return;
		}
	}

	if (uis.realtime < g_arenaservers.nextpingtime)
	{
		// wait for time trigger
		return;
	}

	// trigger after REFRESH_DELAY 
	g_arenaservers.nextpingtime = uis.realtime + REFRESH_DELAY;

	// process ping results
	maxPing = ArenaServers_MaxPing();
	for (i=0; i<MAX_PINGREQUESTS; i++)
	{
		trap_LAN_GetPing( i, adrstr, MAX_ADDRESSLENGTH, &time );
		if (!adrstr[0])
		{
			// ignore empty or pending pings
			continue;
		}

		// find ping result in our local list
		for (j=0; j<MAX_PINGLISTSIZE; j++)
			if (!Q_stricmp( adrstr, g_arenaservers.pinglist[j].adrstr ))
				break;

		if (j < MAX_PINGLISTSIZE)
		{
			// found it
			if (!time)
			{
				time = uis.realtime - g_arenaservers.pinglist[j].start;
				if (time < maxPing)
				{
					// still waiting
					continue;
				}
			}

			if (time > maxPing)
			{
				// stale it out
				info[0] = '\0';
				time = maxPing;

				// set hostname for nonresponsive favorite server
				if ( g_servertype == AS_LOCAL ) {
					Info_SetValueForKey( info, "hostname", adrstr );
					Info_SetValueForKey( info, "game", "???" );
				}
			}
			else
			{
				trap_LAN_GetPingInfo( i, info, MAX_INFO_STRING );
			}

			// insert ping results
			if ( time < maxPing && *g_arenaservers.numservers < MAX_LISTBOXITEMS ) 
			{
				ArenaServers_Insert( adrstr, info, time );
			}

			// clear this query from internal list
			g_arenaservers.pinglist[j].adrstr[0] = '\0';
   		}

		// clear this query from external list
		trap_LAN_ClearPing( i );
	}

	// get results of servers query
	// counts can increase as servers respond
	if ( g_servertype == AS_FAVORITES ) {
		g_arenaservers.numqueriedservers = g_arenaservers.numfavoriteaddresses;
	} else {
		g_arenaservers.numqueriedservers = trap_LAN_GetServerCount( g_servertype );
	}

//	if (g_arenaservers.numqueriedservers > g_arenaservers.maxservers)
//		g_arenaservers.numqueriedservers = g_arenaservers.maxservers;

	// send ping requests in reasonable bursts
	// iterate ping through all found servers

	for (i=0; i<MAX_PINGLISTSIZE && g_arenaservers.currentping < g_arenaservers.numqueriedservers; i++)
	{
		if (trap_LAN_GetPingQueueCount() >= MAX_PINGREQUESTS)
		{
			// ping queue is full
			break;
		}

		// find empty slot
		for (j=0; j<MAX_PINGLISTSIZE; j++)
			if (!g_arenaservers.pinglist[j].adrstr[0]) {
				break;
			}

		if (j >= MAX_PINGLISTSIZE) {
			// no empty slots available yet - wait for timeout
#ifdef _DEBUG
			Com_Printf( S_COLOR_YELLOW "no empty slots in ping list\n" );
#endif
			break;
		}
			

		// get an address to ping

		if (g_servertype == AS_FAVORITES) {
		  strcpy( adrstr, g_arenaservers.favoriteaddresses[g_arenaservers.currentping] ); 		
		} else {
		  trap_LAN_GetServerAddressString(g_servertype, g_arenaservers.currentping, adrstr, MAX_ADDRESSLENGTH );
		}

		strcpy( g_arenaservers.pinglist[j].adrstr, adrstr );
		g_arenaservers.pinglist[j].start = uis.realtime;

		trap_Cmd_ExecuteText( EXEC_NOW, va( "ping %s\n", adrstr )  );
		
		// advance to next server
		g_arenaservers.currentping++;
	}

	if ( !trap_LAN_GetPingQueueCount() )
	{
		// all pings completed
		ArenaServers_StopRefresh();
		return;
	}

	// update the user interface with ping status
	ArenaServers_UpdateMenu();
}


/*
=================
ArenaServers_StartRefresh
=================
*/
static void ArenaServers_StartRefresh( void )
{
	int		i;
	char	myargs[32], protocol[24];

	memset( g_arenaservers.serverlist, 0, g_arenaservers.maxservers*sizeof(table_t) );

	for ( i = 0; i < MAX_PINGLISTSIZE; i++ )
		g_arenaservers.pinglist[i].adrstr[0] = '\0';

	for ( i = 0; i < MAX_PINGREQUESTS; i++ )
		trap_LAN_ClearPing( i );

	g_arenaservers.refreshservers    = qtrue;
	g_arenaservers.currentping       = 0;
	g_arenaservers.nextpingtime      = 0;
	*g_arenaservers.numservers       = 0;
	g_arenaservers.numqueriedservers = 0;

	// allow max 10 seconds for responses
	g_arenaservers.refreshtime = uis.realtime + MAX_RESPONSE_TIME;

	// place menu in zeroed state
	ArenaServers_UpdateMenu();

	if ( g_servertype == AS_LOCAL ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "localservers\n" );
		return;
	}

	if ( g_servertype == AS_GLOBAL ) {
#if 1
		myargs[0] = '\0';
#else
		switch ( g_arenaservers.gametype.curvalue ) {
			default:
			case GAMES_ALL:
				myargs[0] = 0;
				break;

			case GAMES_FFA:
				strcpy( myargs, " ffa" );
				break;

			case GAMES_TEAMPLAY:
				strcpy( myargs, " team" );
				break;

			case GAMES_TOURNEY:
				strcpy( myargs, " tourney" );
				break;

			case GAMES_CTF:
				strcpy( myargs, " ctf" );
				break;
		}
#endif

		//if ( g_emptyservers ) {
			strcat( myargs, " empty" );
		//}

		//if ( g_fullservers ) {
			strcat( myargs, " full" );
		//}

		protocol[0] = '\0';
		trap_Cvar_VariableStringBuffer( "debug_protocol", protocol, sizeof( protocol ) );
		if ( protocol[0] ) {
			trap_Cmd_ExecuteText( EXEC_APPEND, va( "globalservers 0 %s%s\n", protocol, myargs ));
		} else {
			trap_Cmd_ExecuteText( EXEC_APPEND, va( "globalservers 0 %d%s\n", (int)trap_Cvar_VariableValue( "protocol" ), myargs ) );
		}
	}
}


/*
=================
ArenaServers_SaveChanges
=================
*/
void ArenaServers_SaveChanges( void )
{
	int	i;

	for (i=0; i<g_arenaservers.numfavoriteaddresses; i++)
		trap_Cvar_Set( va("server%d",i+1), g_arenaservers.favoriteaddresses[i] );

	for (; i<MAX_FAVORITESERVERS; i++)
		trap_Cvar_Set( va("server%d",i+1), "" );
}


/*
=================
ArenaServers_Sort
=================
*/
void ArenaServers_Sort( int type ) {
	if ( g_sortkey == type ) {
		return;
	}

	g_sortkey = type;
	qsort( g_arenaservers.serverlist, *g_arenaservers.numservers, sizeof( servernode_t ), ArenaServers_Compare );
}


/*
=================
ArenaServers_SetType
=================
*/
void ArenaServers_SetType( int type )
{
	if ( g_servertype == type )
		return;

	ArenaServers_StopRefresh();

	g_servertype = type;

	switch( type ) {
	default:
	case AS_LOCAL:
		g_arenaservers.remove.generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		g_arenaservers.serverlist = g_localserverlist;
		g_arenaservers.numservers = &g_numlocalservers;
		g_arenaservers.maxservers = MAX_LOCALSERVERS;
		break;

	case AS_GLOBAL:
	case AS_MPLAYER:
		g_arenaservers.remove.generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		g_arenaservers.serverlist = g_globalserverlist;
		g_arenaservers.numservers = &g_numglobalservers;
		g_arenaservers.maxservers = MAX_GLOBALSERVERS;
		break;

	case AS_FAVORITES:
		g_arenaservers.remove.generic.flags &= ~(QMF_INACTIVE|QMF_HIDDEN);
		g_arenaservers.serverlist = g_favoriteserverlist;
		g_arenaservers.numservers = &g_numfavoriteservers;
		g_arenaservers.maxservers = MAX_FAVORITESERVERS;
		break;
	}

	if( !*g_arenaservers.numservers ) {
		ArenaServers_StartRefresh();
	}
	else {
		// avoid slow operation, use existing results
		g_arenaservers.currentping       = *g_arenaservers.numservers;
		g_arenaservers.numqueriedservers = *g_arenaservers.numservers; 
		ArenaServers_UpdateMenu();
	}
	strcpy( g_arenaservers.status.string, "hit refresh to update" );
}


/*
=================
ArenaServers_Event
=================
*/
static void ArenaServers_Event( void* ptr, int event ) {
	int		id;
	int value;

	id = ((menucommon_s*)ptr)->id;

	if( event != QM_ACTIVATED && id != ID_LIST && id != ID_SHOW_EMPTY && id != ID_SHOW_FULL ) {
		return;
	}

	switch( id ) {
	case ID_MASTER:
		value = g_arenaservers.master.curvalue;
		if (value >= 1)
		{
			value++;
		}
		trap_Cvar_SetValue( "ui_browserMaster", value );
		ArenaServers_SetType( value );
		break;

	case ID_GAMETYPE:
		trap_Cvar_SetValue( "ui_browserGameType", g_arenaservers.gametype.curvalue );
		if ( g_gametype != g_arenaservers.gametype.curvalue ) 
		{
			g_gametype = g_arenaservers.gametype.curvalue;
			ArenaServers_UpdateList();
			ArenaServers_UpdatePicture();
		}
		break;

	case ID_SORTKEY:
		trap_Cvar_SetValue( "ui_browserSortKey", g_arenaservers.sortkey.curvalue );
		if ( g_sortkey != g_arenaservers.sortkey.curvalue ) 
		{
			ArenaServers_Sort( g_arenaservers.sortkey.curvalue );
			ArenaServers_UpdateList();
			ArenaServers_UpdatePicture();
		}
		break;

	case ID_SHOW_FULL:
		trap_Cvar_SetValue( "ui_browserShowFull", g_arenaservers.showfull.curvalue );
		if ( g_fullservers != g_arenaservers.showfull.curvalue )
		{
			g_fullservers = g_arenaservers.showfull.curvalue;
			ArenaServers_UpdateList();
			ArenaServers_UpdatePicture();
		}
		break;

	case ID_SHOW_EMPTY:
		trap_Cvar_SetValue( "ui_browserShowEmpty", g_arenaservers.showempty.curvalue );
		if ( g_emptyservers != g_arenaservers.showempty.curvalue )
		{
			g_emptyservers = g_arenaservers.showempty.curvalue;
			ArenaServers_UpdateList();
			ArenaServers_UpdatePicture();
		}
		break;

	case ID_LIST:
		if( event == QM_GOTFOCUS ) {
			ArenaServers_UpdatePicture();
		}
		break;

	case ID_SCROLL_UP:
		ScrollList_Key( &g_arenaservers.list, K_UPARROW );
		break;

	case ID_SCROLL_DOWN:
		ScrollList_Key( &g_arenaservers.list, K_DOWNARROW );
		break;

	case ID_BACK:
		ArenaServers_StopRefresh();
		ArenaServers_SaveChanges();
		UI_PopMenu();
		break;

	case ID_REFRESH:
		ArenaServers_StartRefresh();
		break;

	case ID_SPECIFY:
		UI_SpecifyServerMenu();
		break;

	case ID_CREATE:
		UI_StartServerMenu( qtrue );
		break;

	case ID_CONNECT:
		ArenaServers_Go();
		break;

	case ID_REMOVE:
		ArenaServers_Remove();
		ArenaServers_UpdateMenu();
		break;
	}
}


/*
=================
ArenaServers_MenuDraw
=================
*/
static void ArenaServers_MenuDraw( void )
{
	if (g_arenaservers.refreshservers)
		ArenaServers_DoRefresh();

	Menu_Draw( &g_arenaservers.menu );
}


/*
=================
ArenaServers_MenuKey
=================
*/
static sfxHandle_t ArenaServers_MenuKey( int key ) {
	if ( key == K_SPACE  && g_arenaservers.refreshservers ) {
		ArenaServers_StopRefresh();	
		return menu_move_sound;
	}

	if ( ( key == K_DEL || key == K_KP_DEL ) && ( g_servertype == AS_FAVORITES ) &&
		( Menu_ItemAtCursor( &g_arenaservers.menu) == &g_arenaservers.list ) ) {
		ArenaServers_Remove();
		ArenaServers_UpdateMenu();
		return menu_move_sound;
	}

	if ( key == K_MOUSE2 || key == K_ESCAPE ) {
		ArenaServers_StopRefresh();
		ArenaServers_SaveChanges();
	}

	if ( key == '/' && Menu_ItemAtCursor( &g_arenaservers.menu ) == (menucommon_s *)&g_arenaservers.list ) {
		Menu_SetCursorToItem( (menuframework_s *)&g_arenaservers, &g_arenaservers.filter );
		g_arenaservers.filter.field.skipKey = qtrue;
		return menu_in_sound;
	}

	return Menu_DefaultKey( &g_arenaservers.menu, key );
}


/*
=================
ArenaServers_MenuInit
=================
*/
static void ArenaServers_MenuInit( void ) {
	int			i;
	int			type;
	int			y;
	int			value;
	static char	statusbuffer[ MAX_STATUSLENGTH ];

	// zero set all our globals
	memset( &g_arenaservers, 0 ,sizeof(arenaservers_t) );

	ArenaServers_Cache();

	g_arenaservers.menu.fullscreen = qtrue;
	g_arenaservers.menu.wrapAround = qtrue;
	g_arenaservers.menu.draw       = ArenaServers_MenuDraw;
	g_arenaservers.menu.key        = ArenaServers_MenuKey;

	g_arenaservers.banner.generic.type  = MTYPE_BTEXT;
	g_arenaservers.banner.generic.flags = QMF_CENTER_JUSTIFY;
	g_arenaservers.banner.generic.x	    = 320;
	g_arenaservers.banner.generic.y	    = 16;
	g_arenaservers.banner.string  		= "ARENA SERVERS";
	g_arenaservers.banner.style  	    = UI_CENTER;
	g_arenaservers.banner.color  	    = color_white;

	y = 80;
	g_arenaservers.master.generic.type			= MTYPE_SPINCONTROL;
	g_arenaservers.master.generic.name			= "Servers:";
	g_arenaservers.master.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	g_arenaservers.master.generic.callback		= ArenaServers_Event;
	g_arenaservers.master.generic.id			= ID_MASTER;
	g_arenaservers.master.generic.x				= 320;
	g_arenaservers.master.generic.y				= y;
	g_arenaservers.master.itemnames				= master_items;

	y += SMALLCHAR_HEIGHT;
	g_arenaservers.gametype.generic.type		= MTYPE_SPINCONTROL;
	g_arenaservers.gametype.generic.name		= "Game Type:";
	g_arenaservers.gametype.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	g_arenaservers.gametype.generic.callback	= ArenaServers_Event;
	g_arenaservers.gametype.generic.id			= ID_GAMETYPE;
	g_arenaservers.gametype.generic.x			= 320;
	g_arenaservers.gametype.generic.y			= y;
	g_arenaservers.gametype.itemnames			= servertype_items;

	y += SMALLCHAR_HEIGHT;
	g_arenaservers.sortkey.generic.type			= MTYPE_SPINCONTROL;
	g_arenaservers.sortkey.generic.name			= "Sort By:";
	g_arenaservers.sortkey.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	g_arenaservers.sortkey.generic.callback		= ArenaServers_Event;
	g_arenaservers.sortkey.generic.id			= ID_SORTKEY;
	g_arenaservers.sortkey.generic.x			= 320;
	g_arenaservers.sortkey.generic.y			= y;
	g_arenaservers.sortkey.itemnames			= sortkey_items;

	y += SMALLCHAR_HEIGHT;
	g_arenaservers.showfull.generic.type		= MTYPE_RADIOBUTTON;
	g_arenaservers.showfull.generic.name		= "Show Full:";
	g_arenaservers.showfull.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	g_arenaservers.showfull.generic.callback	= ArenaServers_Event;
	g_arenaservers.showfull.generic.id			= ID_SHOW_FULL;
	g_arenaservers.showfull.generic.x			= 320;
	g_arenaservers.showfull.generic.y			= y;

	y += SMALLCHAR_HEIGHT;
	g_arenaservers.showempty.generic.type		= MTYPE_RADIOBUTTON;
	g_arenaservers.showempty.generic.name		= "Show Empty:";
	g_arenaservers.showempty.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	g_arenaservers.showempty.generic.callback	= ArenaServers_Event;
	g_arenaservers.showempty.generic.id			= ID_SHOW_EMPTY;
	g_arenaservers.showempty.generic.x			= 320;
	g_arenaservers.showempty.generic.y			= y;

	g_arenaservers.filter.generic.type			= MTYPE_FIELD;
	g_arenaservers.filter.generic.flags			= QMF_NODEFAULTINIT;
	g_arenaservers.filter.generic.ownerdraw		= ArenaServers_DrawFilter;
	g_arenaservers.filter.generic.id			= ID_FILTER;
	g_arenaservers.filter.field.widthInChars	= 36;
	g_arenaservers.filter.field.maxchars		= 36;
	g_arenaservers.filter.generic.x				= 216;
	g_arenaservers.filter.generic.y				= y + SMALLCHAR_HEIGHT;
	g_arenaservers.filter.generic.left			= g_arenaservers.filter.generic.x;
	g_arenaservers.filter.generic.right			= g_arenaservers.filter.generic.x + (g_arenaservers.filter.field.widthInChars + FILTER_CAPTION_CHARS )* SMALLCHAR_WIDTH + 1;
	g_arenaservers.filter.generic.top			= g_arenaservers.filter.generic.y;
	g_arenaservers.filter.generic.bottom		= g_arenaservers.filter.generic.y + SMALLCHAR_HEIGHT;

	y += 3 * SMALLCHAR_HEIGHT - 10;
	g_arenaservers.list.generic.type			= MTYPE_SCROLLLIST;
	g_arenaservers.list.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.list.generic.id				= ID_LIST;
	g_arenaservers.list.generic.callback		= ArenaServers_Event;
	g_arenaservers.list.generic.x				= 72;
	g_arenaservers.list.generic.y				= y;
	g_arenaservers.list.width					= MAX_LISTBOXWIDTH;
	g_arenaservers.list.height					= 12;
	g_arenaservers.list.scroll					= 12;
	g_arenaservers.list.generic.dblclick		= ArenaServers_Dblclick;
	g_arenaservers.list.itemnames				= (const char **)g_arenaservers.items;
	for( i = 0; i < MAX_LISTBOXITEMS; i++ ) {
		g_arenaservers.items[i] = g_arenaservers.table[i].buff;
	}

	g_arenaservers.mappic.generic.type		= MTYPE_BITMAP;
	g_arenaservers.mappic.generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	g_arenaservers.mappic.generic.x			= 72;
	g_arenaservers.mappic.generic.y			= 80;
	g_arenaservers.mappic.width				= 128;
	g_arenaservers.mappic.height			= 96;
	g_arenaservers.mappic.errorpic			= ART_UNKNOWNMAP;

	g_arenaservers.arrows.generic.type		= MTYPE_BITMAP;
	g_arenaservers.arrows.generic.name		= ART_ARROWS0;
	g_arenaservers.arrows.generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	g_arenaservers.arrows.generic.callback	= ArenaServers_Event;
	g_arenaservers.arrows.generic.x			= 512+48;
	g_arenaservers.arrows.generic.y			= 240-64+32;
	g_arenaservers.arrows.width				= 64;
	g_arenaservers.arrows.height			= 128;

	g_arenaservers.up.generic.type			= MTYPE_BITMAP;
	g_arenaservers.up.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_MOUSEONLY;
	g_arenaservers.up.generic.callback		= ArenaServers_Event;
	g_arenaservers.up.generic.id			= ID_SCROLL_UP;
	g_arenaservers.up.generic.x				= 512+48;
	g_arenaservers.up.generic.y				= 240-64+32;
	g_arenaservers.up.width					= 64;
	g_arenaservers.up.height				= 64;
	g_arenaservers.up.focuspic				= ART_ARROWS_UP;

	g_arenaservers.down.generic.type		= MTYPE_BITMAP;
	g_arenaservers.down.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_MOUSEONLY;
	g_arenaservers.down.generic.callback	= ArenaServers_Event;
	g_arenaservers.down.generic.id			= ID_SCROLL_DOWN;
	g_arenaservers.down.generic.x			= 512+48;
	g_arenaservers.down.generic.y			= 240+32;
	g_arenaservers.down.width				= 64;
	g_arenaservers.down.height				= 64;
	g_arenaservers.down.focuspic			= ART_ARROWS_DOWN;

	y = 378;
	g_arenaservers.status.generic.type		= MTYPE_TEXT;
	g_arenaservers.status.generic.x			= 320;
	g_arenaservers.status.generic.y			= y;
	g_arenaservers.status.string			= statusbuffer;
	g_arenaservers.status.style				= UI_CENTER|UI_SMALLFONT;
	g_arenaservers.status.color				= menu_text_color;

	y += SMALLCHAR_HEIGHT;
	g_arenaservers.statusbar.generic.type   = MTYPE_TEXT;
	g_arenaservers.statusbar.generic.x	    = 320;
	g_arenaservers.statusbar.generic.y	    = y;
	g_arenaservers.statusbar.string	        = "";
	g_arenaservers.statusbar.style	        = UI_CENTER|UI_SMALLFONT;
	g_arenaservers.statusbar.color	        = text_color_normal;

	g_arenaservers.remove.generic.type		= MTYPE_BITMAP;
	g_arenaservers.remove.generic.name		= ART_REMOVE0;
	g_arenaservers.remove.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	g_arenaservers.remove.generic.callback	= ArenaServers_Event;
	g_arenaservers.remove.generic.id		= ID_REMOVE;
	g_arenaservers.remove.generic.x			= 450;
	g_arenaservers.remove.generic.y			= 86;
	g_arenaservers.remove.width				= 96;
	g_arenaservers.remove.height			= 48;
	g_arenaservers.remove.focuspic			= ART_REMOVE1;

	g_arenaservers.back.generic.type		= MTYPE_BITMAP;
	g_arenaservers.back.generic.name		= ART_BACK0;
	g_arenaservers.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	g_arenaservers.back.generic.callback	= ArenaServers_Event;
	g_arenaservers.back.generic.id			= ID_BACK;
	g_arenaservers.back.generic.x			= 0;
	g_arenaservers.back.generic.y			= 480-64;
	g_arenaservers.back.width				= 128;
	g_arenaservers.back.height				= 64;
	g_arenaservers.back.focuspic			= ART_BACK1;

	g_arenaservers.specify.generic.type	    = MTYPE_BITMAP;
	g_arenaservers.specify.generic.name		= ART_SPECIFY0;
	g_arenaservers.specify.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	g_arenaservers.specify.generic.callback = ArenaServers_Event;
	g_arenaservers.specify.generic.id	    = ID_SPECIFY;
	g_arenaservers.specify.generic.x		= 128;
	g_arenaservers.specify.generic.y		= 480-64;
	g_arenaservers.specify.width  		    = 128;
	g_arenaservers.specify.height  		    = 64;
	g_arenaservers.specify.focuspic         = ART_SPECIFY1;

	g_arenaservers.refresh.generic.type		= MTYPE_BITMAP;
	g_arenaservers.refresh.generic.name		= ART_REFRESH0;
	g_arenaservers.refresh.generic.flags	= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	g_arenaservers.refresh.generic.callback	= ArenaServers_Event;
	g_arenaservers.refresh.generic.id		= ID_REFRESH;
	g_arenaservers.refresh.generic.x		= 256;
	g_arenaservers.refresh.generic.y		= 480-64;
	g_arenaservers.refresh.width			= 128;
	g_arenaservers.refresh.height			= 64;
	g_arenaservers.refresh.focuspic			= ART_REFRESH1;

	g_arenaservers.create.generic.type		= MTYPE_BITMAP;
	g_arenaservers.create.generic.name		= ART_CREATE0;
	g_arenaservers.create.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	g_arenaservers.create.generic.callback	= ArenaServers_Event;
	g_arenaservers.create.generic.id		= ID_CREATE;
	g_arenaservers.create.generic.x			= 384;
	g_arenaservers.create.generic.y			= 480-64;
	g_arenaservers.create.width				= 128;
	g_arenaservers.create.height			= 64;
	g_arenaservers.create.focuspic			= ART_CREATE1;

	g_arenaservers.go.generic.type			= MTYPE_BITMAP;
	g_arenaservers.go.generic.name			= ART_CONNECT0;
	g_arenaservers.go.generic.flags			= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	g_arenaservers.go.generic.callback		= ArenaServers_Event;
	g_arenaservers.go.generic.id			= ID_CONNECT;
	g_arenaservers.go.generic.x				= 640;
	g_arenaservers.go.generic.y				= 480-64;
	g_arenaservers.go.width					= 128;
	g_arenaservers.go.height				= 64;
	g_arenaservers.go.focuspic				= ART_CONNECT1;

	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.banner );

	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.master );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.gametype );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.sortkey );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.showfull);
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.showempty );

	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.filter );

	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.mappic );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.list );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.status );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.statusbar );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.arrows );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.up );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.down );

	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.remove );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.back );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.specify );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.refresh );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.create );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.go );

	ArenaServers_LoadFavorites();

	g_servertype = Com_Clamp( 0, 3, ui_browserMaster.integer );
	// hack to get rid of MPlayer stuff
	value = g_servertype;
	if (value >= 1)
		value--;
	g_arenaservers.master.curvalue = value;

	g_gametype = Com_Clamp( 0, GT_MAX_GAME_TYPE-1, ui_browserGameType.integer );
	g_arenaservers.gametype.curvalue = g_gametype;

	g_sortkey = Com_Clamp( 0, 4, ui_browserSortKey.integer );
	g_arenaservers.sortkey.curvalue = g_sortkey;

	g_fullservers = Com_Clamp( 0, 1, ui_browserShowFull.integer );
	g_arenaservers.showfull.curvalue = g_fullservers;

	g_emptyservers = Com_Clamp( 0, 1, ui_browserShowEmpty.integer );
	g_arenaservers.showempty.curvalue = g_emptyservers;
	
	// force to initial state and refresh
	type = g_servertype;
	g_servertype = -1;
	ArenaServers_SetType( type );

	trap_Cvar_Register(NULL, "debug_protocol", "", 0 );
}


/*
=================
ArenaServers_Cache
=================
*/
void ArenaServers_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	trap_R_RegisterShaderNoMip( ART_CREATE0 );
	trap_R_RegisterShaderNoMip( ART_CREATE1 );
	trap_R_RegisterShaderNoMip( ART_SPECIFY0 );
	trap_R_RegisterShaderNoMip( ART_SPECIFY1 );
	trap_R_RegisterShaderNoMip( ART_REFRESH0 );
	trap_R_RegisterShaderNoMip( ART_REFRESH1 );
	trap_R_RegisterShaderNoMip( ART_CONNECT0 );
	trap_R_RegisterShaderNoMip( ART_CONNECT1 );
	trap_R_RegisterShaderNoMip( ART_ARROWS0  );
	trap_R_RegisterShaderNoMip( ART_ARROWS_UP );
	trap_R_RegisterShaderNoMip( ART_ARROWS_DOWN );
	trap_R_RegisterShaderNoMip( ART_UNKNOWNMAP );
}


/*
=================
UI_ArenaServersMenu
=================
*/
void UI_ArenaServersMenu( void ) {
	ArenaServers_MenuInit();
	UI_PushMenu( &g_arenaservers.menu );
}						  
