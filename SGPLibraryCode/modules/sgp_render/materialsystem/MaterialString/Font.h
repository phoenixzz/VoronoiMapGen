char Material_Font_String[] = 
	"// Font rendering				\n"\
	"pass							\n"\
	"{								\n"\
	"	depth_write = false			\n"\
	"	depth_test = false			\n"\
	"	cull_mode = none			\n"\
	"	alpha_blend					\n"\
	"	{							\n"\
	"		op = add				\n"\
	"		src = srcalpha			\n"\
	"		dst = invsrcalpha		\n"\
	"	}							\n"\
	"}								\n"\
	"";