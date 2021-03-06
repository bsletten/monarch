/*
 * Copyright (c) 2007-2011 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/io/IOException.h"

using namespace monarch::io;
using namespace monarch::rt;

IOException::IOException(const char* message, const char* type) :
   Exception(message, type)
{
}

IOException::~IOException()
{
}

void IOException::setUsedBytes(int used)
{
   getDetails()["usedBytes"] = used;
}

int IOException::getUsedBytes()
{
   return getDetails()["usedBytes"]->getInt32();
}

void IOException::setUnusedBytes(int unused)
{
   getDetails()["unusedBytes"] = unused;
}

int IOException::getUnusedBytes()
{
   return getDetails()["unusedBytes"]->getInt32();
}
