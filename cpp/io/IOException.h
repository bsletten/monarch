/*
 * Copyright (c) 2007-2011 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_io_IOException_H
#define monarch_io_IOException_H

#include "monarch/rt/Exception.h"

namespace monarch
{
namespace io
{

/**
 * An IOException is raised when some kind of IO error occurs.
 *
 * @author Dave Longley
 */
class IOException : public monarch::rt::Exception
{
public:
   /**
    * Creates a new IOException.
    *
    * A message, type, and code may be optionally specified.
    *
    * @param message the message for this Exception.
    * @param type the type for this Exception.
    */
   IOException(
      const char* message = "",
      const char* type = "monarch.io.IO");

   /**
    * Destructs this IOException.
    */
   virtual ~IOException();

   /**
    * Sets the number of used bytes (read/written) from an IO operation.
    *
    * @param used the number of used bytes (read or written).
    */
   virtual void setUsedBytes(int used);

   /**
    * Gets the number of used bytes (read/written) from an IO operation.
    *
    * @return the number of used bytes (read or written).
    */
   virtual int getUsedBytes();

   /**
    * Sets the number of unused bytes (unread/unwritten) from an IO operation.
    *
    * @param unused the number of unused bytes (unread or unwritten).
    */
   virtual void setUnusedBytes(int unused);

   /**
    * Gets the number of unused bytes (unread/unwritten) from an IO operation.
    *
    * @return the number of unused bytes (unread or unwritten).
    */
   virtual int getUnusedBytes();
};

} // end namespace io
} // end namespace monarch
#endif
