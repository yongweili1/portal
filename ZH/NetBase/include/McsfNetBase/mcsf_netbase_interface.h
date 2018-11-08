//==========================================================================
/**
 *  Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
 *  All rights reserved
 *
 *  @file   mcsf_netbase_interface.h
 *  @author Ruifei.liu <ruifei.liu@united-imaging.com>
 * 
 *  @date   2011/09/05
 *
 *  @brief  a batch of publish interface definition
 */
//==========================================================================

#ifndef _MCSF_NETBASEINTEAFACE_H
#define _MCSF_NETBASEINTEAFACE_H

#include <string>
#include "mcsf_netbase_export.h"

#ifdef SWIG
#define McsfNetBase_Export
#endif

#include "mcsf_netbase_command_context.h"
#include "mcsf_netbase_senddata_context.h"
#include "mcsf_netbase_iasync_result.h"
#include "mcsf_netbase_ibigdata_handler.h"
#include "mcsf_netbase_command_handler.h"
#include "mcsf_netbase_icmd_callback_handler.h"
#include "mcsf_netbase_icommunication_proxy.h"
#include "mcsf_netbase_ievent_handler.h"
#include "mcsf_netbase_isync_result.h"
#include "mcsf_communication_node_name.h"

#endif
