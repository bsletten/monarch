/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_modest_ModuleLoader_H
#define db_modest_ModuleLoader_H

#include "Module.h"

namespace db
{
namespace modest
{

/**
 * A ModuleLoader is used to load (and unload) Modules.
 * 
 * @author Dave Longley
 */
class ModuleLoader
{
public:
   /**
    * Creates a new ModuleLoader.
    */
   ModuleLoader();
   
   /**
    * Destructs this ModuleLoader.
    */
   virtual ~ModuleLoader();
};

} // end namespace modest
} // end namespace db
#endif
