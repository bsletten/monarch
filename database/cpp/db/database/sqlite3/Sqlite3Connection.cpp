/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/database/sqlite3/Sqlite3Connection.h"
#include "db/database/sqlite3/Sqlite3Statement.h"

#include <iostream>

using namespace std;
using namespace db::database;
using namespace db::database::sqlite3;

Sqlite3Connection::Sqlite3Connection(const char* params) :
   Connection(params)
{
   int ret;
   mHandle = NULL;

   if(mDatabaseParams->getScheme() != "sqlite3")
   {
      // FIXME error handling
   }
   else
   {
      string dbFile = mDatabaseParams->getSchemeSpecificPart();
      ret = sqlite3_open(dbFile.c_str(), &mHandle);
      if(ret != SQLITE_OK)
      {
         Sqlite3Connection::close();
         // FIXME error handling
      }
   }
}

Sqlite3Connection::~Sqlite3Connection()
{
   Sqlite3Connection::close();
}

Statement* Sqlite3Connection::prepareStatement(const char* sql)
{
   return new Sqlite3Statement(this, sql);
}

void Sqlite3Connection::close()
{
   if(mHandle != NULL)
   {
      sqlite3_close(mHandle);
      mHandle = NULL;
   }
}

void Sqlite3Connection::commit()
{
   // FIXME:
   cout << "FIXME: commit" << endl;
}

void Sqlite3Connection::rollback()
{
   // FIXME:
   cout << "FIXME: rollback" << endl;
}
