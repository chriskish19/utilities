/****************************************************************
	Header: test.go.hpp

	Purpose: testing go function, handles all tests

****************************************************************/

#pragma once

// includes all
#include "main_program_lol.blocks.exe/pch/pch.hpp"

// tests
#include "main_program_lol.blocks.exe/testing/api/test.api.hpp"


namespace testing {
	errors::codes basic_tests();

	errors::codes draw_tests();
}