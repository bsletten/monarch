/*
 * Copyright (c) 2007-2008 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_data_avi_AviHeaderList_H
#define db_data_avi_AviHeaderList_H

#include <list>

#include "db/data/avi/AviHeader.h"
#include "db/data/avi/AviStreamHeaderList.h"
#include "db/data/riff/RiffListHeader.h"
#include "db/io/OutputStream.h"
#include "db/logging/Logging.h"

namespace db
{
namespace data
{
namespace avi
{

/**
 * An AVI Header List. This list contains an AVI main header and stream
 * header lists.
 * 
 * AVI Format is as follows:
 * 
 * AVI Form Header ('RIFF' size 'AVI ' data)
 *    Header List ('LIST' size 'hdrl' data)
 *       AVI Header ('avih' size data)
 *       Video Stream Header List ('LIST' size 'strl' data)
 *          Video Stream Header ('strh' size data)
 *          Video Stream Format ('strf' size data)
 *          Video Stream Data ('strd' size data) - IGNORED, for DRIVERS
 *       Audio Stream Header List ('LIST' size 'strl' data)
 *          Audio Stream Header ('strh' size data)
 *          Audio Stream Format ('strf' size data)
 *          Audio Stream Data ('strd' size data) - IGNORED, for DRIVERS
 *    Info List ('LIST' size 'INFO' data)
 *       Index Entry ({'ISBJ','IART','ICMT',...} size data)
 *    Movie List ('LIST' size 'movi' data)
 *       Movie Entry ({'00db','00dc','01wb'} size data)
 *    Index Chunk ('idx1' size data)
 *       Index Entry ({'00db','00dc','01wb',...})
 * 
 * @author Dave Longley
 * @author David I. Lehn
 */
class AviHeaderList
{
public:
   /**
    * Chunk fourcc id "hrdl".
    */
   static const fourcc_t CHUNK_ID = DB_FOURCC_FROM_CHARS('h','r','d','l');
   
protected:
   /**
    * The Header List RIFF header.
    */
   db::data::riff::RiffListHeader mRiffHeader;
   
   /**
    * The main AviHeader for this header list.
    */
   AviHeader mMainHeader;
   
   /**
    * The AviStreamHeaderLists in this header list.
    */
   std::list<AviStreamHeaderList> mStreamHeaderLists;
   
public:
   /**
    * Constructs a new AviHeaderList.
    */
   AviHeaderList();
   
   /**
    * Destructs a AviHeaderList.
    */
   virtual ~AviHeaderList();

   /**
    * Writes this AviHeaderList, including the RIFF header, to an OutputStream.
    * 
    * @param os the OutputStream to write to.
    * 
    * @return true on success, false on an IOException.
    */
   virtual bool writeTo(db::io::OutputStream& os);
   
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
    * Returns whether or not this AviHeaderList is valid.
    * 
    * @return true if valid, false if not.
    */
   virtual bool isValid();
   
   /**
    * Gets the size of this AviHeaderList, including its RIFF header.
    * 
    * @return the size of this AviHeaderList.
    */
   virtual int getSize();
   
   /**
    * Gets the main AviHeader.
    * 
    * @return the main AviHeader.
    */
   virtual AviHeader& getMainHeader();
};

} // end namespace avi
} // end namespace data
} // end namespace db
#endif