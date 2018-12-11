//////////////////////////////////////////////////////////////////////////
/// \defgroup uAI
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2018
///  All rights reserved.
///
///  \author  qiangqiang.zhou     mailto:qiangqiang.zhou@united-imaging.com
///
///  \file    uAI App defs
///  \brief   define the common macros
///
///  \version 1.0
///  \date    June 15, 2018
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef UAI_APP_DEFS_H_
#define UAI_APP_DEFS_H_

#ifdef UAI_LIB
#define UAI_EXPORT __declspec (dllexport)
#else //!defined(UAI_LIB)
#define UAI_EXPORT __declspec (dllimport)
#endif

#define BEGIN_NAMESPACE namespace uai {
#define END_NAMESPACE }

// A macro to delete pointer and assign it to NULL
#ifndef DEL_PTR
#define DEL_PTR(ptr) \
	do \
	{ \
	if (NULL != ptr) \
		{ \
		delete ptr; \
		ptr = NULL; \
		} \
	} while (false)
#endif


// A macro to delete pointer array and assign it to NULL
#ifndef DEL_ARRAY
#define DEL_ARRAY(ptr) \
	do \
	{ \
	if (NULL != ptr) \
		{ \
		delete[] ptr; \
		ptr = NULL; \
		} \
	} while (false)
#endif

#define DISALLOW_COPY(ClassName) \
	ClassName(const ClassName &)
#define DISALLOW_ASSIGN(ClassName) \
	const ClassName &operator = (const ClassName &)
#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
	DISALLOW_COPY(ClassName);               \
	DISALLOW_ASSIGN(ClassName)               // no semicolon here
#endif //UAI_APP_DEFS_H_
