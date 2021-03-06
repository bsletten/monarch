/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_data_riff_RiffChunkHeader_H
#define monarch_data_riff_RiffChunkHeader_H

#include <inttypes.h>

#include "monarch/data/Data.h"
#include "monarch/io/OutputStream.h"

namespace monarch
{
namespace data
{
namespace riff
{

/**
 * A RIFF Chunk. The chunk size does *NOT* include the header size.
 *
 * The format of a RIFF Chunk is such:
 *
 * | bytes 1-4  | bytes 5-8  |  bytes 9+  |
 * ----------------------------------------
 * | identifier | chunk size | chunk data |
 *
 * The identifier is a FOURCC (a four-character code).
 * The chunk size a little-endian (least sig. first) 32-bit character.
 *
 * @author Dave Longley
 * @author David I. Lehn
 */
class RiffChunkHeader
{
public:
   /**
    * The header size for a RIFF chunk.
    */
   static const int HEADER_SIZE = 8;

protected:
   /**
    * The 4 byte chunk identifier. "JUNK" for junk chunks.
    */
   fourcc_t mId;

   /**
    * The size of the chunk.
    */
   uint32_t mChunkSize;

   /**
    * Whether or not this chunk is valid.
    */
   bool mValid;

public:
   /**
    * Constructs a Riff Chunk Header with the passed parameters.
    *
    * @param id the identifier of the chunk.
    * @param size the size of the chunk.
    */
   RiffChunkHeader(fourcc_t id = 0, int size = 0);

   /**
    * Destructs a Riff Chunk Header.
    */
   virtual ~RiffChunkHeader();

   /**
    * Writes this RiffChunkHeader to an OutputStream.
    *
    * @param os the OutputStream to write to.
    *
    * @return true on success, false if an IO error occurs.
    */
   virtual bool writeTo(monarch::io::OutputStream& os);

   /**
    * Converts the chunk header into an 8 byte array.
    *
    * @return the chunk header into an 8 byte array.
    */
   virtual void convertToBytes(char* b);

   /**
    * Converts the chunk header from a byte array with at least 8 bytes.
    *
    * @param b the byte array to convert from.
    * @param length the number of valid bytes in the buffer.
    *
    * @return true if successful, false if not.
    */
   virtual bool convertFromBytes(const char* b, int length);

   /**
    * Sets the fourcc identifier for this chunk.
    *
    * @param id the identifier to set.
    *
    * @return true if set, false if not.
    */
   virtual void setIdentifier(fourcc_t id);

   /**
    * Gets the fourcc identifier for this chunk.
    *
    * @return the chunk identifier.
    */
   virtual fourcc_t getIdentifier();

   /**
    * Sets the size. This is the size of the chunk.
    *
    * @param size the size to use.
    */
   virtual void setChunkSize(uint32_t size);

   /**
    * Returns the chunk size as reported in the chunk.
    *
    * @return the chunk size.
    */
   virtual uint32_t getChunkSize();

   /**
    * Returns the chunk size as rounded up to the nearest even-number.
    *
    * @return the padded chunk size.
    */
   virtual uint32_t getPaddedSize();

   /**
    * Returns the total chunk size, including its header, and rounded up
    * to the nearest even-number.
    *
    * @return the total padded chunk size.
    */
   virtual uint32_t getTotalPaddedSize();

   /**
    * Returns whether or not this chunk is valid.
    *
    * @return true if valid, false if not.
    */
   virtual bool isValid();

   /**
    * Sets whether or not this chunk is valid.
    *
    * @param valid true to set to valid, false to set to invalid.
    */
   virtual void setValid(bool valid);
};

} // end namespace riff
} // end namespace data
} // end namespace monarch
#endif
