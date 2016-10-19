#ifndef __SGP_RENDERSTAGES_HEADER__
#define __SGP_RENDERSTAGES_HEADER__

	//! An enum for all types of drivers the SGP Engine supports.
	enum SGP_RENDER_STAGE
	{
		//! Default Normal Stage
		/** Performs rendering for the default normal render stage */
		SGPRS_NORMAL = 0,

		//! Water Reflection Stage
		/** Performs rendering for water reflection render stage */
		SGPRS_WATERREFLECTION,

		//! No stage, just for counting the elements
		SGPRS_COUNT
	};

#endif		// __SGP_RENDERSTAGES_HEADER__