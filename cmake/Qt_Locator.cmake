# Ref: http://public.kitware.com/pipermail/cmake/2014-November/059118.html
set(ENV_PROGRAMFILES "ProgramFiles(X86)")

# Clear Qt viables
unset(QT_5xx)
unset(QT_6xx)
unset(QT_ROOT)

set(QT_5xx True)
set(QT_MSVC 2019)
set(QT_MISSING True)

IF(QT_5xx)
    # look for user-registry pointing to qtcreator
    GET_FILENAME_COMPONENT(QT_BIN [HKEY_CURRENT_USER\\Software\\Classes\\Applications\\QtProject.QtCreator.cpp\\shell\\Open\\Command] PATH)

    # get root path so we can search for 5.3, 5.4, 5.5, etc
    STRING(REPLACE "/Tools" ";" QT_BIN "${QT_BIN}")
    LIST(GET QT_BIN 0 QT_BIN)
    FILE(GLOB QT_VERSIONS "${QT_BIN}/5.*")
    LIST(SORT QT_VERSIONS)

    # assume the latest version will be last alphabetically
    LIST(REVERSE QT_VERSIONS)

    LIST(GET QT_VERSIONS 0 QT_VERSION)

    # fix any double slashes which seem to be common
    STRING(REPLACE "//" "/"  QT_VERSION "${QT_VERSION}")
    MESSAGE("Qt version: ${QT_VERSIONS}")

    # check for 64-bit os
    # may need to be removed for older compilers as it wasn't always offered
    # only support 64-bit os
    SET(QT_PATH "${QT_VERSION}/msvc${QT_MSVC}_64")
    MESSAGE("Qt path: ${QT_PATH}")
    SET(QT_MISSING False)

    # use Qt_DIR approach so you can find Qt after cmake has been invoked
    IF(NOT QT_MISSING)
        SET(Qt5_DIR "${QT_PATH}/lib/cmake/Qt5/")
        SET(Qt5Test_DIR "${QT_PATH}/lib/cmake/Qt5Test")
    ENDIF()
ENDIF()

# MacOS
if(OSX)

endif()

# Linux
if(LINUX)

endif()