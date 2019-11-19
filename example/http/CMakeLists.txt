add_executable(httprequest_test HttpRequest_test.cc HttpContext.cc)
target_link_libraries(httprequest_test ideal_net ideal_base gtest pthread)

add_executable(httpserver_test HttpServer_test.cc HttpServer.cc HttpContext.cc HttpResponse.cc)
target_link_libraries(httpserver_test ideal_net ideal_base gtest pthread)

