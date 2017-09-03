lfeditorcursor
{
	{
		map $whiteimage
		blendfunc gl_one gl_zero
		rgbgen entity
	}
}

lfeditorline
{
	{
		map sprites/bfglfline.tga
		blendfunc add
	}
}

lfeditorspot
{
	nopicmip
	{
		map sprites/spot.tga
		blendfunc gl_src_alpha gl_one_minus_src_alpha
		rgbgen vertex
	}
}


bfgLFStar
{
	sort nearest
	{
		map sprites/bfglfstar.tga
		blendfunc gl_src_alpha gl_one
		alphagen vertex
	}
}

bfgLFGlare
{
	sort nearest
	{
		map sprites/bfglfglare.tga
		blendfunc gl_src_alpha gl_one
		alphagen vertex
		rgbgen vertex
	}
}

bfgLFDisc
{
	sort nearest
	{
		map sprites/bfglfdisc.tga
		blendfunc gl_src_alpha gl_one
		alphagen vertex
		rgbgen vertex
	}
}

bfgLFRing
{
	sort nearest
	{
		map sprites/bfglfring.tga
		blendfunc gl_src_alpha gl_one
		alphagen vertex
		rgbgen vertex
	}
}

bfgLFLine
{
	nomipmaps
	nopicmip
	sort nearest
	{
		clampmap sprites/bfglfline.tga
		blendfunc gl_src_alpha gl_one
		alphagen vertex
		rgbgen vertex
		tcmod transform 1 0 0 64 0 -31.5
	}
}

lfdisc1
{
	sort nearest
	{
		map sprites/lfdisc1.tga
		blendfunc gl_src_alpha gl_one
		alphagen vertex
		rgbgen vertex
	}
}

lfdisc2
{
	sort nearest
	{
		map sprites/lfdisc2.tga
		blendfunc gl_src_alpha gl_one
		alphagen vertex
		rgbgen vertex
	}
}

lfarc1
{
	sort nearest
	{
		map sprites/lfarc1.tga
		blendfunc gl_src_alpha gl_one
		alphagen vertex
		rgbgen vertex
	}
}


lfring1
{
	sort nearest
	{
		map sprites/lfring1.tga
		blendfunc gl_src_alpha gl_one
		alphagen vertex
		rgbgen vertex
	}
}
