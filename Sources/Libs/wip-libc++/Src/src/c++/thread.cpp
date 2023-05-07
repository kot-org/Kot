//===------------------------- thread.cpp----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "__config"
#ifndef _LIBCPP_HAS_NO_THREADS

#include "thread"
#include "exception"
#include "vector"
#include "future"
#include "limits"

_LIBCPP_BEGIN_NAMESPACE_STD

thread::~thread()
{
    if (!__libcpp_thread_isnull(&__t_))
        terminate();

#ifdef _LIBCPP_HAS_THREAD_API_EXTERNAL
    __libcpp_thread_destroy(&__t_);
#endif
}

void
thread::join()
{
    int ec = EINVAL;
    if (!__libcpp_thread_isnull(&__t_))
    {
        ec = __libcpp_thread_join(&__t_);
        if (ec == 0)
            __t_ = _LIBCPP_NULL_THREAD;
    }

    if (ec)
        __throw_system_error(ec, "thread::join failed");
}

void
thread::detach()
{
    int ec = EINVAL;
    if (!__libcpp_thread_isnull(&__t_))
    {
        ec = __libcpp_thread_detach(&__t_);
        if (ec == 0)
            __t_ = _LIBCPP_NULL_THREAD;
    }

    if (ec)
        __throw_system_error(ec, "thread::detach failed");
}

unsigned
thread::hardware_concurrency() _NOEXCEPT
{
    return 0;
}

namespace this_thread
{

void
sleep_for(const chrono::nanoseconds& ns)
{
    if (ns > chrono::nanoseconds::zero())
    {
        __libcpp_thread_sleep_for(ns);
    }
}

}  // this_thread

__thread_specific_ptr<__thread_struct>&
__thread_local_data()
{
    static __thread_specific_ptr<__thread_struct> __p;
    return __p;
}

// __thread_struct_imp

template <class T>
class _LIBCPP_HIDDEN __hidden_allocator
{
public:
    typedef T  value_type;

    T* allocate(size_t __n)
        {return static_cast<T*>(::operator new(__n * sizeof(T)));}
    void deallocate(T* __p, size_t) {::operator delete(static_cast<void*>(__p));}

    size_t max_size() const {return size_t(~0) / sizeof(T);}
};

class _LIBCPP_HIDDEN __thread_struct_imp
{
    typedef vector<__assoc_sub_state*,
                          __hidden_allocator<__assoc_sub_state*> > _AsyncStates;
    typedef vector<pair<condition_variable*, mutex*>,
               __hidden_allocator<pair<condition_variable*, mutex*> > > _Notify;

    _AsyncStates async_states_;
    _Notify notify_;

    __thread_struct_imp(const __thread_struct_imp&);
    __thread_struct_imp& operator=(const __thread_struct_imp&);
public:
    __thread_struct_imp() {}
    ~__thread_struct_imp();

    void notify_all_at_thread_exit(condition_variable* cv, mutex* m);
    void __make_ready_at_thread_exit(__assoc_sub_state* __s);
};

__thread_struct_imp::~__thread_struct_imp()
{
    for (_Notify::iterator i = notify_.begin(), e = notify_.end();
            i != e; ++i)
    {
        i->second->unlock();
        i->first->notify_all();
    }
    for (_AsyncStates::iterator i = async_states_.begin(), e = async_states_.end();
            i != e; ++i)
    {
        (*i)->__make_ready();
        (*i)->__release_shared();
    }
}

void
__thread_struct_imp::notify_all_at_thread_exit(condition_variable* cv, mutex* m)
{
    notify_.push_back(pair<condition_variable*, mutex*>(cv, m));
}

void
__thread_struct_imp::__make_ready_at_thread_exit(__assoc_sub_state* __s)
{
    async_states_.push_back(__s);
    __s->__add_shared();
}

// __thread_struct

__thread_struct::__thread_struct()
    : __p_(new __thread_struct_imp)
{
}

__thread_struct::~__thread_struct()
{
    delete __p_;
}

void
__thread_struct::notify_all_at_thread_exit(condition_variable* cv, mutex* m)
{
    __p_->notify_all_at_thread_exit(cv, m);
}

void
__thread_struct::__make_ready_at_thread_exit(__assoc_sub_state* __s)
{
    __p_->__make_ready_at_thread_exit(__s);
}

_LIBCPP_END_NAMESPACE_STD

#endif // !_LIBCPP_HAS_NO_THREADS
