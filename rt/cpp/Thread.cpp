/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "Thread.h"
#include "System.h"
#include "Math.h"

using namespace std;
using namespace db::rt;

// initialize current thread key parameters
pthread_once_t Thread::CURRENT_THREAD_KEY_INIT = PTHREAD_ONCE_INIT;
pthread_key_t Thread::CURRENT_THREAD_KEY;

// initialize exception key parameters
pthread_once_t Thread::EXCEPTION_KEY_INIT = PTHREAD_ONCE_INIT;
pthread_key_t Thread::EXCEPTION_KEY;

// initialize interruption lock and SIGINT value
Object Thread::INTERRUPTION_LOCK;
bool Thread::SIGINT_RECEIVED = false;

Thread::Thread(Runnable* runnable, std::string name)
{
   // initialize POSIX thread attributes
   pthread_attr_init(&mPThreadAttributes);
   
   // make thread joinable
   pthread_attr_setdetachstate(&mPThreadAttributes, PTHREAD_CREATE_JOINABLE);
   
   // make thread cancelable upon joins/waits/etc
   pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
   pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
   
   // thread not waiting to enter a Monitor yet
   mWaitMonitor = NULL;
   
   // store runnable
   mRunnable = runnable;
   
   // set name
   mName = name;
   
   // thread is not alive yet
   mAlive = false;
   
   // thread not detached yet
   mDetached = false;
   
   // thread is not interrupted yet
   mInterrupted = false;
   
   // thread has not joined yet
   mJoined = false;
   
   // thread is not started yet
   mStarted = false;
}

Thread::~Thread()
{
   // destroy the POSIX thread attributes
   pthread_attr_destroy(&mPThreadAttributes);
}

void Thread::run()
{
   // if a Runnable if available, use it
   if(mRunnable != NULL)
   {
      mRunnable->run();
   }
}

void Thread::createCurrentThreadKey()
{
   // create the thread key for obtaining the current thread
   pthread_key_create(&CURRENT_THREAD_KEY, NULL);
}

void Thread::createExceptionKey()
{
   // create the thread key for obtaining the last thread-local exception
   pthread_key_create(&EXCEPTION_KEY, NULL);
}

void* Thread::execute(void* thread)
{
//   // create signal set containing only SIGINT
//   sigset_t newset;
//   sigemptyset(newset);
//   sigaddset(&newset, SIGINT);
//   
//   // block all (and only) SIGINT signals
//   sigset_t oldset;
//   sigthreadmask(SIG_SETMASK, &newset, &oldset);
   
   // get the Thread object
   Thread* t = (Thread*)thread;
   
   // create the current thread key, if not created
   pthread_once(&CURRENT_THREAD_KEY_INIT, Thread::createCurrentThreadKey);
   
   // create the exception key, if not created
   pthread_once(&EXCEPTION_KEY_INIT, Thread::createExceptionKey);
   
   // set thread specific data for current thread to the Thread
   pthread_setspecific(CURRENT_THREAD_KEY, t);
   
   // set thread specific data for exception to NULL (no exception yet)
   pthread_setspecific(EXCEPTION_KEY, NULL);
   
   // thread is alive
   t->mAlive = true;
   
   // run the passed thread's run() method
   t->run();
   
   // thread is no longer alive
   t->mAlive = false;
   
   // clean up any exception
   setException(NULL);   
   
   // detach thread
   t->detach();
   
//   // reblock old signals
//   sigthreadmask(SIG_SETMASK, &oldset, NULL);
   
   // exit thread
   pthread_exit(NULL);
   return NULL;
}

void Thread::handleSigInt(int signum)
{
   // SIGINT received
   SIGINT_RECEIVED = true;
}

bool Thread::start()
{
   bool rval = false;
   
   if(!hasStarted())
   {
      // create the POSIX thread
      int rc = pthread_create(
         &mPThread, &mPThreadAttributes, execute, (void*)this);
         
      // if the thread was created successfully, return true
      if(rc == 0)
      {
         // thread has started
         mStarted = true;
         rval = true;
      }
   }
   
   return rval;
}

bool Thread::isAlive()
{
   return mAlive;
}

void Thread::interrupt()
{
   // synchronize
   lock();
   {
      // only interrupt if not already interrupted
      if(!isInterrupted())
      {
         // set interrupted flag
         mInterrupted = true;
         
         // send SIGINT to thread
         pthread_kill(mPThread, SIGINT);
         
         // wake up thread if necessary
         if(mWaitMonitor != NULL)
         {
            mWaitMonitor->signalAll();
         }
      }
   }
   unlock();
}

bool Thread::isInterrupted()
{
   return mInterrupted;
}

bool Thread::hasStarted()
{
   return mStarted;
}

void Thread::join()
{
   bool join = false;
   
   // synchronize
   lock();
   {
      // check for previous detachments/joins
      if(!mDetached && !mJoined)
      {
         join = true;
         mJoined = true;
      }
   }
   unlock();
   
   if(join)
   {
      // join thread, wait for it to detach/terminate indefinitely
      int status;
      pthread_join(mPThread, (void **)&status);
   }
}

void Thread::detach()
{
   bool detach = false;
   
   // synchronize
   lock();
   {
      // check for previous detachments/joins
      if(!mDetached && !mJoined)
      {
         detach = true;
         mDetached = true;
      }
   }
   unlock();
   
   if(detach)
   {
      // detach thread
      pthread_detach(mPThread);
   }
}

void Thread::setName(string name)
{
   mName = name;
}

const string& Thread::getName()
{
   return mName;
}

Thread* Thread::currentThread()
{
   // get a pointer to the current thread
   return (Thread*)pthread_getspecific(CURRENT_THREAD_KEY);
}

bool Thread::interrupted(bool clear)
{
   bool rval = false;
   
   // get the current thread's interrupted status
   Thread* t = Thread::currentThread();
   if(t != NULL)
   {
      // synchronize
      t->lock();
      {
         if(t->isInterrupted())
         {
            rval = true;
         }
         else
         {
            // FIXME: this code was put on hold because windows hates it
            // -- we would also need to install the handler before select()
            // calls and unblock SIGINT at that point, which could be done
            // by adding a "select" like call to Thread for ease of use
            
//            // synchronize on interruption lock
//            INTERRUPTION_LOCK.lock();
//            {
//               // create the SIGINT handler
//               sigaction newsa;
//               newsa.sa_handler = handleSigInt;
//               newsa.sa_flags = 0;
//               sigemptyset(&newsa.sa_mask);
//               
//               // set the SIGINT handler
//               sigaction oldsa;
//               sigaction(SIGINT, &newsa, &oldsa);
//               
//               // unblock all signals on this thread
//               sigset_t newset;
//               sigemptyset(newset);
//               sigset_t oldset;
//               sigthreadmask(SIG_SETMASK, &newset, &oldset);
//               
//               // SIGINT, if any, will be handled here
//               if(SIGINT_RECEIVED)
//               {
//                  // SIGINT received, thread is interrupted
//                  rval = t->mInterrupted = true;
//                  SIGINT_RECEIVED = false;
//               }
//               
//               // reblock old signals
//               sigthreadmask(SIG_SETMASK, &oldset, NULL);
//               
//               // restore old SIGINT handler
//               sigaction(SIGINT, &oldsa, NULL);
//            }
//            INTERRUPTION_LOCK.unlock();
            
            if(clear)
            {
               // clear interrupted flag
               t->mInterrupted = false;
            }
         }
      }
      t->unlock();
   }
   
   return rval;
}

InterruptedException* Thread::sleep(unsigned long time)
{
   InterruptedException* rval = NULL;
   
   // create a lock object
   Object lock;
   
   lock.lock();
   {
      // wait on the lock object for the specified time
      rval = lock.wait(time);
   }
   lock.unlock();
   
   return rval;
}

void Thread::yield()
{
   sched_yield();
}

void Thread::setException(Exception* e)
{
   if(currentThread() != NULL)
   {
      // get the existing exception for the current thread, if any
      Exception* existing = getException();
      if(existing != e)
      {
         // replace the existing exception
         pthread_setspecific(EXCEPTION_KEY, e);
         
         // delete the old exception
         delete existing;
      }
   }
   else if(e != NULL)
   {
      // delete passed exception, since we are not on a thread
      delete e;
   }
}

Exception* Thread::getException()
{
   Exception* rval = NULL;
   
   if(currentThread() != NULL)
   {
      // get the exception for the current thread, if any
      rval = (Exception*)pthread_getspecific(EXCEPTION_KEY);
   }
   
   return rval;
}

bool Thread::hasException()
{
   return getException() != NULL;
}

void Thread::clearException()
{
   setException(NULL);
}

InterruptedException* Thread::waitToEnter(Monitor* m, unsigned long timeout)
{
   InterruptedException* rval = NULL;
   
   Thread* t = currentThread();
   if(t != NULL)
   {
      // set the current thread's wait monitor
      t->mWaitMonitor = m;
      
      // get the current time and determine if wait should be indefinite
      unsigned long long time = System::getCurrentMilliseconds();
      bool indefinite = (timeout == 0);
      
      // wait while not interrupted, must wait, and timeout not exhausted
      while(!t->isInterrupted() && m->mustWait() && (indefinite || timeout > 0))
      {
         m->wait(timeout);
         time = (System::getCurrentMilliseconds() - time);
         timeout -= (time > timeout) ? timeout : time;
      }
      
      // clear the current thread's wait monitor
      t->mWaitMonitor = NULL;
      
      // create interrupted exception if interrupted
      if(t->isInterrupted())
      {
         rval = new InterruptedException(
            "Thread '" + t->getName() + "' interrupted");
      }
   }
   
   // set exception
   setException(rval);
   
   return rval;
}
