%module(directors="1") McsfNetBase

%{
#include "mcsf_netbase_interface.h"
#include "mcsf_communication_proxy.h"
%}

%include "stl.i"

%apply const std::string & {std::string &};

/* turn on director wrapping Callback See example at swigwin-2.0.4\Examples\csharp\callback*/
%feature("director") ICommandCallbackHandler;
%feature("director") ICommandHandler;
%feature("director") IContainee;
%feature("director") IEventHandler;

/* Let's just grab the original header file here */
%include "mcsf_netbase_interface.h"
%include "mcsf_communication_proxy.h"

/* let do array typemap, see example at swigwin-2.0.4\Examples\csharp\arrays and swigwin-2.0.4\Lib\csharp\arrays_csharp.i */
%include "arrays_csharp.i"

%apply byte FIXED[] { unsigned char* pRawData }

%csmethodmodifiers SendDataContext "public unsafe";

struct SendDataContext
{
    std::string sReceiver;
    std::string sSender;
    int iCommandId;
    int iLen;
    unsigned char* pRawData;
};



