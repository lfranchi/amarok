include_directories( ../..
  ../../collections
  podcasts
  ${CMAKE_BINARY_DIR}/src
  ${AMAROK_COLLECTION_SUPPORT_DIR}
  ${KDE4_INCLUDE_DIR}
  ${QT_INCLUDES}
  )

########### next target ################

set(amarok_collection-umscollection_PART_SRCS
  UmsCollection.cpp
  UmsCollectionLocation.cpp
  podcasts/UmsPodcastProvider.cpp
  podcasts/UmsPodcastMeta.cpp)

kde4_add_ui_files(amarok_collection-umscollection_PART_SRCS
    UmsConfiguration.ui
)

kde4_add_plugin(amarok_collection-umscollection ${amarok_collection-umscollection_PART_SRCS})
	
target_link_libraries(amarok_collection-umscollection
  amaroklib
  amarokcore
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KIO_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_SOLID_LIBRARY}
  ${KDE4_THREADWEAVER_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  )

install(TARGETS 
  amarok_collection-umscollection 
  DESTINATION 
  ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install(FILES 
  amarok_collection-umscollection.desktop 
  DESTINATION 
  ${SERVICES_INSTALL_DIR})

