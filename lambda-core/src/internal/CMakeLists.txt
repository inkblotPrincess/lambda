add_subdirectory(opengl)

if(WIN32)
    add_subdirectory(win32)
else()
    message(FATAL_ERROR "Unrecognised OS")
endif()