#ifndef _MCSFNETBASE_SPEED_COMPILING_STDAFX_
#define _MCSFNETBASE_SPEED_COMPILING_STDAFX_

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <stdexcept>

#ifdef ACE_WIN32
#include <MSTCPiP.h>
#endif

#if defined (_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4355)
#endif 

#if defined (_MSC_VER)
#pragma warning(pop)
#endif 

#include "ace/Select_Reactor.h"
#include "ace/Dev_Poll_Reactor.h"
#include "ace/DLL.h"
#include "ace/Mutex.h"
#include "ace/Get_Opt.h"
#include "ace/Message_Block.h"
#include "ace/UUID.h"
#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Log_Msg.h"
#include "ace/os_include/netinet/os_tcp.h"
#include "ace/os_include/os_netdb.h"
#include "ace/Global_Macros.h"
#include "ace/Time_Value.h"
#include "ace/Reactor.h"
#include "ace/Shared_Memory_MM.h"
#include "ace/Mem_Map.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_sys_stat.h"
#include "ace/SOCK_Connector.h"

#include "boost/lexical_cast.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "boost/typeof/typeof.hpp"
#include "boost/foreach.hpp"
#include "boost/make_shared.hpp"
#include "boost/bind.hpp"
#include "boost/assign.hpp"
#include "boost/function.hpp"
#include <boost/enable_shared_from_this.hpp>
#include "boost/crc.hpp"

#include "mcsf_acceptor.h"
#include "mcsf_tcphandler.h"
#include "mcsf_tcp_server.h"
#include "mcsf_tcp_client.h"
#include "mcsf_error_code.h"
#include "mcsf_tcp_channel.h"
#include "mcsf_asyc_servicecmd_process.h"
#include "mcsf_netbase_interface.h"
#include "mcsf_asyncResult.h"
#include "mcsf_client_manager.h"
#include "mcsf_acceptor.h"
#include "mcsf_communication_node_name.h"
#include "mcsf_communication_proxy.h"
#include "mcsf_cpp_common.h"
#include "mcsf_inner_cmd_handler.h"
#include "mcsf_netbase_command_context.h"
#include "mcsf_share_memory_sender.h"
#include "mcsf_socket_sender.h"
#include "mcsf_sync_result.h"
#include "mcsf_sys_event_def.h"
#include "mcsf_stl_util.h"
#include "mcsf_communication_proxy_factory.h"
#include "mcsf_connector.h"
#include "mcsf_dynamic_threadpool.h"
#include "mcsf_share_memory_receiver.h"
#include "mcsf_socket_receiver.h"
#include "mcsf_logger_message_export.h"
#include "mcsf_netbase_iclr_cmd_handler.h"
#include "mcsf_pending_call.h"
#include "mcsf_container_log.h"
#include "mcsf_share_memory_receiver.h"
#include "mcsf_blobdata_interface.h"

#include "mcsf_rpc_msg_redef.h"

#include "SystemCmdMessage.pb.h"
#include "SystemCmdMessage.pb.h"
#include "Mcsf_SysCommandDef.pb.h"


#include "McsfErrorHandler/mcsf_error_handler.h"

#endif