/*
 * Copyright (c) 2007-2012 Digital Bazaar, Inc. All rights reserved.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __STDC_CONSTANT_MACROS

#include <cstdio>

#include "monarch/test/Test.h"
#include "monarch/test/TestModule.h"
#include "monarch/data/json/JsonLd.h"
#include "monarch/data/json/JsonReader.h"
#include "monarch/data/json/JsonWriter.h"
#include "monarch/logging/Logging.h"
#include "monarch/io/File.h"
#include "monarch/io/FileInputStream.h"
#include "monarch/io/FileList.h"
#include "monarch/util/StringTools.h"
#include "monarch/validation/Validation.h"

using namespace std;
using namespace monarch::test;
using namespace monarch::data::json;
using namespace monarch::io;
using namespace monarch::rt;
using namespace monarch::util;
namespace v = monarch::validation;

namespace mo_test_jsonld
{

// hack just so code compiles even w/o test suite support
#ifndef HAVE_JSON_LD_TEST_SUITE
#define JSON_LD_TEST_SUITE_DIR ""
#endif

// declare
static void _runJsonLdTestSuiteFromPath(TestRunner& tr, const char* path);

static void _readFile(const char* root, const char* name, DynamicObject& data)
{
   string fullPath;
   fullPath.assign(File::join(root, name));

   File f(fullPath.c_str());
   if(!f->exists() || !f->isFile())
   {
      ExceptionRef e = new Exception("Invalid test file.");
      e->getDetails()["name"] = name;
      Exception::set(e);
   }
   assertNoExceptionSet();
   FileInputStream is(f);
   JsonReader r(false);
   r.start(data);
   bool success = r.read(&is) && r.finish();
   if(!success)
   {
      ExceptionRef e = new Exception("Failure reading test file.");
      e->getDetails()["name"] = name;
      Exception::push(e);
   }
   assertNoExceptionSet();
   is.close();
}

static void _runJsonLdTestSuiteTest(
   TestRunner& tr, const char* root, DynamicObject& test)
{
   // flag to check if warning should be printed
   bool skipped = false;

   tr.test(test["name"]);

   // read input
   DynamicObject input;
   _readFile(root, test["input"], input);
   // expected output
   DynamicObject expect;

   DynamicObject output;
   DynamicObject& type = test["@type"];
   if(type->indexOf("jld:NormalizeTest") != -1)
   {
      // read expected output
      _readFile(root, test["expect"], expect);
      // normalize
      assertNoException(
         JsonLd::normalize(input, output));
   }
   else if(type->indexOf("jld:ExpandTest") != -1)
   {
      // read expected output
      _readFile(root, test["expect"], expect);
      // expand
      assertNoException(
         JsonLd::expand(input, output));
   }
   else if(type->indexOf("jld:CompactTest") != -1)
   {
      // sanity check
      v::ValidatorRef tv = new v::Map(
         "context", new v::Type(String),
         NULL);
      assertNoException(
         tv->isValid(test));

      // read expected output
      _readFile(root, test["expect"], expect);

      // read context
      DynamicObject context;
      _readFile(root, test["context"], context);

      // compact
      assertNoException(
         JsonLd::compact(context["@context"], input, output));
   }
   else if(type->indexOf("jld:FrameTest") != -1)
   {
      // sanity check
      v::ValidatorRef tv = new v::Map(
         "frame", new v::Type(String),
         NULL);
      assertNoException(
         tv->isValid(test));

      // read expected output
      _readFile(root, test["expect"], expect);

      // read frame
      DynamicObject frame;
      _readFile(root, test["frame"], frame);

      // reframe
      assertNoException(
         JsonLd::frame(input, frame, output));
   }
   else
   {
      skipped = true;
   }

   if(!skipped && !expect.isNull())
   {
      assertNamedDynoCmp("expect", expect, "output", output);
   }

   tr.pass();
   if(skipped)
   {
      string warn = StringTools::format(
         "Skipped tests of type \"%s\".",
         JsonWriter::writeToString(type, true, false).c_str());
      tr.warning(warn.c_str());
   }
}

static void _runJsonLdTestSuiteManifest(TestRunner& tr, const char* path)
{
   string dirname;
   string basename;
   File::split(path, dirname, basename);

   // read manifest file
   DynamicObject manifest;
   _readFile(dirname.c_str(), basename.c_str(), manifest);

   // types
   DynamicObject manifestType;
   manifestType = "jld:Manifest";
   DynamicObject compactType;
   compactType = "jld:CompactTest";
   DynamicObject expandType;
   expandType = "jld:ExpandTest";
   DynamicObject frameType;
   frameType = "jld:FrameTest";
   DynamicObject normalizeType;
   normalizeType = "jld:NormalizeTest";
   DynamicObject rdfType;
   rdfType = "jld:RDFTest";

   // sanity check
   v::ValidatorRef tv = new v::Map(
      "@type", new v::Contains(manifestType),
      "name", new v::Type(String),
      "sequence", new v::Each(new v::Any(
         new v::Type(String),
         new v::All(
            new v::Map(
               "name", new v::Type(String),
               "input", new v::Type(String),
               NULL),
            new v::Any(
               new v::Map(
                  "@type", new v::Contains(compactType),
                  "context", new v::Type(String),
                  "expect", new v::Type(String),
                  NULL),
               new v::Map(
                  "@type", new v::Contains(expandType),
                  "expect", new v::Type(String),
                  NULL),
               new v::Map(
                  "@type", new v::Contains(frameType),
                  "frame", new v::Type(String),
                  "expect", new v::Type(String),
                  NULL),
               new v::Map(
                  "@type", new v::Contains(normalizeType),
                  "expect", new v::Type(String),
                  NULL),
               new v::Map(
                  "@type", new v::Contains(rdfType),
                  "purpose", new v::Type(String),
                  "sparql", new v::Type(String),
                  NULL),
               NULL),
            NULL),
         NULL)),
      NULL);
   if(!tv->isValid(manifest))
   {
      string warn = StringTools::format("Invalid manifest \"%s\".", path);
      tr.warning(warn.c_str());
      dumpException();
      Exception::clear();
   }
   else
   {
      tr.group(manifest["name"]);

      // process each test
      int count = 0;
      DynamicObjectIterator i = manifest["sequence"].getIterator();
      while(i->hasNext())
      {
         tr.group(StringTools::format("%04d", ++count).c_str());
         DynamicObject& next = i->next();
         DynamicObjectType type = next->getType();
         if(type == Map)
         {
            _runJsonLdTestSuiteTest(tr, dirname.c_str(), next);
         }
         else if(type == String)
         {
            string full = File::join(dirname.c_str(), next->getString());
            _runJsonLdTestSuiteFromPath(tr, full.c_str());
         }
         tr.ungroup();
      }

      tr.ungroup();
   }
}

static void _runJsonLdTestSuiteFromPath(TestRunner& tr, const char* path)
{
   tr.group(StringTools::format("(%s)", File::basename(path).c_str()).c_str());

   File f(path);
   if(!f->exists())
   {
      tr.test("test file exists");
      tr.fail(StringTools::format("%s not found", path).c_str());
   }
   else
   {
      if(f->isFile())
      {
         _runJsonLdTestSuiteManifest(tr, path);
      }
      else if(f->isDirectory())
      {
         FileList list;
         f->listFiles(list);
         IteratorRef<File> i = list->getIterator();
         while(i->hasNext())
         {
            File& next = i->next();
            if(next->isFile())
            {
               const char* ext = next->getExtension();
               // FIXME: hack, manifests are now JSON-LD files
               if(strstr(next->getBaseName(), "manifest") != NULL &&
                  strcmp(ext, ".jsonld") == 0)
               {
                  string full = File::join(path, next->getBaseName());
                  _runJsonLdTestSuiteManifest(tr, full.c_str());
               }
            }
         }
      }
      else
      {
         tr.test("test file type");
         tr.fail(StringTools::format("%s is invalid", path).c_str());
      }
   }

   tr.ungroup();
}

static void runJsonLdTestSuite(TestRunner& tr)
{
   _runJsonLdTestSuiteFromPath(tr, JSON_LD_TEST_SUITE_DIR);
}

static bool run(TestRunner& tr)
{
   if(tr.isDefaultEnabled() || tr.isTestEnabled("json-ld"))
   {
#ifdef HAVE_JSON_LD_TEST_SUITE
      runJsonLdTestSuite(tr);
#endif
   }
   return true;
}

} // end namespace

MO_TEST_MODULE_FN("monarch.tests.jsonld.test", "1.0", mo_test_jsonld::run)
