/**********************************************************/
//
// File: names.hpp
//
// Purpose: easy file move/name changes in one location 
//
// Project: core
//
/**********************************************************/

#pragma once

// dependencies
#define CORE_STL_PRIVATE_INCLUDE_PATH			CORE_stl_includes_hpp				// the entire stl library
#define CORE_STL_INCLUDE_PATH					CORE_stl_hpp						// specific version include stl library
#define CORE_SDL_INCLUDE_PATH					CORE_all_sdl_hpp					// all sdl includes
#define CORE_ASSIMP_INCLUDE_PATH				CORE_all_assimp_hpp					// all assimp includes
#define CORE_IMGUI_INCLUDE_PATH					CORE_all_imgui_hpp					// all imgui includes
#define CORE_GLM_INCLUDE_PATH					CORE_all_glm_hpp					// all glm includes
#define CORE_SPD_INCLUDE_PATH					CORE_all_spd_hpp					// all spd log includes
#define CORE_ALL_DEPENDS_INCLUDE_PATH			CORE_all_depends_hpp				// all dependencies

// api
#define CORE_API_INCLUDE_PATH					CORE_core_api_hpp					// helper functions

// errors
#define CORE_CODES_INCLUDE_PATH					CORE_codes_hpp						// codes

// classes
#define CORE_DATA_INCLUDE_PATH					CORE_data_structs_hpp				// all kinds of objects