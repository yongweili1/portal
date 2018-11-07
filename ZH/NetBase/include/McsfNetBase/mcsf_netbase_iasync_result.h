////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_iasync_result.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_IASYNC_RESULT_H_
#define MCSF_NETBASE_IASYNC_RESULT_H_

namespace Mcsf {
    
    /// \class IAsyncResult
    /// \brief the asynchronous command call result base class.
    /// here is an example.
    /// \verbatim
    /// class LongCmdCallBack : public ICommandCallbackHandler
    /// {
    ///     virtual int HandleReply(const IAsyncResult* pAsyncResult)
    ///     {
    ///          std::string result = pAsyncResult->GetMarshalObject();
    ///          // here you can use protocol buffer deserialize to C++ struct from result
    ///     }
    /// }
    /// CommandContext *pContext = new CommandContext()
    /// pContext->pCommandCallback = new LongCmdCallBack()
    ///
    /// \endverbatim
    class IAsyncResult
    {
    public:
        enum CallResult{
            ReceiveResponse,
            TimeOut,
            TCPClose,
            NotHandler,
        };

        /// Get the return result that serialized in string
        /// 
        /// \return  std::string the serialized result.
        virtual std::string GetMarshalObject() const = 0;
         
        /// No practical use, when callback invoke it return true, else false
        /// 
        /// \return  bool 
        virtual bool IsCompleted() const = 0;

        /// Get the asynchronous call result 
        ///
        /// \return CallResult::ReceiveResponse mean receive response, and 
        /// CallResult::TimeOut means not receive response till timeout.
        virtual CallResult GetCallResult() const = 0;

        virtual ~IAsyncResult() {}
        //////////////////////////////////////////////////////////////////////////
        //utility function for C# wrapper 
        virtual IAsyncResult* Clone() const = 0;
        virtual void Destory() = 0;
    };

}

#endif // MCSF_NETBASE_IASYNC_RESULT_H_
