set(QT_MENU_NIB "/path/to/qt_menu.nib" CACHE PATH "Path to qt_menu.nib")
set_source_files_properties(${AI_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/scripts)
set_source_files_properties(${GFX_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/gfx)
add_custom_command(TARGET fivehundred POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${QT_MENU_NIB} fivehundred.app/Contents/Resources/qt_menu.nib)

install(TARGETS fivehundred BUNDLE DESTINATION .) 

set(MACOSX_BUNDLE_INFO_STRING ${VERSION})
set(MACOSX_BUNDLE_ICON_FILE Resources/gfx/icon.gif)
set(MACOSX_BUNDLE_GUI_IDENTIFIER "Five Hundred")
set(MACOSX_BUNDLE_LONG_VERSION_STRING ${VERSION})
set(MACOSX_BUNDLE_BUNDLE_NAME "Five Hundred")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
set(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION})
set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2011 Oliver Giles")

set(CPACK_GENERATOR "DragNDrop")
