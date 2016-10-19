char Material_particleaddalpha_Line_String[] =
	"// particle linerendering material		\n"\
	"pass									\n"\
	"{										\n"\
	"	depth_write = false					\n"\
	"	line_drawing = true					\n"\
	"	cull_mode = none					\n"\
	"	alpha_blend							\n"\
	"	{									\n"\
	"		src = one						\n"\
	"		dst = invsrcalpha				\n"\
	"	}									\n"\
	"										\n"\
	"}										\n"\
	"";