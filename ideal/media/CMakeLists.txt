set(NET_SRCS
    RtspMessage.cc
    H264Source.cc
    MediaSession.cc
    RtpConnection.cc
    RtspServer.cc
    RtspConnection.cc
   )

add_library(ideal_media ${NET_SRCS})

install(TARGETS ideal_media DESTINATION ${PROJECT_SOURCE_DIR}/lib)

set(NET_HEADERS
    Media.h
    Rtp.h
    RtspMessage.h
    MediaSource.h
    H264Source.h
    MediaSession.h
    RtpConnection.h
    Rtsp.h
    RtspServer.h
    RtspConnection.h
   )

install(FILES ${NET_HEADERS} DESTINATION ${PROJECT_SOURCE_DIR}/include/ideal/media)

