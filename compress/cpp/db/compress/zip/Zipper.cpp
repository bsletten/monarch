/*
 * Copyright (c) 2008 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/compress/zip/Zipper.h"

using namespace db::compress::zip;
using namespace db::io;
using namespace db::rt;

Zipper::Zipper()
{
}

Zipper::~Zipper()
{
}

//Exception* Zipper::startEntry(ZipEntry& ze)
//{
//   Exception* rval = NULL;
//   
//   // FIXME:
//   
//   return rval;
//}

MutationAlgorithm::Result Zipper::mutateData(
   ByteBuffer* src, ByteBuffer* dst, bool finish)
{
   MutationAlgorithm::Result rval = MutationAlgorithm::Stepped;
   
   // FIXME:
   
   return rval;
}