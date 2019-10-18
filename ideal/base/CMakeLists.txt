set(BASE_SRCS
    Timestamp.cc
    CurrentThread.cc
    Exception.cc
    Thread.cc
    LogStream.cc
    Date.cc
    TimeZone.cc
    Logger.cc
    ThreadPool.cc
    FileUtil.cc
    ProcessInfo.cc
    LogFile.cc
  )

add_library(ideal_base ${BASE_SRCS})

install(TARGETS ideal_base DESTINATION ${PROJECT_SOURCE_DIR}/lib)

set(BASE_HEADERS
    copyable.h
    noncopyable.h
	Timestamp.h
    BlockingQueue.h 
    BoundedBlockingQueue.h
    CountDownLatch.h
    CurrentThread.h
    Exception.h
    Thread.h
    LogStream.h
    Date.h
    TimeZone.h
    Logger.h
    ThreadPool.h
    FileUtil.h
    StringPiece.h
    ProcessInfo.h
    LogFile.h
    WeakCallback.h
  )
install(FILES ${BASE_HEADERS} DESTINATION ${PROJECT_SOURCE_DIR}/include/ideal/base)

