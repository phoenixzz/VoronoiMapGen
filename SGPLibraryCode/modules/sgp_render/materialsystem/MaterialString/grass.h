char Material_grass_String[] = 
	"// transparent grass rendering			\n"\
	"pass									\n"\
	"{										\n"\
	"	cull_mode = none					\n"\
	"	depth_write = false					\n"\
	"	alpha_blend							\n"\
	"	{									\n"\
	"		src = srcalpha					\n"\
	"		dst = invsrcalpha				\n"\
	"	}									\n"\
	"										\n"\
	"}										\n"\
	"";