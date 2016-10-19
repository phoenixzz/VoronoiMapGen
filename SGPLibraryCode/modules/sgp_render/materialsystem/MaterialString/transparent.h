char Material_transparent_String[] = 
	"// transparent rendering			\n"\
	"pass								\n"\
	"{									\n"\
	"	depth_write = false				\n"\
	"	alpha_blend						\n"\
	"	{								\n"\
	"		src = srcalpha				\n"\
	"		dst = invsrcalpha			\n"\
	"	}								\n"\
	"									\n"\
	"}									\n"\
	"";