/****************************************************************
	Header: test_settings.hpp

	Purpose: controls tests throughout the program.
			 

****************************************************************/


// just a nice readable thing
#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

// Control Main entry point for whole application
#define USE_MAIN_ENTRY_CPP OFF
#if USE_MAIN_ENTRY_CPP
#define USE_TEST_API OFF
#else
#define USE_TEST_API ON
#endif

// testing drawing primitives
#define TESTING_SIMPLE_DRAW ON

// Testing code
#define TESTING ON