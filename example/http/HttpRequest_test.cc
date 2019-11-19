
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : HttpRequest_test.cc
*   Last Modified : 2019-11-19 19:55
*   Describe      :
*
*******************************************************/

#include "HttpContext.h"

#include "ideal/net/Buffer.h"
#include <gtest/gtest.h>

TEST(HttpRequest, testParseRequestAllInOne) {
    HttpContext context;
    ideal::net::Buffer input;
    input.append("GET /index.html HTTP/1.1\r\n"
         "Host: www.chenshuo.com\r\n"
         "\r\n");
    
    EXPECT_TRUE(context.parseRequest(&input, ideal::Timestamp::now()));
    EXPECT_TRUE(context.gotAll());

    const HttpRequest& request = context.request();
    EXPECT_EQ(request.getMethod(), HttpRequest::kGet);
    EXPECT_EQ(request.getPath(), std::string("/index.html"));
    EXPECT_EQ(request.getVersion(), HttpRequest::kHttp11);
    EXPECT_EQ(request.getHeader("Host"), std::string("www.chenshuo.com"));
    EXPECT_EQ(request.getHeader("User-Agent"), std::string(""));
}

TEST(HttpRequest, testParseRequestInTwoPieces) {
    std::string all("GET /index.html HTTP/1.1\r\n"
         "Host: www.chenshuo.com\r\n"
         "\r\n");
    
    for(size_t sz1 = 0; sz1 < all.size(); ++sz1) {
        HttpContext context;
        ideal::net::Buffer input;
        input.append(all.c_str(), sz1);
        EXPECT_TRUE(context.parseRequest(&input, ideal::Timestamp::now()));
        EXPECT_FALSE(context.gotAll());
        
        size_t sz2 = all.size() - sz1;
        input.append(all.c_str() + sz1, sz2);
        EXPECT_TRUE(context.parseRequest(&input, ideal::Timestamp::now()));
        EXPECT_TRUE(context.gotAll());

        const HttpRequest& request = context.request();
        EXPECT_EQ(request.getMethod(), HttpRequest::kGet);
        EXPECT_EQ(request.getPath(), std::string("/index.html"));
        EXPECT_EQ(request.getVersion(), HttpRequest::kHttp11);
        EXPECT_EQ(request.getHeader("Host"), std::string("www.chenshuo.com"));
        EXPECT_EQ(request.getHeader("User-Agent"), std::string(""));
    }
}

TEST(HttpRequest, testParseRequestEmptyHeaderValue) {
    HttpContext context;
    ideal::net::Buffer input;
    input.append("GET /index.html HTTP/1.1\r\n"
         "Host: www.chenshuo.com\r\n"
         "User-Agent:\r\n"
         "Accept-Encoding: \r\n"
         "\r\n");
    
    EXPECT_TRUE(context.parseRequest(&input, ideal::Timestamp::now()));
    EXPECT_TRUE(context.gotAll());

    const HttpRequest& request = context.request();
    EXPECT_EQ(request.getMethod(), HttpRequest::kGet);
    EXPECT_EQ(request.getPath(), std::string("/index.html"));
    EXPECT_EQ(request.getVersion(), HttpRequest::kHttp11);
    EXPECT_EQ(request.getHeader("Host"), std::string("www.chenshuo.com"));
    EXPECT_EQ(request.getHeader("User-Agent"), std::string(""));
    EXPECT_EQ(request.getHeader("Accept-Encoding"), std::string(""));
}


int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

