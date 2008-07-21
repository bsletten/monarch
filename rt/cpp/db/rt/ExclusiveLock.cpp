/*
 * Copyright (c) 2007-2008 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/rt/ExclusiveLock.h"

#include "db/rt/Thread.h"

using namespace db::rt;

ExclusiveLock::ExclusiveLock()
{
}

ExclusiveLock::~ExclusiveLock()
{
}

inline void ExclusiveLock::lock()
{
   mMonitor.enter();
}

inline void ExclusiveLock::unlock()
{
   mMonitor.exit();
}

inline void ExclusiveLock::notify()
{
   mMonitor.notify();
}

inline void ExclusiveLock::notifyAll()
{
   mMonitor.notify();
}

inline bool ExclusiveLock::wait(uint32_t timeout)
{
   // instruct the current thread to wait to enter this Object's monitor
   return Thread::waitToEnter(&mMonitor, timeout);
}