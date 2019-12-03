# IdealServer
gtest安装
	sudo apt-get install libgtest-dev
	在目录/usr/src/下生成gtest目录存放源码

	编译源码
	cd /usr/src/gtest
	sudo mkdir build
	cd build
	sudo cmake ..
	sudo make

	将编译生成的库拷贝到系统目录下
	sudo cp libgtest*.a /usr/local/lib

