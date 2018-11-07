%~d0
cd %~p0

..\tools\protoc.exe --cpp_out=.\ Mcsf_SysCommandDef.proto
..\tools\ProtoGen.exe --proto_path=.\ Mcsf_SysCommandDef.proto


copy Mcsf_SysCommandDef.pb.h .\include\
copy Mcsf_SysCommandDef.pb.cc .\src\
copy McsfSysCommandDef.cs ..\McsfCLRNetBase\
