%~d0
cd %~p0

..\tools\protoc.exe --cpp_out=.\ RpcMessage.proto
..\tools\protoc.exe --cpp_out=.\ SystemCmdMessage.proto

copy /Y *.h .\include\*.h
copy /Y *.cc .\src\*.cc

