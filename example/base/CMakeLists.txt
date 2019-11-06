add_executable(types_test Types_test.cc) 
target_link_libraries(types_test ideal_base)

add_executable(timestamp_test Timestamp_test.cc) 
target_link_libraries(timestamp_test ideal_base)

add_executable(exception_test Exception_test.cc) 
target_link_libraries(exception_test ideal_base)

add_executable(blockingqueue_test BlockingQueue_test.cc) 
target_link_libraries(blockingqueue_test ideal_base pthread)

add_executable(boundedblockingqueue_test BoundedBlockingQueue_test.cc) 
target_link_libraries(boundedblockingqueue_test ideal_base pthread)

add_executable(date_test Date_test.cc)
target_link_libraries(date_test ideal_base)

add_executable(timezone_test TimeZone_test.cc)
target_link_libraries(timezone_test ideal_base)

add_executable(threadpool_test ThreadPool_test.cc)
target_link_libraries(threadpool_test ideal_base pthread)

add_executable(fileutil_test FileUtil_test.cc)
target_link_libraries(fileutil_test ideal_base)

add_executable(processinfo_test ProcessInfo_test.cc)
target_link_libraries(processinfo_test ideal_base)

add_executable(logfile_test LogFile_test.cc)
target_link_libraries(logfile_test ideal_base)

if(GOOGLETEST_LIBRARY)
    add_executable(any_test Any_test.cc) 
    target_link_libraries(any_test gtest pthread)

    add_executable(logstream_test LogStream_test.cc) 
    target_link_libraries(logstream_test ideal_base gtest pthread)
endif()
