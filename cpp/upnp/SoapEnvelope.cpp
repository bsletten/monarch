/*
 * Copyright (c) 2009 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/upnp/SoapEnvelope.h"

#include "monarch/data/json/JsonWriter.h"
#include "monarch/data/xml/DomReader.h"
#include "monarch/data/xml/DomWriter.h"
#include "monarch/io/ByteArrayInputStream.h"
#include "monarch/io/ByteArrayOutputStream.h"
#include "monarch/logging/Logging.h"

using namespace std;
using namespace monarch::data::json;
using namespace monarch::data::xml;
using namespace monarch::io;
using namespace monarch::logging;
using namespace monarch::rt;
using namespace monarch::upnp;

#define SOAP_NS_URI        "http://schemas.xmlsoap.org/soap/envelope"
#define SOAP_NS_PREFIX     "soap"
#define ENC_STYLE_NS_URI   "http://schemas.xmlsoap.org/soap/encoding"
#define TARGET_NS_PREFIX   "m"

SoapEnvelope::SoapEnvelope()
{
}
SoapEnvelope::~SoapEnvelope()
{
}

// a helper function that converts message parameters into a dom element
static void paramsToElement(SoapMessage& msg, DynamicObject& params, Element& e)
{
   if(params->getType() == Map)
   {
      DynamicObjectIterator pi = params.getIterator();
      while(pi->hasNext())
      {
         DynamicObject& p = pi->next();

         // add param element
         Element param;
         param["name"] = pi->getName();
         param["namespace"] = msg["namespace"]->getString();
         e["children"][pi->getName()]->append(param);
         paramsToElement(msg, p, param);
      }
   }
   else
   {
      // use 0/1 for booleans
      e["data"] = (params->getType() == Boolean) ?
         (params->getBoolean() ? "1" : "0") : params->getString();
   }
}

string SoapEnvelope::create(SoapMessage& msg)
{
   string envelope;

   // create root element
   Element root;
   root["name"] = "Envelope";
   root["namespace"] = SOAP_NS_URI;
   // add soap namespace attribute
   {
      Attribute attr;
      attr["name"] = "xmlns:" SOAP_NS_PREFIX;
      attr["value"] = SOAP_NS_URI;
      root["attributes"][attr["name"]->getString()] = attr;
   }
   // add encoding style attribute
   {
      Attribute attr;
      attr["name"] = "encodingStyle";
      attr["namespace"] = SOAP_NS_URI;
      attr["value"] = ENC_STYLE_NS_URI;
      root["attributes"][attr["name"]->getString()] = attr;
   }

   // add body element
   Element body;
   body["name"] = "Body";
   body["namespace"] = SOAP_NS_URI;
   root["children"]["Body"]->append(body);
   // add target namespace attribute
   {
      Attribute attr;
      attr["name"] = "xmlns:" TARGET_NS_PREFIX;
      attr["value"] = msg["namespace"]->getString();
      body["attributes"][attr["name"]->getString()] = attr;
   }

   // add message
   Element message;
   message["name"] = msg["name"]->getString();
   message["namespace"] = msg["namespace"]->getString();
   body["children"][message["name"]->getString()]->append(message);

   // add message parameters
   paramsToElement(msg, msg["params"], message);

   // write envelope to string
   DomWriter writer;
   writer.setCompact(true);
   ByteBuffer bb(1024);
   ByteArrayOutputStream baos(&bb, true);
   if(writer.write(root, &baos))
   {
      envelope.append(bb.data(), bb.length());
   }
   else
   {
      ExceptionRef e = new Exception(
         "Could not create soap envelope.",
         "monarch.upnp.InvalidSoapEnvelope");
      Exception::push(e);
   }

   return envelope;
}

// a helper function to convert elements into objects using tag names as keys
static void elementToParams(Element& e, DynamicObject& params)
{
   // convert any children
   if(e["children"]->length() > 0)
   {
      // each entry in children is an array of child names
      DynamicObjectIterator i = e["children"].getIterator();
      while(i->hasNext())
      {
         DynamicObject& childName = i->next();

         // each entry in child name is a child of that name
         ElementIterator ci = childName.getIterator();
         while(ci->hasNext())
         {
            Element& child = ci->next();
            elementToParams(child, params[i->getName()]);
         }
      }
   }
   else
   {
      params = e["data"]->getString();
   }
}

bool SoapEnvelope::parse(InputStream* is, SoapResult& result)
{
   bool rval = false;

   // prepare passed result
   result->setType(Map);
   result->clear();
   result["fault"] = false;
   SoapMessage& msg = result["message"];
   msg->setType(Map);

   //  parse result
   Element root;
   DomReader reader;
   reader.start(root);
   if((rval = reader.read(is) && reader.finish()))
   {
      MO_CAT_DEBUG(MO_UPNP_CAT, "Parsing SOAP envelope: %s",
         JsonWriter::writeToString(root).c_str());

      // ensure there is a body in the response
      rval = false;
      if(root["children"]->hasMember("Body"))
      {
         // ensure body is valid
         Element& body = root["children"]["Body"][0];
         if(body["children"]->length() == 1)
         {
            // body contains message and is valid
            rval = true;

            // get message name and namespace
            Element message = body["children"].first()[0];
            const char* name = message["name"]->getString();
            const char* ns = message["namespace"]->getString();
            msg["name"] = name;
            msg["namespace"] = ns;
            msg["params"]->setType(Map);

            // is a soap fault if name is "Fault" and namespace is
            // the same as the body element
            if(strcmp(name, "Fault") == 0 &&
               strcmp(ns, body["namespace"]->getString()) == 0)
            {
               result["fault"] = true;
            }

            // convert element to message params
            elementToParams(message, msg["params"]);
         }
      }

      if(!rval)
      {
         ExceptionRef e = new Exception(
            "Invalid or no soap body found in response envelope.",
            "monarch.upnp.InvalidSoapEnvelope");
         Exception::set(e);
      }
      else
      {
         MO_CAT_DEBUG(MO_UPNP_CAT, "Parsed SOAP result: %s",
            JsonWriter::writeToString(result).c_str());
      }
   }

   return rval;
}
