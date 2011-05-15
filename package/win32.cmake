install(TARGETS fivehundred )
install(DIRECTORY gfx PATTERN ".svn" EXCLUDE)
install(DIRECTORY scripts PATTERN ".svn" EXCLUDE)
# There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/gfx\\\\icon48.png")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "fivehundred.exe")
    set(CPACK_NSIS_DISPLAY_NAME "Five Hundred")
    set(CPACK_NSIS_HELP_LINK "http://fivehundred.sourceforge.net")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://fivehundred.sourceforge.net")
	set(CPACK_NSIS_MUI_FINISHPAGE_RUN "fivehundred.exe")
	set(CPACK_GENERATOR "NSIS")

