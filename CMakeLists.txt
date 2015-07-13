CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libscl-core)

SET(SRCS
    src/sclcoreui.cpp
    src/sclcoreui-efl.cpp
    src/sclcoreimpl.cpp
    src/sclconnection.cpp
    src/sclconnection-isf.cpp
    src/sclcore.cpp
)

IF (with_wayland)
    SET(SRCS ${SRCS} src/input-method-protocol.c)
ELSE (with_wayland)
    SET(SRCS ${SRCS})
ENDIF (with_wayland)

SET(PACKAGE ${PROJECT_NAME})
SET(PKGNAME ${PACKAGE})
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(LIBDIR ${LIB_INSTALL_DIR})

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_SOURCE_DIR}/src/include
                   )

INCLUDE(FindPkgConfig)

SET(PKGS_CHECK_MODULES
        elementary
        dlog
        isf
        vconf
        libscl-common
        )

IF (with_wayland)
        ADD_DEFINITIONS("-DWAYLAND")
        pkg_check_modules(pkgs REQUIRED ${PKGS_CHECK_MODULES} ecore-wayland wayland-client)
ELSE (with_wayland)
        pkg_check_modules(pkgs REQUIRED ${PKGS_CHECK_MODULES} ecore-x x11)
ENDIF(with_wayland)

FOREACH(flag ${pkgs_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g")

ADD_DEFINITIONS("-DPACKAGE=\"${PACKAGE}\"")
ADD_DEFINITIONS("-DPACKAGE_NAME=\"${PKGNAME}\"")
ADD_DEFINITIONS("-DLOG_TAG=\"LIBSCL_CORE\"")
ADD_DEFINITIONS("-D__EFL__")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIBDIR})

INSTALL(FILES "${CMAKE_SOURCE_DIR}/src/sclcore.h" DESTINATION include/libscl-core)
INSTALL(FILES "${CMAKE_SOURCE_DIR}/src/sclcoretypes.h" DESTINATION include/libscl-core)
INSTALL(FILES "${CMAKE_SOURCE_DIR}/src/sclcorecallback.h" DESTINATION include/libscl-core)

CONFIGURE_FILE(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${LIBDIR}/pkgconfig)
