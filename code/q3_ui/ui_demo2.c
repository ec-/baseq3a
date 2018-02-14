// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

DEMOS MENU

=======================================================================
*/


#include "ui_local.h"
#include "../game/q_shared.h"


#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"	
#define ART_GO0				"menu/art/play_0"
#define ART_GO1				"menu/art/play_1"
#define ART_ARROWS_VERT		"menu/art/arrows_vert_0"
#define ART_ARROWS_UP		"menu/art/arrows_vert_top"
#define ART_ARROWS_DOWN		"menu/art/arrows_vert_bot"

#define UI_DEMO_LENGTH      64
#define UI_MAX_DEMOS        1024
#define UI_MAX_ITEMS        18
#define NAMEBUFSIZE         65536

#define ID_BACK				10
#define ID_GO				11
#define ID_LIST				12
#define ID_UP				13
#define ID_DOWN				14
#define ID_TIMEDEMO			15
#define ID_SORT				16
#define ID_FILTER			17


#define ARROWS_WIDTH		48
#define ARROWS_HEIGHT		128
#define ARROWS_TOP			240+4.75f
#define ARROWS_LEFT			512+64

typedef struct {
	menuframework_s	menu;

	menutext_s		banner;

	menulist_s		list;
	menulist_s		sort;

	menubitmap_s	arrows;
	menubitmap_s	up;
	menubitmap_s	down;
	menubitmap_s	back;
	menubitmap_s	go;
	menuradiobutton_s	timedemo;

	menufield_s		filter;

	int				numDemos;
	qboolean		canPlay;
	char			*itemname[UI_MAX_DEMOS];
	char			namefilter[MAX_EDIT_LINE];
} demos_t;

static demos_t	s_demos;

typedef struct {
	char *file_name;
	int	 file_nlen;
	char file_type;
} demo_entry_t;

static demo_entry_t dentry[UI_MAX_DEMOS];	// will be filled by UI_DemosReadDir()
static demo_entry_t *dptr[UI_MAX_DEMOS];	// used for sorting
static int	num_files;
static char	dir[256] = { "demos" }; // current directory
static int	dirlevel = 0;			// directory level
static int	listpos[64][2];			// saved list positions( curvalue, top )
static char	buffer[NAMEBUFSIZE];	// filename buffer
static char	show_names[UI_MAX_DEMOS][UI_DEMO_LENGTH*3];

static const char* sorttype[] = {
	"No",			
	"Name, asc",
	"Name, desc",
	0
};

static void UI_DemosFillList( void );

/*
=================
Demos_DrawFilter
=================
*/
static void Demos_DrawFilter( void *self ) {
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
#if 0
	UI_FillRect( f->generic.left, f->generic.top, 
		f->generic.right-f->generic.left, 
		f->generic.bottom-f->generic.top, 
		text_color_disabled );
#endif
	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;

	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawString( basex, y, "Filter:", style, color );

	basex += 64;
	//y += PROP_HEIGHT;
	txt = f->field.buffer;
	color = g_color_table[ColorIndex(COLOR_WHITE)];
	x = basex;

	while ( (c = *txt) != 0 ) {
		UI_DrawChar( x, y, c, style, color );
		txt++;
		x += SMALLCHAR_WIDTH;
	}

	if ( strcmp( s_demos.namefilter, f->field.buffer) ) {
		strcpy( s_demos.namefilter, f->field.buffer );
		UI_DemosFillList();
	}

	// draw cursor if we have focus
	if( focus ) {
		if ( trap_Key_GetOverstrikeMode() ) {
			c = 11;
		} else {
			c = 10;
		}

		style &= ~UI_PULSE;
		style |= UI_BLINK;

		UI_DrawChar( basex + f->field.cursor * SMALLCHAR_WIDTH, y, c, style, color_white );
	}
}


static void UI_DemosSavePosition( void ) {
	if ( dirlevel < ARRAY_LEN( listpos ) ) {
		listpos[dirlevel][0] = s_demos.list.curvalue;
		listpos[dirlevel][1] = s_demos.list.top;
	}
}


static void UI_DemosRestorePosition( void ) {
	if ( dirlevel < ARRAY_LEN( listpos ) ) {
		s_demos.list.curvalue = listpos[dirlevel][0];
		s_demos.list.top = listpos[dirlevel][1];
	}
}


/*
===============
UI_DemosReadDir
===============
*/
static void UI_DemosReadDir( void ) 
{
	char	extension[32], *s;
	int		i, len, n, c, m;

	c = 0; // count of valid entries

	// reserve room for ".."
	if ( dirlevel > 0 )
		c++;

	// get directories first
	n = trap_FS_GetFileList( dir, "/", buffer, sizeof( buffer ) );
	if ( n > UI_MAX_DEMOS )
		n = UI_MAX_DEMOS;
	s = buffer;

	for ( i = 0; i < n; i++ ) {
		len = strlen( s );
		// don't and "." and ".." entries for demo root dir
		if ( !s[0] || ( s[0]== '.' && !s[1] ) || ( !strcmp( s, ".." ) && dirlevel == 0 ) ) {
			s += len + 1;
			continue;
		}
		if ( !strcmp( s, ".." ) ) {
			dentry[0].file_type = 2;
			dentry[0].file_name = s;
			dentry[0].file_nlen = len;
		} else {
			dentry[c].file_type = 1;
			dentry[c].file_name = s;
			dentry[c].file_nlen = len;
			c++;
		}
		s += len + 1;
	}
	num_files = c;
	len = sizeof(buffer) - (s - buffer) - 1;
	if ( len > 2  && num_files < UI_MAX_DEMOS-1 ) {
		// count regular files
		m = trap_FS_GetFileList( dir, "dm_??", s, len ); // try to perform pattern match in first place
		if ( !m ) {
			Com_sprintf( extension, sizeof( extension ), "dm_%d", (int)trap_Cvar_VariableValue( "protocol" ) );
			m = trap_FS_GetFileList( dir, extension, s, len );
		}
		if ( num_files + m > UI_MAX_DEMOS ) 
			m = UI_MAX_DEMOS - n;
		for ( i = 0; i < m; i++, c++ ) {
			len = strlen( s );
			dentry[c].file_name = s;
			dentry[c].file_nlen = len;
			dentry[c].file_type = 0;
			s += len + 1;
		}
		num_files += m;
	}
	s_demos.numDemos = num_files;
}


/*
===============
UI_UpdateDirectory
===============
*/
static qboolean UI_UpdateDirectory( char *name ) {
	char *s;

	s_demos.namefilter[0] = '\0';
	s_demos.filter.field.buffer[0] = '\0';
	s_demos.filter.field.cursor = 0;

	if ( !name || !*name ) {
		s = Q_strrchr( dir, '/' );
		if ( !s || s == dir ) {
			dirlevel = 0;
			return qfalse;
		} else {
			*s = '\0';
			s = Q_strrchr( dir, '/' );
			if ( dirlevel > 0 )
				dirlevel--;
			if ( !s || s == dir )
				dirlevel = 0;
			return qtrue;
		}		
	} else {
		Q_strcat( dir, sizeof( dir ), "/" );
		Q_strcat( dir, sizeof( dir ), name );
		UI_DemosSavePosition();
		dirlevel++;
		return qtrue;
	}
}


/*
===============
UI_UpdateDirectory
===============
*/
static void UI_DemoClick( void ) {
	demo_entry_t *d;
	char *s, *r;

	if ( !s_demos.numDemos || !s_demos.canPlay )
		return;

	d = dptr[ s_demos.list.curvalue ];
	if ( !d )
		return;

	if ( d->file_type > 0 ) {
		if ( !strcmp( d->file_name, ".." ) ) {
			UI_UpdateDirectory( NULL );
			UI_DemosReadDir();
			UI_DemosFillList();
			UI_DemosRestorePosition();
		} else {
			UI_UpdateDirectory( d->file_name );
			UI_DemosReadDir();
			UI_DemosFillList();
		}
	} else {
		UI_ForceMenuOff ();
		s = strchr( dir, '/' );
		if ( s ) {
			r = va( "demo \"%s/%s\"\n", s + 1, d->file_name );
		} else
			r = va( "demo \"%s\"\n", d->file_name );
		trap_Cmd_ExecuteText( EXEC_APPEND, r );
	}
}


/*
===============
Demos_MenuEvent
===============
*/
static void Demos_MenuEvent( void *ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_GO:
		UI_DemoClick();
		break;

	case ID_BACK:
		UI_PopMenu();
		break;

	case ID_UP:
		ScrollList_Key( &s_demos.list, K_UPARROW );
		break;

	case ID_DOWN:
		ScrollList_Key( &s_demos.list, K_DOWNARROW );
		break;

	case ID_TIMEDEMO:
		if ( s_demos.timedemo.curvalue )
			trap_Cvar_Set( "timedemo", "1" );
		else
			trap_Cvar_Set( "timedemo", "0" );
		break;

	case ID_SORT:
		trap_Cvar_Set( "ui_demoSort", va( "%i", s_demos.sort.curvalue ) );
		UI_DemosFillList();
		break;
	}
}

static void UI_DemosDblclick( void *ptr ) {
	UI_DemoClick();
}

#if 0
static sfxHandle_t UI_DemosKeydown( void *ptr, int key ) {
/*	
	if ( key == K_ENTER || key == K_KP_ENTER ) {
		UI_DemoClick();
		return menu_move_sound;
	}
	if ( key == K_BACKSPACE ) {
		if ( UI_UpdateDirectory( NULL ) ) {
			UI_DemosReadDir();
			UI_DemosFillList();
			UI_DemosRestorePosition();
		}
		return menu_move_sound;
	}
	if ( key == '/' ) {
		Menu_SetCursorToItem( (menuframework_s *)&s_demos, &s_demos.filter );
		s_demos.filter.field.skipKey = qtrue;
		return menu_null_sound;
	}
*/
	return 0;

}
#endif

/*
=================
UI_DemosMenu_Key
=================
*/
static sfxHandle_t UI_DemosMenu_Key( int key ) {
	menucommon_s	*item;

	item = Menu_ItemAtCursor( &s_demos.menu );

	if ( item == (menucommon_s *)&s_demos.list ) {
		if ( key == K_ENTER || key == K_KP_ENTER ) {
			UI_DemoClick();
			return menu_in_sound;
		}
		if ( key == K_BACKSPACE ) {
			if ( UI_UpdateDirectory( NULL ) ) {
				UI_DemosReadDir();
				UI_DemosFillList();
				UI_DemosRestorePosition();
			}
			return menu_in_sound;
		}
		if ( key == '/' ) {
			Menu_SetCursorToItem( (menuframework_s *)&s_demos, &s_demos.filter );
			s_demos.filter.field.skipKey = qtrue;
			return menu_in_sound;
		}
	}

	return Menu_DefaultKey( &s_demos.menu, key );
}


// calculate length difference for color strings
int	UI_cstrdiff( char * str ) {
	int diff;
	if ( !str )
		return 0;
	diff = 0;
	while ( *str ) {
		if ( *str == Q_COLOR_ESCAPE && str[1] && str[2] ) {
			diff += 2;
			str += 2;
		} else
			str++;
	}
	return diff;
}

int UI_cstricmp( const char *src, const char *dst ) {
    int ret = 0;
    int c1 = 0, c2 = 0;
	unsigned char	ch1, ch2;
    do {
        if ( *src == '^' && src[1] ) {
            c1 = src[1];
            src += 2;
            continue;
        } 
                
        if ( *dst == '^' && dst[1] ) {
            c2 = dst[1];
            dst += 2;
            continue;
        } 
		
		ch1 = *src;
		ch2 = *dst;

		if ( ch1 >= 'a' && ch1 <= 'z' ) 
			ch1 = ch1 - 'a' + 'A';
		if ( ch2 >= 'a' && ch2 <= 'z' ) 
			ch2 = ch2 - 'a' + 'A';

        ret = ch1 - ch2;
        if ( !ret )
            ret = c1 - c2;

        dst++;
        if ( ret || !*dst )
            break;

        src++;

    } while ( 1 );

    if ( ret < 0 )
        ret = -1;
    else if ( ret > 0 )
        ret = 1;

    return( ret );
}


static int compare_entries( demo_entry_t * a, demo_entry_t *b, int mode ) 
{
	int t;
	t = b->file_type - a->file_type;
	if ( !t )
		if ( mode == 1 ) 
			return UI_cstricmp( a->file_name, b->file_name );
		else
			return -UI_cstricmp( a->file_name, b->file_name );

	else 
		return t;
}


static void UI_demosort( demo_entry_t **a, int n, int mode ) 
{
    demo_entry_t * tmp;
    int i = 0, j = n;
    demo_entry_t *m = a[ n>>1 ];
    do 
    {
        while ( compare_entries( a[i], m, mode ) < 0 ) i++;
        while ( compare_entries( a[j], m, mode ) > 0 ) j--;

		if ( i <= j ) 
        {
            tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
            i++;
            j--;
        }
    } 
    while ( i <= j );
    if ( j > 0 ) UI_demosort( a, j, mode );
    if ( n > i ) UI_demosort( a+i, n-i, mode );
}


static void UI_DemosFillList( void ) {

	int		i, diff, sort, len;
	char	matchname[256];
	demo_entry_t *d;

	if ( !s_demos.numDemos )
		return;

	sort = s_demos.sort.curvalue;

	s_demos.list.numitems = 0;

	for ( i = 0; i < s_demos.numDemos; i++ ) {
		len = dentry[i].file_nlen;

		if ( s_demos.namefilter[0] && strcmp( dentry[i].file_name, ".." ) ) {
			strcpy( matchname, dentry[i].file_name );

			// strip extension
			if ( !Q_stricmp( matchname + len - 6, ".dm_68" ) )
				matchname[ len-6 ] = '\0';
		
			BG_StripColor( matchname );
			if ( !Q_stristr( matchname, s_demos.namefilter ) ) {
				continue;
			}
		}
		dptr[ s_demos.list.numitems++ ] = &dentry[ i ];
		if ( s_demos.list.numitems >= UI_MAX_DEMOS )
			break;
	}

	if ( sort && s_demos.list.numitems > 1 )
		UI_demosort( dptr, s_demos.list.numitems - 1, sort );

	for ( i = 0; i < s_demos.list.numitems; i++ ) {
		d = dptr[ i ];
		s_demos.list.itemnames[i] = show_names[i];

		Q_strncpyz( show_names[i], d->file_name, sizeof( show_names[i] ) );
		if ( d->file_type > 0 ) {
			Q_strcat( show_names[i], sizeof( show_names[0] ), "^7/" );
		}

		len = d->file_nlen;
		// strip extension
		if ( !Q_stricmp( show_names[i] +  len - 6,".dm_68" ) ) {
			memset( show_names[i] +  len - 6, ' ', 6 );
			len -= 6;
		}
		
		diff = UI_cstrdiff ( show_names[i] );

		show_names[i][s_demos.list.width-1+diff] = '\0';

		//Q_strupr( show_names[i] );

		if ( len - diff > s_demos.list.width - 1 ) {
			strcpy( &show_names[i][s_demos.list.width-1+diff], "^7>" );
		}
	}

	s_demos.list.curvalue = 0;
	s_demos.list.top = 0;

	if ( !s_demos.list.numitems ) {
		s_demos.list.itemnames[i] = show_names[0];
		strcpy( show_names[0], "No files matching your request." );
		s_demos.list.numitems = 1;
		s_demos.list.curvalue = 1; // will remove selection
		s_demos.canPlay = qfalse;
		//degenerate case, not selectable
		s_demos.list.generic.flags = QMF_INACTIVE;
		s_demos.go.generic.flags = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_INACTIVE ;
	} else {
		s_demos.canPlay = qtrue;
		s_demos.list.generic.flags = QMF_PULSEIFFOCUS;
		s_demos.go.generic.flags = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	}

	if ( s_demos.list.numitems > 1 ) {
		s_demos.up.generic.flags = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_MOUSEONLY;
		s_demos.down.generic.flags = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_MOUSEONLY;
	} else {
		s_demos.up.generic.flags = QMF_INACTIVE;
		s_demos.down.generic.flags = QMF_INACTIVE;
	}
}


/*
===============
Demos_MenuInit
===============
*/
static void Demos_MenuInit( void ) {

	memset( &s_demos, 0 ,sizeof(demos_t) );
	s_demos.menu.key = UI_DemosMenu_Key;

	Demos_Cache();

	s_demos.menu.fullscreen = qtrue;
	s_demos.menu.wrapAround = qtrue;

	s_demos.banner.generic.type		= MTYPE_TEXT;
	s_demos.banner.generic.x		= 320;
	s_demos.banner.generic.y		= 28;
	s_demos.banner.string			= dir;
	//s_demos.banner.color			= color_white;
	s_demos.banner.color			= text_color_normal;
	s_demos.banner.style			= UI_CENTER;

	s_demos.arrows.generic.type		= MTYPE_BITMAP;
	s_demos.arrows.generic.name		= ART_ARROWS_VERT;
	s_demos.arrows.generic.flags	= QMF_INACTIVE;
	s_demos.arrows.generic.x		= ARROWS_LEFT;
	s_demos.arrows.generic.y		= ARROWS_TOP-ARROWS_HEIGHT/2;
	s_demos.arrows.width			= ARROWS_WIDTH;
	s_demos.arrows.height			= ARROWS_HEIGHT;

	s_demos.up.generic.type			= MTYPE_BITMAP;
	s_demos.up.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_MOUSEONLY;
	s_demos.up.generic.x			= ARROWS_LEFT;
	s_demos.up.generic.y			= ARROWS_TOP-ARROWS_HEIGHT/2;
	s_demos.up.generic.id			= ID_UP;
	s_demos.up.generic.callback		= Demos_MenuEvent;
	s_demos.up.width				= ARROWS_WIDTH;
	s_demos.up.height				= ARROWS_HEIGHT/2;
	s_demos.up.focuspic				= ART_ARROWS_UP;

	s_demos.down.generic.type		= MTYPE_BITMAP;
	s_demos.down.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_MOUSEONLY;
	s_demos.down.generic.x			= ARROWS_LEFT;
	s_demos.down.generic.y			= ARROWS_TOP;
	s_demos.down.generic.id			= ID_DOWN;
	s_demos.down.generic.callback	= Demos_MenuEvent;
	s_demos.down.width				= ARROWS_WIDTH;
	s_demos.down.height				= ARROWS_HEIGHT/2;
	s_demos.down.focuspic			= ART_ARROWS_DOWN;

	s_demos.back.generic.type		= MTYPE_BITMAP;
	s_demos.back.generic.name		= ART_BACK0;
	s_demos.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_demos.back.generic.id			= ID_BACK;
	s_demos.back.generic.callback	= Demos_MenuEvent;
	s_demos.back.generic.x			= 0;
	s_demos.back.generic.y			= 480-64;
	s_demos.back.width				= 128;
	s_demos.back.height				= 64;
	s_demos.back.focuspic			= ART_BACK1;

	s_demos.go.generic.type			= MTYPE_BITMAP;
	s_demos.go.generic.name			= ART_GO0;
	s_demos.go.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_demos.go.generic.id			= ID_GO;
	s_demos.go.generic.callback		= Demos_MenuEvent;
	s_demos.go.generic.x			= 640;
	s_demos.go.generic.y			= 480-64;
	s_demos.go.width				= 128;
	s_demos.go.height				= 64;
	s_demos.go.focuspic				= ART_GO1;

	s_demos.timedemo.generic.type		= MTYPE_RADIOBUTTON;
	s_demos.timedemo.generic.name		= "Time Demo:";
	s_demos.timedemo.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_demos.timedemo.generic.callback	= Demos_MenuEvent;
	s_demos.timedemo.generic.id			= ID_TIMEDEMO;
	s_demos.timedemo.generic.x			= 320+24-4;
	s_demos.timedemo.generic.y			= 480-42;

	if ( (int)trap_Cvar_VariableValue("timedemo" ) )
		s_demos.timedemo.curvalue = qtrue;
	else
		s_demos.timedemo.curvalue = qfalse;

	s_demos.sort.generic.type		= MTYPE_SPINCONTROL;
	s_demos.sort.generic.name		= "Sort:";
	s_demos.sort.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_demos.sort.generic.callback	= Demos_MenuEvent;
	s_demos.sort.generic.id			= ID_SORT;
	//s_demos.sort.generic.x			= 332;
	//s_demos.sort.generic.y			= 406;
	s_demos.sort.generic.x			= 485;
	s_demos.sort.generic.y			= 72;
	s_demos.sort.itemnames			= sorttype;
	s_demos.sort.curvalue			= (int)trap_Cvar_VariableValue( "ui_demoSort" );
	
	if ( s_demos.sort.curvalue < 0 || s_demos.sort.curvalue > 2 )
		 s_demos.sort.curvalue = 0;

	s_demos.filter.generic.type			= MTYPE_FIELD;
	s_demos.filter.generic.flags		= QMF_NODEFAULTINIT;
	s_demos.filter.generic.ownerdraw	= Demos_DrawFilter;
	s_demos.filter.generic.id			= ID_FILTER;
	s_demos.filter.field.widthInChars	= 37;
	s_demos.filter.field.maxchars		= 37;
	s_demos.filter.generic.x			= 62;
	s_demos.filter.generic.y			= 72;
	s_demos.filter.generic.left			= 62;
	//s_demos.filter.generic.right		= 640 - 64;
	s_demos.filter.generic.right		= 430;
	s_demos.filter.generic.top			= 66;
	s_demos.filter.generic.bottom		= 94;

	s_demos.list.generic.type		= MTYPE_SCROLLLIST;
	s_demos.list.generic.flags		= QMF_PULSEIFFOCUS;
	s_demos.list.generic.callback	= Demos_MenuEvent;
	s_demos.list.generic.id			= ID_LIST;
	s_demos.list.generic.x			= (640-UI_DEMO_LENGTH*SMALLCHAR_WIDTH)/2;
	s_demos.list.generic.y			= (480-UI_MAX_ITEMS*SMALLCHAR_HEIGHT)/2;
	s_demos.list.width				= UI_DEMO_LENGTH;
	s_demos.list.height				= UI_MAX_ITEMS;
	s_demos.list.columns			= 1;
	s_demos.list.scroll				= s_demos.list.height - 1;
	s_demos.list.generic.dblclick	= UI_DemosDblclick;

	s_demos.list.itemnames			= (const char **)s_demos.itemname;

	UI_DemosReadDir();

	if ( !s_demos.numDemos && dirlevel == 0 ) {
		strcpy( buffer, "No Demos Found." );
		s_demos.list.numitems = 1;

		//degenerate case, not selectable
		s_demos.go.generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		s_demos.up.generic.flags = QMF_INACTIVE;
		s_demos.down.generic.flags = QMF_INACTIVE;
	} else {
		UI_DemosFillList();
	}

	Menu_AddItem( &s_demos.menu, &s_demos.banner );
	Menu_AddItem( &s_demos.menu, &s_demos.list );
	Menu_AddItem( &s_demos.menu, &s_demos.arrows );
	Menu_AddItem( &s_demos.menu, &s_demos.up );
	Menu_AddItem( &s_demos.menu, &s_demos.down );
	Menu_AddItem( &s_demos.menu, &s_demos.back );
	Menu_AddItem( &s_demos.menu, &s_demos.go );
	Menu_AddItem( &s_demos.menu, &s_demos.timedemo );
	Menu_AddItem( &s_demos.menu, &s_demos.sort );
	Menu_AddItem( &s_demos.menu, &s_demos.filter );
}

/*
=================
Demos_Cache
=================
*/
void Demos_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	trap_R_RegisterShaderNoMip( ART_GO0 );
	trap_R_RegisterShaderNoMip( ART_GO1 );
	trap_R_RegisterShaderNoMip( ART_ARROWS_VERT );
	trap_R_RegisterShaderNoMip( ART_ARROWS_UP );
	trap_R_RegisterShaderNoMip( ART_ARROWS_DOWN );
}

/*
===============
UI_DemosMenu
===============
*/
void UI_DemosMenu( void ) {
	Demos_MenuInit();
	UI_PushMenu( &s_demos.menu );
}
