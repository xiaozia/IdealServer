add_executable(rtsp_h264file rtsp_h264file.cc H264File.cc)
target_link_libraries(rtsp_h264file ideal_media ideal_net ideal_base gtest pthread)

