////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_irefcount.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/04
///
/// \brief  implement an thread safe reference count strategy
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_IREFCOUNT_H
#define _MCSF_IREFCOUNT_H

#include "ace/Thread_Mutex.h"
#include "ace/Guard_T.h"
#include "mcsf_cpp_common.h"
#include "mcsf_container_log.h"
namespace Mcsf 
{
    class ISafeRefCount
    {
    public:  
        void Duplicate(int i=1) const
        {
            //NTB_TRACE_INFO_WITH_THIS("duplicate enter. this="<<this<<" m_iCount="<<m_iCount);
            ACE_GUARD( ACE_Thread_Mutex, localMutex, m_mutex );
            m_iCount+=i;
            //NTB_TRACE_INFO_WITH_THIS("duplicate exit. this="<<this<<" m_iCount="<<m_iCount);

        }

        bool Release(int i=1) const
        {
            //NTB_TRACE_INFO_WITH_THIS("release enter. this="<<this<<" m_iCount="<<m_iCount);
            ACE_GUARD_RETURN( ACE_Thread_Mutex, localMutex, m_mutex, false );
            m_iCount-=i;
            //NTB_TRACE_INFO_WITH_THIS("release exit. this="<<this<<" m_iCount="<<m_iCount);
            return (m_iCount == 0) ? true : false;

        }

        int GetCount() const {
            return m_iCount;
        }
    protected:
        ISafeRefCount() : m_iCount(1), m_mutex()  {
        }

        ~ISafeRefCount(){}

    private:
        DISALLOW_CLASS_COPY_AND_ASSIGN(ISafeRefCount);

        mutable int m_iCount;
        mutable ACE_Thread_Mutex m_mutex;
    };

    /// @brief: A base class for reference counted classes.
    /// To use this guy just extend your class from it like so:
    /// class MyFoo : public RefCounted<MyFoo> {
    ///  ...
    ///  private:
    ///   friend class RefCounted<MyFoo>;
    ///   ~MyFoo();
    /// };
    /// You should always make your destructor private, to avoid any code deleting
    /// the object accidently while there are references to it.
    template<class T>
    class RefCounted : public ISafeRefCount
    {
    public:
        RefCounted() {};
        ~RefCounted() {};

        void Duplicate(int i=1) const
        {
            ISafeRefCount::Duplicate(i);
        }

        void Release(int i=1) const 
        {
            if( ISafeRefCount::Release(i) )
            {
                delete static_cast<const T*>(this);
            }
        }
    private:
        DISALLOW_CLASS_COPY_AND_ASSIGN(RefCounted);
    };
}

#endif