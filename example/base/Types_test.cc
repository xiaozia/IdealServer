
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Types_test.cc
*   Last Modified : 2019-11-05 17:05
*   Describe      :
*
*******************************************************/

#include "ideal/base/Types.h"

#include <iostream>

using namespace ideal;

class B { };
class A1 : public B { };
class A2 : public B { };
class C : public A1, public A2 { };


void Function(const A1& a) {
    std::cout << "A1 Function" << std::endl;
}

void Function(const A2& a) {
    std::cout << "A2 Function" << std::endl;
}

int main() {
    C c;      // downcast
    Function(static_cast<A1&>(c));
    Function(implicit_cast<A1&>(c));

    B b;  // upcast
    Function(static_cast<A2&>(b));     // 编译成功，运行时可能导致程序崩溃
//  Function(implicit_cast<A2&>(b));   // 编译失败

    return 0;
}

