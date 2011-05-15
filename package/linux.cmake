install(TARGETS fivehundred RUNTIME DESTINATION bin)
install(DIRECTORY gfx DESTINATION share/fivehundred PATTERN ".svn" EXCLUDE)
install(DIRECTORY scripts DESTINATION share/fivehundred PATTERN ".svn" EXCLUDE)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fivehundred.desktop DESTINATION share/applications)

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
		set(CPACK_SYSTEM_NAME "amd64")
	else()
		set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
		set(CPACK_SYSTEM_NAME "i386")
	endif()
 
    set(CPACK_DEBIAN_PACKAGE_NAME "fivehundred")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6 (>= 4.5), libqt4-core (>= 4.5), liblua5.1-0-dev")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Oliver Giles")
	set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Five Hundred is a card game for four players similar to Euchre or Bridge. This cross-platform Qt-based implementation includes both a graphical mode and a non-interactive mode and Lua scripting engine to develop AI players")
    set(CPACK_GENERATOR "DEB;TGZ")

