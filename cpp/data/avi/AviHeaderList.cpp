/*
 * Copyright (c) 2007-2010 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/data/avi/AviHeaderList.h"

using namespace std;
using namespace monarch::data;
using namespace monarch::data::avi;
using namespace monarch::data::riff;
using namespace monarch::io;

AviHeaderList::AviHeaderList() :
   mRiffHeader(CHUNK_ID)
{
}

AviHeaderList::~AviHeaderList()
{
   freeStreamHeaderLists();
}

bool AviHeaderList::writeTo(OutputStream& os)
{
   bool rval;

   // write RIFF header
   rval = mRiffHeader.writeTo(os);

   if(rval)
   {
      // write the main AVI header
      rval = mMainHeader.writeTo(os);
   }

   // write each stream header list
   for(StreamHeaderLists::iterator i = mStreamHeaderLists.begin();
       i != mStreamHeaderLists.end() && rval; ++i)
   {
      AviStreamHeaderList* hl = *i;
      rval = hl->writeTo(os);
   }

   return rval;
}

bool AviHeaderList::convertFromBytes(const char* b, int length)
{
   bool rval = false;

   // convert the header
   int offset = 0;
   if(mRiffHeader.convertFromBytes(b + offset, length) &&
      mRiffHeader.getIdentifier() == CHUNK_ID)
   {
      // step forward past RIFF header
      offset += RiffListHeader::HEADER_SIZE;
      length -= RiffListHeader::HEADER_SIZE;

      // convert main header
      if(mMainHeader.convertFromBytes(b + offset, length))
      {
         // main header converted
         rval = true;

         // ensure there is enough data remaining to convert the header list
         if(length >= (int)mRiffHeader.getListSize())
         {
            // set length to size of list
            length = (int)mRiffHeader.getListSize();

            // move past header
            offset += mMainHeader.getSize();
            length -= mMainHeader.getSize();

            // convert all stream header lists
            while(length > 0)
            {
               AviStreamHeaderList* list = new AviStreamHeaderList();
               if(list->convertFromBytes(b + offset, length))
               {
                  MO_CAT_DEBUG_DETAIL(MO_DATA_CAT, "AviHeaderList: "
                     "AviStreamHeaderList read, size=%d",
                     list->getListSize());

                  // append list
                  mStreamHeaderLists.push_back(list);

                  // move to next stream header list
                  offset += list->getSize();
                  length -= list->getSize();
               }
               else
               {
                  // invalid stream header list
                  delete list;
                  MO_CAT_ERROR(MO_DATA_CAT, "AviHeaderList: "
                     "Invalid stream header list detected at offset %d",
                     offset);
                  break;
               }
            }
         }
      }
   }

   return rval;
}

bool AviHeaderList::isValid()
{
   return mRiffHeader.isValid() &&
      mRiffHeader.getIdentifier() == CHUNK_ID && mMainHeader.isValid();
}

int AviHeaderList::getSize()
{
   // AVI header list is expected to be under 32-bits
   return (int)mRiffHeader.getListSize() + RiffListHeader::HEADER_SIZE;
}

AviHeader& AviHeaderList::getMainHeader()
{
   return mMainHeader;
}

std::vector<AviStreamHeaderList*>& AviHeaderList::getStreamHeaderLists()
{
   return mStreamHeaderLists;
}

void AviHeaderList::freeStreamHeaderLists()
{
   for(StreamHeaderLists::iterator i = mStreamHeaderLists.begin();
       i != mStreamHeaderLists.end(); ++i)
   {
      delete *i;
   }
   mStreamHeaderLists.clear();
}
