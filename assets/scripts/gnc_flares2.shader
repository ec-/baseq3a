gnc_line1
{
	nomipmaps
	nopicmip
	sort nearest
		{
			clampmap textures/gnc_sfx/line1.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
			tcmod transform 1 0 0 32 0 -15.5
		}
}

gnc_line2
{
	nomipmaps
	nopicmip
	sort nearest
		{
			clampmap textures/gnc_sfx/line2.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
			tcmod transform 1 0 0 8 0 -3.5
		}
}

gnc_flare4
{
	sort nearest
		{
			map textures/gnc_sfx/flare4.tga
			blendfunc gl_src_alpha gl_one
			rgbgen vertex
			alphagen vertex
		}
}

