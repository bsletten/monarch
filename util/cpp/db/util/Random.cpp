/*
 * Copyright (c) 2008 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/util/Random.h"

#include <stdlib.h>

using namespace db::util;

uint64_t Random::next(uint64_t low, uint64_t high)
{
   // get a random number between 1 and 1000000000
   return low + (uint64_t)((long double)high * (rand() / (RAND_MAX + 1.0)));
}