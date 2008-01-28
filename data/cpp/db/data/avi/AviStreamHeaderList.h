/*
 * Copyright (c) 2007-2008 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_data_avi_AviStreamHeaderList_H
#define db_data_avi_AviStreamHeaderList_H

#include "db/data/avi/AviStreamData.h"
#include "db/data/avi/AviStreamFormat.h"
#include "db/data/avi/AviStreamHeader.h"
#include "db/data/riff/RiffListHeader.h"
#include "db/io/OutputStream.h"

namespace db
{
namespace data
{
namespace avi
{

/**
 * An AVI Stream Header List ('strl').
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
 * The Stream Header List 'strl' applies to the first stream in the 'movi'
 * LIST, the second applies to the second stream, etc.
 * 
 * @author Dave Longley
 * @author David I. Lehn
 */
class AviStreamHeaderList
{
public:
   /**
    * Chunk fourcc id "strl".
    */
   static const fourcc_t CHUNK_ID = DB_FOURCC_FROM_CHARS('s','t','r','l');

protected:
   /**
    * The AVI stream header RIFF header.
    */
   db::data::riff::RiffListHeader mRiffHeader;
   
   /**
    * The Stream Header.
    */
   AviStreamHeader mStreamHeader;
   
   /**
    * The Stream Format.
    */
   AviStreamFormat mStreamFormat;
   
   /**
    * The Stream Data.
    */
   AviStreamData mStreamData;
   
public:
   /**
    * Constructs a new AviStreamHeaderList.
    */
   AviStreamHeaderList();
   
   /**
    * Destructs a AviStreamHeaderList.
    */
   virtual ~AviStreamHeaderList();
   
   /**
    * Writes this AviStreamHeaderList, including the RIFF header, to an
    * OutputStream.
    * 
    * @param os the OutputStream to write to.
    * 
    * @exception true on success, false on an IOException.
    */
   virtual bool writeTo(db::io::OutputStream& os);
   
   /**
    * Converts this AviStreamHeaderList from a byte array.
    * 
    * @param b the byte array to convert from.
    * @param length the number of valid bytes in the buffer.
    * 
    * @return true if successful, false if not.
    */
   virtual bool convertFromBytes(const char* b, int length);
   
   /**
    * Returns whether or not this AviStreamHeaderList is valid.
    * 
    * @return true if valid, false if not.
    */
   virtual bool isValid();
   
   /**
    * Gets the size of this AviStreamHeaderList, excluding its chunk header.
    * 
    * @return the size of this AviStreamHeaderList chunk.
    */
   virtual int getListSize();
   
   /**
    * Gets the size of this AviStreamHeaderList, including its chunk header.
    * 
    * @return the size of this AviStreamHeaderList.
    */
   virtual int getSize();
};

} // end namespace avi
} // end namespace data
} // end namespace db
#endif