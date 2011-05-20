# Set up dependencies
include_directories(${CMAKE_SOURCE_DIR} ${GTEST_INCLUDE_DIRS})

# Convenience macro
macro(add_gtest TESTNAME)
    add_executable(${TESTNAME} ${TESTNAME}.cpp)
    target_link_libraries(${TESTNAME} 500 ${GTEST_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
    add_custom_command(TARGET ${TESTNAME} POST_BUILD COMMAND ${TESTNAME})
endmacro()

# Add tests here
add_gtest(test_trumps)
add_gtest(test_bids)
add_gtest(test_bidding)
#add_gtest(test_lua ${CMAKE_CURRENT_SOURCE_DIR}/test.lua)
#add_gtest(test_computer)
