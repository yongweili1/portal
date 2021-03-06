// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Mcsf_SysCommandDef.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace Mcsf {

namespace {

const ::google::protobuf::EnumDescriptor* SystemCommandId_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_Mcsf_5fSysCommandDef_2eproto() {
  protobuf_AddDesc_Mcsf_5fSysCommandDef_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "Mcsf_SysCommandDef.proto");
  GOOGLE_CHECK(file != NULL);
  SystemCommandId_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_Mcsf_5fSysCommandDef_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void protobuf_ShutdownFile_Mcsf_5fSysCommandDef_2eproto() {
}

void protobuf_AddDesc_Mcsf_5fSysCommandDef_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\030Mcsf_SysCommandDef.proto\022\004Mcsf*\244\020\n\017Sys"
    "temCommandId\022\037\n\033SYSTEM_COMMAND_ID_UNDEFI"
    "NED\020\000\022&\n\"SYSTEM_COMMAND_ID_REGIST_CONTAI"
    "NEE\020\001\022$\n SYSTEM_COMMAND_ID_FIND_CONTAINE"
    "E\020\002\022*\n&SYSTEM_COMMAND_ID_REGIST_EVENT_CH"
    "ANNEL\020\003\022%\n!SYSTEM_COMMAND_ID_REGIST_WHO_"
    "I_AM\020\004\022 \n\034SYSTEM_COMMAND_ID_CHECK_CAST\020\005"
    "\0222\n.SYSTEM_COMMAND_ID_SHAREMEMORY_BIGDAT"
    "A_TRANSFER\020\006\022&\n\"SYSTEM_COMMAND_ID_SUBSCR"
    "IBE_EVENTS\020\007\022\036\n\032SYSTME_COMMAND_ID_IS_EVE"
    "NT\020\010\022\034\n\030SYSTEM_COMMAND_ID_NUMBER\020\t\022\032\n\026SY"
    "STEM_COMMAND_ID_ECHO\020\n\022&\n\"SYTTEM_COMMAND"
    "_ID_CHECKCAST_TONAME\020\013\022.\n)SYSTEM_COMMAND"
    "_ID_SYS_MANAGER_GET_PROCESS\020\200\002\022/\n*SYSTEM"
    "_COMMAND_ID_SYS_MANAGER_STOP_PROCESS\020\201\002\022"
    "0\n+SYSTEM_COMMAND_ID_SYS_MANAGER_START_P"
    "ROCESS\020\202\002\0222\n-SYSTEM_COMMAND_ID_SYS_MANAG"
    "ER_RESTART_PROCESS\020\203\002\022-\n(SYSTEM_COMMAND_"
    "ID_SYS_MANAGER_STOP_GROUP\020\204\002\022.\n)SYSTEM_C"
    "OMMAND_ID_SYS_MANAGER_START_GROUP\020\205\002\0220\n+"
    "SYSTEM_COMMAND_ID_SYS_MANAGER_RESTART_GR"
    "OUP\020\206\002\0225\n0SYSTEM_COMMAND_ID_SYS_MANAGER_"
    "STOP_ALL_PROCESSES\020\207\002\0226\n1SYSTEM_COMMAND_"
    "ID_SYS_MANAGER_START_ALL_PROCESSES\020\210\002\0228\n"
    "3SYSTEM_COMMAND_ID_SYS_MANAGER_RESTART_A"
    "LL_PROCESSES\020\211\002\022,\n\'SYSTEM_COMMAND_ID_SYS"
    "_MANAGER_SHUT_DOWN\020\212\002\022/\n*SYSTEM_COMMAND_"
    "ID_SYS_MANAGER_KILL_PROCESS\020\213\002\022-\n(SYSTEM"
    "_COMMAND_ID_SYS_MANAGER_KILL_GROUP\020\214\002\0225\n"
    "0SYSTEM_COMMAND_ID_SYS_MANAGER_KILL_ALL_"
    "PROCESSES\020\215\002\0222\n-SYSTEM_COMMAND_ID_SYS_MA"
    "NAGER_SHUTDOWN_SYSTEM\020\216\002\022/\n*SYSTEM_COMMA"
    "ND_ID_SYS_MANAGER_QUERY_STATUS\020\217\002\0227\n2SYS"
    "TEM_COMMAND_ID_SYS_MANAGER_QUERY_ISSHUTD"
    "OWNABLE\020\220\002\0223\n.SYSTEM_COMMAND_ID_SYS_MANA"
    "GER_CONTAINER_REPORT\020\221\002\0226\n1SYSTEM_COMMAN"
    "D_ID_SYS_MANAGER_REGISTER_SLAVE_NAME\020\222\002\022"
    "=\n8SYSTEM_COMMAND_ID_SYS_MANAGER_REGISTE"
    "R_SLAVE_PROCESSINFO\020\223\002\0227\n2SYSTEM_COMMAND"
    "_ID_SYS_MANAGER_CSHARP_ERASE_PROCESS\020\224\002\022"
    "*\n%SYSTME_COMMAND_ID_PROCESS_INFO_REPORT"
    "\020\225\002\022)\n$SYSTEM_COMMAND_ID_PROCESS_INFO_QU"
    "ERY\020\226\002\0224\n/SYSTEM_COMMAND_ID_SYS_MANAGER_"
    "SLAVE_EXAMINATION\020\227\002\0222\n-SYSTEM_COMMAND_I"
    "D_SYS_MANAGER_QUERY_SHUT_DOWN\020\230\002\0227\n2SYST"
    "EM_COMMAND_ID_SYS_MANAGER_QUERY_SLAVE_DI"
    "SCINFO\020\231\002\0227\n2SYSTEM_COMMAND_ID_SYS_MANAG"
    "ER_SHUTDOWN_PREPARATION\020\232\002\022,\n\'SYSTEM_COM"
    "MAND_ID_SYS_MANAGER_OVER_SELF\020\233\002\0228\n3SYST"
    "EM_COMMAND_ID_SYS_MANAGER_ABORT_SHUTDOWN"
    "_SYSTEM\020\234\002\022@\n;SYSTEM_COMMAND_ID_SYS_MANA"
    "GER_ADJUST_SLAVE_PROCESS_PRIORITY\020\235\002\022)\n$"
    "SYSTEM_COMMAND_ID_UNREGIST_CONTAINEE\020\200\006", 2119);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "Mcsf_SysCommandDef.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_Mcsf_5fSysCommandDef_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_Mcsf_5fSysCommandDef_2eproto {
  StaticDescriptorInitializer_Mcsf_5fSysCommandDef_2eproto() {
    protobuf_AddDesc_Mcsf_5fSysCommandDef_2eproto();
  }
} static_descriptor_initializer_Mcsf_5fSysCommandDef_2eproto_;

const ::google::protobuf::EnumDescriptor* SystemCommandId_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return SystemCommandId_descriptor_;
}
bool SystemCommandId_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 256:
    case 257:
    case 258:
    case 259:
    case 260:
    case 261:
    case 262:
    case 263:
    case 264:
    case 265:
    case 266:
    case 267:
    case 268:
    case 269:
    case 270:
    case 271:
    case 272:
    case 273:
    case 274:
    case 275:
    case 276:
    case 277:
    case 278:
    case 279:
    case 280:
    case 281:
    case 282:
    case 283:
    case 284:
    case 285:
    case 768:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Mcsf

// @@protoc_insertion_point(global_scope)
