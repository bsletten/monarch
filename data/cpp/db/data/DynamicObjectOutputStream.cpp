/*
 * Copyright (c) 2008 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/data/DynamicObjectOutputStream.h"

using namespace db::data;
using namespace db::io;
using namespace db::util;

DynamicObjectOutputStream::DynamicObjectOutputStream(
   DynamicObject& dyno, DynamicObjectReader* reader, bool cleanup) :
   mInputStream(NULL, 0)
{
   mReader = reader;
   mCleanupReader = cleanup;
   
   // start DynamicObjectReader
   mReader->start(dyno);
}

DynamicObjectOutputStream::~DynamicObjectOutputStream()
{
   if(mCleanupReader && mReader != NULL)
   {
      delete mReader;
   }
}

bool DynamicObjectOutputStream::write(const char* b, int length)
{
   // set input stream byte array
   mInputStream.setByteArray(b, length);
   
   // use reader
   return mReader->read(&mInputStream) == NULL;
}

void DynamicObjectOutputStream::close()
{
   // finish reader
   mReader->finish();
}
