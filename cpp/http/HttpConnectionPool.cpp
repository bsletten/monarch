/*
 * Copyright (c) 2009 Digital Bazaar, Inc. All rights reserved.
 */
#include "db/http/HttpConnectionPool.h"

#include <algorithm>

using namespace std;
using namespace db::http;
using namespace db::net;
using namespace db::rt;
using namespace db::util;

HttpConnectionPool::HttpConnectionPool()
{
}

HttpConnectionPool::~HttpConnectionPool()
{
   for(PoolMap::iterator i = mPools.begin(); i != mPools.end(); i++)
   {
      free((char*)i->first);
      delete i->second;
   }
}

static string _getUrlKey(Url* url)
{
   // build url key
   string key = url->getSchemeHostAndPort();
   key = StringTools::toLower(key.c_str());
   return key;
}

void HttpConnectionPool::addConnection(Url* url, HttpConnectionRef conn)
{
   mPoolsLock.lock();
   {
      // get url key
      string key = _getUrlKey(url);

      // find existing connection list
      HttpConnectionList* pool = NULL;
      PoolMap::iterator i = mPools.find(key.c_str());
      if(i != mPools.end())
      {
         pool = i->second;
      }
      else
      {
         // no existing list, so create one
         pool = new HttpConnectionList;
         mPools.insert(make_pair(strdup(key.c_str()), pool));
      }

      // add connection to pool
      pool->push_back(conn);
   }
   mPoolsLock.unlock();
}

HttpConnectionRef HttpConnectionPool::getConnection(Url* url)
{
   HttpConnectionRef rval(NULL);

   mPoolsLock.lock();
   {
      // get url key
      string key = _getUrlKey(url);

      // get existing connection list
      PoolMap::iterator i = mPools.find(key.c_str());
      if(i != mPools.end())
      {
         // keep popping connections until one that isn't closed is found
         while(rval.isNull() && !i->second->empty())
         {
            rval = i->second->front();
            i->second->pop_front();

            // drop connection if its closed
            if(rval->isClosed())
            {
               rval.setNull();
            }
         }

         if(rval.isNull())
         {
            // no idle connection available, drop connection list
            mPools.erase(i);
         }
      }
   }
   mPoolsLock.unlock();

   return rval;
}
