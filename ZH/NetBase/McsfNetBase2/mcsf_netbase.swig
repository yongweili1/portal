%module(directors="1") McsfNetBase

%{
#include "mcsf_swig_string_util.h"
#include "mcsf_swig_sharedarray_util.h"
#include "mcsf_netbase_interface.h"
#include "mcsf_netbase_command_context.h"
#include "mcsf_netbase_senddata_context.h"
#include "mcsf_netbase_iasync_result.h"
#include "mcsf_netbase_ibigdata_handler.h"
#include "mcsf_netbase_iclr_cmd_handler.h"
#include "mcsf_netbase_icmd_callback_handler.h"
#include "mcsf_netbase_icommunication_proxy.h"
#include "mcsf_netbase_ievent_handler.h"
#include "mcsf_netbase_isync_result.h"
#include "mcsf_communication_proxy.h"
%}

/* turn on director wrapping Callback See example at swigwin-2.0.4\Examples\csharp\callback*/
%feature("director") ICommandCallbackHandler;
%feature("director") ICommandCallbackHandlerEx;
%feature("director") ICLRCommandCallbackHandlerEx;
%feature("director") ICommandHandler;
%feature("director") ICLRCommandHandler;
%feature("director") ICommandHandlerEx;
%feature("director") ICLRCommandHandlerEx;
%feature("director") IDataHandler;
%feature("director") IDataHandlerEx;
%feature("director") ICLRDataHandlerEx;
%feature("director") IEventHandler;


/* Let's just grab the original header file here */
%include "mcsf_swig_string_util.h"

#include "../../include/McsfNetBase/mcsf_netbase_interface.h"
%include "../../include/McsfNetBase/mcsf_netbase_command_context.h"
%include "../../include/McsfNetBase/mcsf_netbase_icmd_callback_handler.h"
%include "../../include/McsfNetBase/mcsf_netbase_command_handler.h"
%include "../../include/McsfNetBase/mcsf_netbase_iasync_result.h"

%include "../../include/McsfNetBase/mcsf_netbase_senddata_context.h"
%include "../../include/McsfNetBase/mcsf_netbase_ibigdata_handler.h"
%include "mcsf_netbase_iclr_cmd_handler.h"


%include "mcsf_swig_sharedarray_util.h" 
%include "boost_shared_ptr.i"
%shared_ptr(Mcsf::ICommandContextEx)
%shared_ptr(Mcsf::ICommandCallbackHandlerEx)

%include "../../include/McsfNetBase/mcsf_netbase_ievent_handler.h"
%include "../../include/McsfNetBase/mcsf_netbase_isync_result.h"

%include "stl.i"
%include "../../include/McsfNetBase/mcsf_netbase_icommunication_proxy.h"
%include "../../include/McsfNetBase/mcsf_communication_proxy.h"





