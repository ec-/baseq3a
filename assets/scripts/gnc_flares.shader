gnc_fflare1
{
	sort nearest
		{
			map textures/gnc_sfx/fire_glare1.tga
			blendfunc gl_src_alpha gl_one
			rgbgen vertex
			alphagen vertex
		}
}

gnc_flare0
{
	sort nearest
		{
			map textures/gnc_sfx/flare0.tga
			blendfunc gl_src_alpha gl_one
			rgbgen vertex
			alphagen vertex
		}
}

gnc_flare1
{
	sort nearest
		{
			map textures/gnc_sfx/flare1.tga
			blendfunc gl_src_alpha gl_one
			rgbgen vertex
			alphagen vertex
		}
}

gnc_flare2
{
	sort nearest
		{
			map textures/gnc_sfx/flare2.tga
			blendfunc gl_src_alpha gl_one
			rgbgen vertex
			alphagen vertex
		}
}

gnc_flare3
{
	sort nearest
		{
			map textures/gnc_sfx/flare3.tga
			blendfunc gl_src_alpha gl_one
			rgbgen vertex
			alphagen vertex
		}
}

gnc_disc0
{
	sort nearest
		{
			map textures/gnc_sfx/disc0.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_arc1
{
	sort nearest
		{
			map textures/gnc_sfx/arc1.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_disc1
{
	sort nearest
		{
			map textures/gnc_sfx/disc1.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_disc2
{
	sort nearest
		{
			map textures/gnc_sfx/disc2.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_ring0
{
	sort nearest
		{
			map textures/gnc_sfx/ring0.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_ring1
{
	sort nearest
		{
			map textures/gnc_sfx/ring1.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_ring2
{
	sort nearest
		{
			map textures/gnc_sfx/ring2.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_ring3
{
	sort nearest
		{
			map textures/gnc_sfx/ring3.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_glare0
{
	sort nearest
		{
			map textures/gnc_sfx/glare0.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_glare1
{
	sort nearest
		{
			map textures/gnc_sfx/glare1.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_line0
{
	nomipmaps
	nopicmip
	sort nearest
		{
			map textures/gnc_sfx/line0.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_line1
{
	nomipmaps
	nopicmip
	sort nearest
		{
			map textures/gnc_sfx/line1.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_line2
{
	nomipmaps
	nopicmip
	sort nearest
		{
			map textures/gnc_sfx/line2.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_star0
{
	sort nearest
		{
			map textures/gnc_sfx/star0.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_penta0
{
	sort nearest
		{
			map textures/gnc_sfx/penta0.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_hexa0
{
	sort nearest
		{
			map textures/gnc_sfx/hexa0.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

gnc_octa0
{
	sort nearest
		{
			map textures/gnc_sfx/octa0.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen vertex
		}
}

// unique for proto_redlight
gnc_flicker0
{
	sort nearest
		{
			map textures/gnc_sfx/disc0_red.tga
			blendfunc gl_src_alpha gl_one
			alphagen vertex
			rgbgen wave square .5 .5 0 1
			
		}
}

// unique for xconduit
gnc_flicker1
{
	sort nearest
		// random shit below ;)
		{
			map textures/gnc_sfx/glare0.tga
			blendfunc gl_src_alpha gl_one
			rgbGen wave noise .3 .4 .2 10
			alphagen vertex
		}
		{
			map textures/gnc_sfx/glare0.tga
			blendfunc gl_src_alpha gl_one
			rgbGen wave noise .4 .5 .1 9
			alphagen vertex
		}
}
