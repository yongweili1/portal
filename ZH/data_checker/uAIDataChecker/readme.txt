1. dmctk编译 
https://blog.csdn.net/xiaominglang/article/details/84315597
2. dcmjp2k
直接进入libsrc 编译静态库，依赖jasper, 其中报错函数第四个参数无效，将其删除即可 
3, CMakeFile 
SET(CMAKE_CXX_FLAGS "-fPIC")
