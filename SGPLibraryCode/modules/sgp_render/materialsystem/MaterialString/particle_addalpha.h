char Material_particleaddalpha_String[] =
	"// particle rendering material		\n"\
	"pass								\n"\
	"{									\n"\
	"	depth_write = false				\n"\
	"	alpha_blend						\n"\
	"	{								\n"\
	"		src = one					\n"\
	"		dst = invsrcalpha			\n"\
	"	}								\n"\
	"									\n"\
	"}									\n"\
	"";