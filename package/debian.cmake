configure_file(fivehundred.desktop.in fivehundred.desktop)
install(TARGETS fivehundred RUNTIME DESTINATION games)
install(DIRECTORY gfx DESTINATION share/fivehundred PATTERN ".svn" EXCLUDE)
install(DIRECTORY scripts DESTINATION share/fivehundred PATTERN ".svn" EXCLUDE)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fivehundred.desktop DESTINATION share/applications)
install(FILES package/fivehundred.6.gz DESTINATION share/man/man6)

# Draconian debian packaging requirements
install(FILES README package/copyright DESTINATION share/doc/fivehundred/)
install(FILES package/changelog.gz DESTINATION share/doc/fivehundred)
	
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
	set(CPACK_SYSTEM_NAME "amd64")
else()
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
	set(CPACK_SYSTEM_NAME "i386")
endif()
 
set(CPACK_DEBIAN_PACKAGE_NAME "fivehundred")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6 (>= 4.5), libqt4-core (>= 4.5), liblua5.1-0, libc6 (>= 2.12)")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Oliver Giles <ohwgiles@users.sourceforge.net>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://fivehundred.sf.net")
execute_process(COMMAND cat ${CMAKE_SOURCE_DIR}/DESCRIPTION OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_DESCRIPTION)
set(CPACK_DEBIAN_PACKAGE_DEBUG ON)
set(CPACK_GENERATOR "DEB")


