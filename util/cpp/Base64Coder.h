#ifndef Base64Coder_H
#define Base64Coder_H

#include <string>

namespace db
{
namespace util
{

/**
 * Encodes and decodes data into/from Base64.
 * 
 * Base64 encoding consists of taking 3 bytes (24 bits) and converting
 * it into 4 6-bit index values. Each index value corresponds to an
 * alphanumeric character. Every 76 characters, an CRLF must be
 * inserted. Decoders should ignore whitespace.
 * 
 * So these bytes:
 * 
 * b0: 00000001
 * b1: 00000010
 * b2: 00000011
 * 
 * Would be concatenated to:
 * 
 * 000000010000001000000011
 * 
 * And then split up like this:
 * 
 * 000000|010000|001000|000011
 * 
 * An algorithm for doing this would be the following,
 * where 0x3F = 63 (6-bits):
 * 
 * Index 1 = b0 >> 2
 * Index 2 = (b0 << 4 | b1 >> 4) & 0x3F 
 * Index 3 = (b1 << 2 | b2 >> 6) & 0x3F
 * Index 4 = b2 & 0x3F
 *
 * Algorithm steps (and byte view):
 * 
 *     1   |   2    |   3   |    4
 * 00000000
 *   000000
 *   000000|00010000
 *   000000|  010000
 *   000000|  010000|00001000
 *   000000|  010000|  001000
 *   000000|  010000|  001000|00000011
 *   000000|  010000|  001000|  000011
 *   
 *   000000|  010000|  001000|  000011
 * 
 * And these values would be translated into index values:
 * 
 * 0, 32, 16, 3
 * 
 * Which would in turn translate into:
 * 
 * AgQD
 * 
 * If the byte array that is being translated is not a multiple of 3, then
 * the bytes are translated and the '=' is used as padding.
 * 
 * So for 2 bytes:
 * 
 * 00000001
 * 00000010
 * 
 * 0000000100000010
 * 
 * 000000|010000|001000|=
 * 
 * And for 1 byte:
 * 
 * 0000001
 * 
 * 000000|001000|=|=
 * 
 * @author Dave Longley
 */
class Base64Coder
{
protected:
   /**
    * Base64 encoding maps 6-bit (0-63) indices to alphanumeric characters.
    * This is the Base64 map.
    */
   static const char INDEX_TO_BASE64[];
   
   /**
    * This is the reverse map that maps alphanumeric characters to
    * 6-bit (0-63) indices.
    * 
    * The character '+' has the lowest integer value of all of the
    * Base64 characters. Its value is 43, so we subtract 43 from
    * all of the character int values to get the appropriate index.
    * 
    * -1 is an invalid entry, -2 is the padding entry for '='.
    */
   static const int BASE64_TO_INDEX[];

public:
   /**
    * Replaces all instances in "str" of "find" with "replace".
    * 
    * @param str the string to operate on.
    * @param find the string to find.
    * @param replace the string to replace "find" with.
    */
   static void replaceAll(
      std::string& str, std::string find, std::string replace);
   
   /**
    * Converts a character into an integer (according to the
    * decoder map).
    * 
    * @param c the character to convert.
    * 
    * @return the integer.
    */
   static int charToInt(char c);
   
   /**
    * Encodes 3 bytes into 4 characters.
    * 
    * @param data the byte array with the 3 bytes.
    * @param offset the offset where the 3 bytes start.
    * @param length the length of the byte array.
    * @param group the group to populate with 4 characters.
    */
   static void encodeGroup(char* data, int offset, int length, char* group);
   
   /**
    * Decodes ONE group of Base64 characters into bytes.
    * 
    * @param str the string with the characters to decode into bytes.
    * @param offset the offset to begin decoding at.
    * @param bytes the decoded byte array to populate (of size 3).
    * 
    * @return the number of decoded bytes (1 to 3).
    */
   static int decodeGroup(std::string str, int offset, char* bytes);
   
   /**
    * Base64 encodes data.
    * 
    * @param data the byte array to encode.
    * 
    * @return the Base64 encoded string.
    */
   static std::string encode(char* data);
   
   /**
    * Decodes a Base64-encoded string. Calls AbstractPreferences
    * getByteArray() method which Base64 decodes a value from the internal
    * Coder map.
    *  
    * @param str the Base64-encoded string.
    * 
    * @return the decoded byte array.
    */
   static char* decode(std::string str);
};

} // end namespace util
} // end namespace db
#endif
