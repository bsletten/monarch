/*
 * Copyright (c) 2007-2010 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_util_Pattern_H
#define monarch_util_Pattern_H

#include <regex.h>
#include "monarch/rt/DynamicObject.h"

namespace monarch
{
namespace util
{

/**
 * A Pattern is a utility class for storing compiled regular expressions.
 *
 * @author Dave Longley
 */
class Pattern
{
protected:
   /**
    * The storage struct for the pattern.
    */
   regex_t mStorage;

   /**
    * Creates a new Pattern.
    */
   Pattern();

   /**
    * Gets the storage for this Pattern.
    *
    * @return the storage for this Pattern.
    */
   regex_t& getStorage();

public:
   /**
    * Destructs this Pattern.
    */
   virtual ~Pattern();

   /**
    * Matches this pattern against the passed string at the given offset and
    * sets the offsets for the next match, if one can be found. The pattern
    * must have been compiled with submatch support enabled.
    *
    * @param str the string to match this pattern against.
    * @param offset the offset in the string to start matching at.
    * @param start the starting offset for the found match, if one was found.
    * @param end the ending offset for the found match, if one was found.
    *
    * @return true if a match was found, false if not.
    */
   virtual bool match(const char* str, int offset, int& start, int& end);

   /**
    * Matches this pattern against the given string.
    *
    * @param str the string to match this pattern against.
    *
    * @return true if the passed string matches the regex, false if not.
    */
   virtual bool match(const char* str);

   /**
    * Matches this pattern against the given string and returns the portions
    * of the string that matched subexpressions in the pattern. The pattern
    * must have been compiled with submatch support enabled.
    *
    * If the given number of subexpressions is higher than the number in this
    * pattern then fewer than 'n' results will be in the array. To access
    * the full match use matches[0]. To access any submatch, use the index
    * of the submatch starting at 1. Thus, to get the first submatch, use
    * matches[1].
    *
    * @param str the string to match this pattern against.
    * @param matches the array to store the string matches in, with the first
    *           always containing the full match.
    * @param n the number of subexpressions to get strings for, -1 for all.
    *
    * @return true if the passed string matches the regex, false if not.
    */
   virtual bool getSubMatches(
      const char* str, monarch::rt::DynamicObject& matches, int n = -1);

   /**
    * Compiles a regular expression into a Pattern.
    *
    * @param regex the regular expression to compile.
    * @param matchCase true to match case, false to be case-insensitive.
    * @param subMatches true to allow sub-matches, false not to.
    *
    * @return the compiled Pattern or NULL if an exception occurred.
    */
   static monarch::rt::Collectable<Pattern> compile(
      const char* regex, bool matchCase = true, bool subMatches = true);

   /**
    * Matches the passed regex against the given string.
    *
    * @param regex the regex to use.
    * @param str the string to match this pattern against.
    * @param matchCase true to match case, false to be case-insensitive.
    *
    * @return true if the passed string matches the regex, false if not or
    *         if an exception occurred.
    */
   static bool match(const char* regex, const char* str, bool matchCase = true);
};

// typedef for a reference counted Pattern class
typedef monarch::rt::Collectable<Pattern> PatternRef;

} // end namespace util
} // end namespace monarch
#endif