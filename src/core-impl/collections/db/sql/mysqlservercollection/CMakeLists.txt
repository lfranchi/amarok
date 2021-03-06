#build-time only dependency in XesamDbus.h
#remove at some point
remove_definitions(-DMAKE_AMAROK_DATABASECOLLECTION_LIB)

add_definitions(${MYSQL_CFLAGS})

include_directories(
        ..
        .
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${QTIQTCORE_INCLUDES}
        ${MYSQL_INCLUDE_DIR}
        )

########### mysqlserver ###############

set(amarok_collection-mysqlservercollection_PART_SRCS
    ../mysql-shared/MySqlStorage.cpp
    MySqlServerCollection.cpp
 )

kde4_add_plugin(amarok_collection-mysqlservercollection ${amarok_collection-mysqlservercollection_PART_SRCS})

STRING(REPLACE "-Wl,--fatal-warnings" "" CMAKE_SHARED_LINKER_FLAGS_NOFATALWARN "${CMAKE_SHARED_LINKER_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS_NOFATALWARN} )

STRING(REPLACE "-Wl,--fatal-warnings" "" CMAKE_MODULE_LINKER_FLAGS_NOFATALWARN "${CMAKE_MODULE_LINKER_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS_NOFATALWARN} )

target_link_libraries(amarok_collection-mysqlservercollection
    amarok-sqlcollection
    amarokcore
    amaroklib
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${MYSQL_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${ZLIB_LIBRARIES}
)

if(NOT WIN32 AND NOT APPLE)
    target_link_libraries( amarok_collection-mysqlservercollection crypt pthread )
endif(NOT WIN32 AND NOT APPLE)

if(APPLE)
	SET_TARGET_PROPERTIES(amarok_collection-mysqlservercollection PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)

install(TARGETS amarok_collection-mysqlservercollection DESTINATION ${PLUGIN_INSTALL_DIR} )

install(FILES amarok_collection-mysqlservercollection.desktop DESTINATION ${SERVICES_INSTALL_DIR})

