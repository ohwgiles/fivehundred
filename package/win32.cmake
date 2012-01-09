install(TARGETS fivehundred RUNTIME DESTINATION .)
install(DIRECTORY gfx DESTINATION . PATTERN ".svn" EXCLUDE)
install(DIRECTORY scripts DESTINATION . PATTERN ".svn" EXCLUDE)
find_program(WINDRES windres.exe)
add_custom_command(OUTPUT icon.obj COMMAND ${WINDRES} ${CMAKE_CURRENT_SOURCE_DIR}/package/win_icon.rc icon.a DEPENDS package/win_icon.rc)
target_link_libraries(fivehundred ${CMAKE_CURRENT_BINARY_DIR}/icon.obj)
set_target_properties(fivehundred PROPERTIES WIN32_EXECUTABLE true)
set(CPACK_NSIS_INSTALLED_ICON_NAME "fivehundred.exe")
set(CPACK_NSIS_DISPLAY_NAME "Five Hundred")
set(CPACK_NSIS_HELP_LINK "http://fivehundred.sourceforge.net")
set(CPACK_NSIS_URL_INFO_ABOUT "http://fivehundred.sourceforge.net")
set(CPACK_NSIS_CREATE_ICONS "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Five Hundred.lnk' '\$INSTDIR\\\\fivehundred.exe'")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "fivehundred.exe")
set(CPACK_STRIP_FILES "fivehundred.exe")
set(CPACK_GENERATOR "NSIS")

