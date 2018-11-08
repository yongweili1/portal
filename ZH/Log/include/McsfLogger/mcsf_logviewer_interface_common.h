#ifndef mcsf_logviewer_interface_common_h__
#define mcsf_logviewer_interface_common_h__


#ifdef MCSFLOGVIEWERINTERFACE_EXPORTS
#define MCSF_LOGVIEWER_INTERFACE	__declspec(dllexport)
#else
#define MCSF_LOGVIEWER_INTERFACE	__declspec(dllimport)
#endif


#ifdef OUT
#undef OUT
#endif
#define OUT
#endif // mcsf_logviewer_interface_common_h__