  include_directories(
    ${CMAKE_BINARY_DIR}/src
    ${KDE4_INCLUDE_DIR}
    ${QT_INCLUDES}
    ${TOMAHAWK_INCLUDE_DIRS}
  )

  ########### Tomahawk Collection ################
  
  set(amarok_collection_tomahawkcollection_PART_SRCS
    TomahawkCollection.cpp
    TomahawkCollectionFactory.cpp
    TomahawkCollectionLocation.cpp
    TomahawkMeta.cpp
    TomahawkMetaEditCapability.cpp
    TomahawkPlaylist.cpp
    TomahawkPlaylistProvider.cpp
  )

  kde4_add_plugin( amarok_collection_tomahawkcollection
    ${amarok_collection_tomahawkcollection_PART_SRCS}
  )

  TARGET_LINK_LIBRARIES( amarok_collection_tomahawkcollection
    amarokcore
    amaroklib
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_LIBRARIES}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    ${TOMAHAWK_LIBRARY}
  )

    install(TARGETS
    amarok_collection_tomahawkcollection
    DESTINATION
    ${PLUGIN_INSTALL_DIR}
  )

  ########### Tomahawk KCM ################

  find_package(LibAttica)
  find_package(QuaZip)
  find_package(QCA2)
  
  include_directories(${LIBATTICA_INCLUDE_DIR} ${QuaZip_INCLUDE_DIR} ${QCA2_INCLUDE_DIR})
  
  set(kcm_amarok_tomahawkcollection_PART_SRCS
     TomahawkServiceSettings.cpp
  )

  kde4_add_ui_files( kcm_amarok_tomahawkcollection_PART_SRCS
       support/TomahawkConfigWidget.ui
       support/AccountListFactoryWrapper.ui
  )

  kde4_add_plugin( kcm_amarok_tomahawkcollection ${kcm_amarok_tomahawkcollection_PART_SRCS} )

  target_link_libraries( kcm_amarok_tomahawkcollection
    amarokcore
    amaroklib
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_LIBRARIES}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    ${TOMAHAWK_LIBRARY}
    ${LIBATTICA_LIBRARIES}
    ${QuaZip_LIBRARY}
    ${QCA2_LIBRARIES}
  )

  install(TARGETS
    kcm_amarok_tomahawkcollection
    DESTINATION
    ${PLUGIN_INSTALL_DIR}
  )
  
  ########### install files ###############
  
  install(FILES
    amarok_collection_tomahawkcollection.desktop
    DESTINATION
    ${SERVICES_INSTALL_DIR}
  )

  install(FILES
    amarok_collection_tomahawkcollection_config.desktop
    DESTINATION
    ${SERVICES_INSTALL_DIR}
  )