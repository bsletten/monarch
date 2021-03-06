/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_crypto_DefaultBlockCipher_H
#define monarch_crypto_DefaultBlockCipher_H

#include "monarch/crypto/AbstractBlockCipher.h"
#include "monarch/crypto/SymmetricKey.h"

namespace monarch
{
namespace crypto
{

/**
 * A DefaultBlockCipher uses a block cipher algorithm to encrypt or
 * decrypt data.
 *
 * This class uses OpenSSL's block cipher implementation.
 *
 * @author Dave Longley
 */
class DefaultBlockCipher : public AbstractBlockCipher
{
public:
   /**
    * Creates a new DefaultBlockCipher.
    */
   DefaultBlockCipher();

   /**
    * Destructs this DefaultBlockCipher.
    */
   virtual ~DefaultBlockCipher();

   /**
    * Starts encrypting with a randomly generated SymmetricKey that is
    * stored in the passed parameter.
    *
    * @param algorithm the algorithm to use to encrypt.
    * @param symmetricKey to store the generated SymmetricKey.
    *
    * @return true if no exception occurred, false if not.
    */
   virtual bool startEncrypting(
      const char* algorithm, SymmetricKey* symmetricKey);

   /**
    * Starts encrypting with the given SymmetricKey.
    *
    * @param symmetricKey to start encrypting with.
    *
    * @return true if no exception occurred, false if not.
    */
   virtual bool startEncrypting(SymmetricKey* symmetricKey);

   /**
    * Starts decrypting with the given SymmetricKey.
    *
    * @param symmetricKey to start decrypting with.
    *
    * @return true if no exception occurred, false if not.
    */
   virtual bool startDecrypting(SymmetricKey* symmetricKey);

   /**
    * Updates the data that is being encrypted or decrypted. This method can be
    * called repeatedly with chunks of the data that is to be encrypted or
    * decrypted.
    *
    * The out buffer must be at least inLength + getBlockSize().
    *
    * @param in a buffer with data to encrypt/decrypt.
    * @param inLength the length of the data.
    * @param out a buffer to fill with encrypt/decrypt data.
    * @param outLength to store the number of bytes put into the output buffer.
    *
    * @return true if no exception occurred, false if not.
    */
   virtual bool update(const char* in, int inLength, char* out, int& outLength);

   /**
    * Puts the final chunk of encrypted or decrypted data into an array of
    * bytes.
    *
    * The out buffer must be at least getBlockSize() bytes long.
    *
    * @param out a buffer to fill with the data.
    * @param length to store the number of bytes put into the output buffer.
    *
    * @return true if no exception occurred, false if not.
    */
   virtual bool finish(char* out, int& length);

   // use update/finish from AbstractBlockCipher
   using AbstractBlockCipher::update;
   using AbstractBlockCipher::finish;

   /**
    * Gets the number of input bytes. If encrypting, this is the total size
    * of the unencrypted data. If decrypting, this is the total size of the
    * encrypted data. This number is reset when a new encryption or decryption
    * is started.
    *
    * @return the total number of input bytes.
    */
   virtual uint64_t getTotalInput();

   /**
    * Gets the number of output bytes. If encrypting, this is the total size
    * of the encrypted data. If decrypting, this is the total size of the
    * decrypted data. This number is reset when a new encryption or decryption
    * is started.
    *
    * @return the total number of output bytes.
    */
   virtual uint64_t getTotalOutput();
};

} // end namespace crypto
} // end namespace monarch
#endif
