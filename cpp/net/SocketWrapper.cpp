/*
 * Copyright (c) 2007-2011 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/net/SocketWrapper.h"

using namespace monarch::io;
using namespace monarch::net;

SocketWrapper::SocketWrapper(Socket* socket, bool cleanup)
{
   setSocket(socket, cleanup);
}

SocketWrapper::~SocketWrapper()
{
   // cleanup wrapped socket as appropriate
   if(mCleanupSocket)
   {
      delete mSocket;
   }
}

void SocketWrapper::setSocket(Socket* socket, bool cleanup)
{
   mSocket = socket;
   mCleanupSocket = cleanup;
}

inline Socket* SocketWrapper::getSocket()
{
   return mSocket;
}

inline bool SocketWrapper::mustCleanupSocket()
{
   return mCleanupSocket;
}

inline bool SocketWrapper::bind(SocketAddress* address)
{
   return getSocket()->bind(address);
}

inline bool SocketWrapper::listen(int backlog)
{
   return getSocket()->listen(backlog);
}

inline Socket* SocketWrapper::accept(int timeout)
{
   return getSocket()->accept(timeout);
}

inline bool SocketWrapper::connect(SocketAddress* address, int timeout)
{
   return getSocket()->connect(address, timeout);
}

inline bool SocketWrapper::send(const char* b, int length)
{
   return getSocket()->send(b, length);
}

inline int SocketWrapper::receive(char* b, int length)
{
   return getSocket()->receive(b, length);
}

inline void SocketWrapper::close()
{
   getSocket()->close();
}

inline bool SocketWrapper::isBound()
{
   return getSocket()->isBound();
}

inline bool SocketWrapper::isListening()
{
   return getSocket()->isListening();
}

inline bool SocketWrapper::isConnected()
{
   return getSocket()->isConnected();
}

inline bool SocketWrapper::getLocalAddress(SocketAddress* address)
{
   return getSocket()->getLocalAddress(address);
}

inline bool SocketWrapper::getRemoteAddress(SocketAddress* address)
{
   return getSocket()->getRemoteAddress(address);
}

inline InputStream* SocketWrapper::getInputStream()
{
   return getSocket()->getInputStream();
}

inline OutputStream* SocketWrapper::getOutputStream()
{
   return getSocket()->getOutputStream();
}

inline void SocketWrapper::setSendTimeout(uint32_t timeout)
{
   getSocket()->setSendTimeout(timeout);
}

inline uint32_t SocketWrapper::getSendTimeout()
{
   return getSocket()->getSendTimeout();
}

inline void SocketWrapper::setReceiveTimeout(uint32_t timeout)
{
   getSocket()->setReceiveTimeout(timeout);
}

inline uint32_t SocketWrapper::getReceiveTimeout()
{
   return getSocket()->getReceiveTimeout();
}

inline int SocketWrapper::getBacklog()
{
   return getSocket()->getBacklog();
}

inline int SocketWrapper::getFileDescriptor()
{
   return getSocket()->getFileDescriptor();
}

inline SocketAddress::CommunicationDomain
   SocketWrapper::getCommunicationDomain()
{
   return getSocket()->getCommunicationDomain();
}

inline void SocketWrapper::setSendNonBlocking(bool on)
{
   getSocket()->setSendNonBlocking(on);
}

inline bool SocketWrapper::isSendNonBlocking()
{
   return getSocket()->isSendNonBlocking();
}

inline void SocketWrapper::setReceiveNonBlocking(bool on)
{
   getSocket()->setReceiveNonBlocking(on);
}

inline bool SocketWrapper::isReceiveNonBlocking()
{
   return getSocket()->isReceiveNonBlocking();
}
