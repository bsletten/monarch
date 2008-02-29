/*
 * Copyright (c) 2008 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_validation_Optional_H
#define db_validation_Optional_H

#include "db/validation/Validator.h"

namespace db
{
namespace validation
{

/**
 * A validator container that can be used to check optional Map keys.  A Map's
 * key validator can be wrapped in an Optional validator if it does not need to
 * be present for successful validation.  If it is present then the
 * sub-validator is checked.
 *
 * d1 => { "a": 0 }
 * d2 => { "a": 0, "b": "b!" }
 *
 *   Map m(
 *      "a", new Type(Int32),
 *      "b", new Optional(new Type(String)),
 *      NULL)
 *   m.isValid(d1) => true
 *   m.isValid(d2) => true
 * 
 * @author David I. Lehn
 */
class Optional : Validator
{
protected:
   Validator* mValidator;

public:
   /**
    * Creates a new validator.
    */
   Optional(Validator* validator);
   
   /**
    * Destructs this validator.
    */
   virtual ~Optional();
   
   /**
    * Checks if an object is valid.
    * 
    * @param obj the object to validate.
    * @param state arbitrary state for validators to use during validation.
    * @param path the validation path used to get to this validator.
    * 
    * @return true if obj is valid, false and exception set otherwise.
    */
   virtual bool isValid(
      db::rt::DynamicObject& obj,
      db::rt::DynamicObject* state = NULL,
      std::vector<const char*>* path = NULL);

   /**
    * If an object is missing is this Check if this validator is optional.
    * 
    * @param state arbitrary state for validators to use during validation.
    * 
    * @return true if optional, false if mandatory.
    */
   virtual bool isOptional(db::rt::DynamicObject* state = NULL);
};

} // end namespace validation
} // end namespace db
#endif
