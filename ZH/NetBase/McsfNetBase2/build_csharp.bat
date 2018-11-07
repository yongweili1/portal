@rem ！！！！！！进行转换之后，需要对以下转换后的文件进行修改，必须修改！！！！！！！
@rem   file_name                     location                  							  where need to modify
@rem   CommunicationProxy.cs         mcsf\MAIN\Features\McsfContainer\McsfCLRNetBase      public virtual ISyncResult SyncSendCommand(CommandContext pCommandContext) {
@rem                                                                                      IntPtr cPtr = McsfNetBasePINVOKE.CommunicationProxy_SyncSendCommand__SWIG_1(swigCPtr, CommandContext.getCPtr(pCommandContext));
@rem                                                                                      ISyncResult ret = (cPtr == IntPtr.Zero) ? null : new ISyncResult(cPtr, false);
@rem                                                                                      return ret;}  
@rem                                                                                      change to
@rem                                                                                      public virtual ISyncResult SyncSendCommand(CommandContext pCommandContext) {
@rem                                                                                      IntPtr cPtr = McsfNetBasePINVOKE.CommunicationProxy_SyncSendCommand__SWIG_1(swigCPtr, CommandContext.getCPtr(pCommandContext));
@rem                                                                                      ISyncResult ret = (cPtr == IntPtr.Zero) ? null : new ISyncResult(cPtr, true);
@rem                                                                                      return ret;}  

@rem                                                                                      public virtual ISyncResult SyncSendDataEx(SendDataContextEx pSendContext) {
@rem                                                                                      IntPtr cPtr = McsfNetBasePINVOKE.CommunicationProxy_SyncSendDataEx__SWIG_1(swigCPtr, SendDataContextEx.getCPtr(pSendContext));
@rem                                                                                      ISyncResult ret = (cPtr == IntPtr.Zero) ? null : new ISyncResult(cPtr, false);
@rem 																					  if (McsfNetBasePINVOKE.SWIGPendingException.Pending) throw McsfNetBasePINVOKE.SWIGPendingException.Retrieve();
@rem                                                                                      return ret;}  
@rem                                                                                      change to
@rem                                                                                      public virtual ISyncResult SyncSendDataEx(SendDataContextEx pSendContext) {
@rem                                                                                      IntPtr cPtr = McsfNetBasePINVOKE.CommunicationProxy_SyncSendDataEx__SWIG_1(swigCPtr, SendDataContextEx.getCPtr(pSendContext));
@rem                                                                                      ISyncResult ret = (cPtr == IntPtr.Zero) ? null : new ISyncResult(cPtr, true);
@rem 																					  if (McsfNetBasePINVOKE.SWIGPendingException.Pending) throw McsfNetBasePINVOKE.SWIGPendingException.Retrieve();
@rem                                                                                      return ret;}  

@rem   IAsyncResult.cs               mcsf\MAIN\Features\McsfContainer\McsfCLRNetBase      public virtual IAsyncResult Clone() {
@rem                                                                                      IntPtr cPtr = McsfNetBasePINVOKE.IAsyncResult_Clone(swigCPtr);
@rem                                                                                      IAsyncResult ret = (cPtr == IntPtr.Zero) ? null : new IAsyncResult(cPtr, false);
@rem                                                                                      return ret;}  
@rem                                                                                      change to
@rem                                                                                      public virtual IAsyncResult Clone() {
@rem                                                                                      IntPtr cPtr = McsfNetBasePINVOKE.IAsyncResult_Clone(swigCPtr);
@rem                                                                                      IAsyncResult ret = (cPtr == IntPtr.Zero) ? null : new IAsyncResult(cPtr, true);
@rem                                                                                      return ret;}

%~d0
cd %~p0

D:\001.Tools\swigwin-3.0.12\swig.exe -c++ -python -outdir ..\McsfPythonNetBase mcsf_netbase.swig



pause