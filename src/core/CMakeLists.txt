include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/shared
    ${KDE4_INCLUDE_DIR}
    ${QT_INCLUDES}
)

set(libcore_interfaces_SRCS
    interfaces/Logger.cpp
)

set(libcore_meta_SRCS
    meta/Meta.cpp
    meta/support/MetaConstants.cpp
    meta/support/MetaUtility.cpp
    meta/support/MetaKeys.cpp
    meta/support/PrivateMetaRegistry.cpp
    ../../shared/FileType.cpp
)

set(libcore_playlists_SRCS
    playlists/Playlist.cpp
    playlists/PlaylistFormat.cpp
    playlists/PlaylistProvider.cpp
)

set(libcore_statistics_SRCS
    statistics/StatisticsProvider.cpp
)

set(libcore_capabilities_SRCS
    capabilities/Capability.cpp
    capabilities/ActionsCapability.cpp
    capabilities/BookmarkThisCapability.cpp
    capabilities/BoundedPlaybackCapability.cpp
    capabilities/CollectionScanCapability.cpp
    capabilities/CollectionImportCapability.cpp
    capabilities/EditCapability.cpp
    capabilities/EditablePlaylistCapability.cpp
    capabilities/FindInSourceCapability.cpp
    capabilities/LastFmCapability.cpp
    capabilities/MultiPlayableCapability.cpp
    capabilities/MultiSourceCapability.cpp
    capabilities/OrganiseCapability.cpp
    capabilities/ReadLabelCapability.cpp
    capabilities/SourceInfoCapability.cpp
    capabilities/StatisticsCapability.cpp
    capabilities/StreamInfoCapability.cpp
    capabilities/TranscodeCapability.cpp
    capabilities/WriteLabelCapability.cpp
)

set(libcore_collection_SRCS
    collections/Collection.cpp
    collections/CollectionLocation.cpp
    collections/MetaQueryMaker.cpp
    collections/QueryMaker.cpp
    collections/support/TrackForUrlWorker.cpp
)

set(libcore_podcasts_SRCS
    podcasts/PodcastReader.cpp
    podcasts/PodcastMeta.cpp
    podcasts/PodcastImageFetcher.cpp
    podcasts/PodcastProvider.cpp
)

set(libcore_support_SRCS
    support/Amarok.cpp
    support/Components.cpp
    support/SmartPointerList.cpp
    support/PluginFactory.cpp
    support/Debug.cpp
)

set(libcore_transcoding_SRCS
    transcoding/formats/TranscodingNullFormat.cpp
    transcoding/formats/TranscodingAacFormat.cpp
    transcoding/formats/TranscodingAlacFormat.cpp
    transcoding/formats/TranscodingFlacFormat.cpp
    transcoding/formats/TranscodingMp3Format.cpp
    transcoding/formats/TranscodingVorbisFormat.cpp
    transcoding/formats/TranscodingWmaFormat.cpp
    transcoding/TranscodingConfiguration.cpp
    transcoding/TranscodingController.cpp
    transcoding/TranscodingProperty.cpp
)

#####################################################################
# LIBCORE
#####################################################################
set(libcore_LIB_SRCS
    ${libcore_podcasts_SRCS}
    ${libcore_interfaces_SRCS}
    ${libcore_collection_SRCS}
    ${libcore_statistics_SRCS}
    ${libcore_playlists_SRCS}
    ${libcore_meta_SRCS}
    ${libcore_capabilities_SRCS}
    ${libcore_support_SRCS}
    ${libcore_transcoding_SRCS}
)

kde4_add_library(amarokcore SHARED ${libcore_LIB_SRCS})

target_link_libraries(amarokcore
    ${CMAKE_THREAD_LIBS_INIT}
    ${KDE4_KIO_LIBS}
#    ${KDE4_KUTILS_LIBS}
    ${KDE4_SOLID_LIBRARY}
    ${KDE4_THREADWEAVER_LIBRARIES}
    ${QT_CORE_LIBRARY}
)

if(APPLE)
    SET_TARGET_PROPERTIES(amarokcore PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)

if(LIBDL_FOUND)
    target_link_libraries(amarokcore dl)
endif(LIBDL_FOUND)

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME MATCHES Linux)
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=iso9899:1999" )
endif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME MATCHES Linux)

set_target_properties(amarokcore PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS amarokcore ${INSTALL_TARGETS_DEFAULT_ARGS} )


