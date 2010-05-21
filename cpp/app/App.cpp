/*
 * Copyright (c) 2008-2010 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/app/App.h"

#include "monarch/app/AppRunner.h"

using namespace std;
using namespace monarch::app;
using namespace monarch::config;
using namespace monarch::kernel;
using namespace monarch::rt;

App::App() :
   mAppRunner(NULL)
{
}

App::~App()
{
   mAppRunner = NULL;
}

void App::setAppRunner(AppRunner* ar)
{
   mAppRunner = ar;
}

AppRunner* App::getAppRunner()
{
   return mAppRunner;
}

Config App::getConfig()
{
   return mAppRunner->getConfig();
}

ConfigManager* App::getConfigManager()
{
   return mAppRunner->getConfigManager();
}

MicroKernel* App::getKernel()
{
   return mAppRunner->getKernel();
}

bool App::initialize()
{
   return true;
}

void App::cleanup()
{
}

bool App::initConfigs(Config& defaults)
{
   return true;
}

DynamicObject App::getCommandLineSpec(Config& cfg)
{
   DynamicObject spec;
   spec["options"]->setType(Array);
   return spec;
}

bool App::willLoadConfigs()
{
   return true;
}

bool App::didLoadConfigs()
{
   return true;
}

DynamicObject App::getWaitEvents()
{
   DynamicObject rval;
   rval->setType(Array);
   return rval;
}

bool App::run()
{
   return true;
}
