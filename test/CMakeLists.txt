# Set up dependencies
find_package(GTEST QUIET)
include_directories(${CMAKE_SOURCE_DIR})

# Convenience macro
macro(add_gtest TESTNAME)
	add_executable(${TESTNAME} ${TESTNAME}.cpp )
	if(${ARGV1}) 
	message("i depend! ${TESTNAME}")		
set_target_properties(${TESTNAME} PROPERTIES LINK_DEPENDS ${ARGV1})
	endif()
	target_link_libraries(${TESTNAME} ${GTEST_LIBRARIES} fivehundredcore)
	add_custom_command(TARGET ${TESTNAME} POST_BUILD COMMAND ${TESTNAME} ${ARGV1})
endmacro()

if(${GTEST_FOUND})

# Add tests here	
add_gtest(test_trumps)
add_gtest(test_bids)
add_gtest(test_lua ${CMAKE_CURRENT_SOURCE_DIR}/test.lua)
add_gtest(test_computer)


endif()
