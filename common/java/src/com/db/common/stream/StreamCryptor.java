/*
 * Copyright (c) 2003-2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.common.stream;

import com.db.common.Cryptor;
import com.db.common.logging.Logger;
import com.db.common.logging.LoggerManager;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;

/**
 * A class for encrypting and decrypting streaming data.
 * 
 * This class uses some specified encryption scheme to encrypt
 * streaming data. Once that data has been encrypted, the size of the
 * last encrypted chunk and the data/file size are appended to
 * the stream. The size of the last encrypted chunk takes up 4 bytes,
 * and the total data/file size takes up 8 bytes. So at the end of the
 * encrypted stream, a total of 12 bytes of information required for
 * decryption will be appended.
 * 
 * The last 8 bytes is the total number of encrypted bytes, and therefore
 * the true size that the data should be after decryption. The 4 bytes
 * preceeding those 8 bytes are the size of the very last encrypted chunk,
 * which, in all likelihood, will be smaller than the specified decrypt
 * chunk size. 
 * 
 * @author Dave Longley
 */
public abstract class StreamCryptor implements IStreamManager,
                                               IStreamProcessor
{
   /**
    * The underlying Cryptor that does encryption/decryption.
    */
   public Cryptor mCryptor;
   
   /**
    * Encrypt mode option.
    */
   public static final int ENCRYPT = 0;

   /**
    * Decrypt mode option.
    */
   public static final int DECRYPT = 1;
   
   /**
    * The current crypt mode.
    */
   protected int mCryptMode;
   
   /**
    * The encrypt chunk size. In encrypt mode, whenever the number of
    * bytes read from this stream hits this number of bytes, the bytes
    * will be encrypted and the process will start over.
    */
   protected int mEncryptChunkSize;

   /**
    * The decrypt chunk size. In decrypt mode, whenever the number of
    * bytes read from this stream hits this number of bytes, the bytes
    * will be decrypted and the process will start over.
    */
   protected int mDecryptChunkSize;
   
   /**
    * The padding size. This is the number of bytes that are just
    * "padding" in an chunk that is to be decrypted.
    */
   protected int mPadding;   
   
   /**
    * Stores the total number of bytes that have been encrypted. This is
    * the true stream data size and is needed for data decryption. 
    */
   protected long mNumBytesEncrypted;
   
   /**
    * Stores the total number of bytes that have been decrypted.
    */
   protected long mNumDecryptedBytes;
   
   /**
    * The decrypt chunk size for the last chunk in the stream.
    */
   protected int mLastDecryptChunkSize;
   
   /**
    * Constructs a stream cryptor. The decrypt and encrypt chunk sizes must
    * be specified. The decrypt chunk size is the size of a chunk in
    * decrypt mode. The encrypt chunk size is the size of a chunk in
    * encrypt mode. Each size corresponds to the size that the internal
    * chunk buffer must reach before it is encrypted/decrypted.
    * 
    * @param encryptChunkSize the size of a chunk to encrypt.
    * @param decryptChunkSize the size of a chunk to decrypt.
    * @param padding the padding size.
    */
   public StreamCryptor(int encryptChunkSize, int decryptChunkSize,
                        int padding)
   {
      this(new Cryptor(), encryptChunkSize, decryptChunkSize, padding);
   }
   
   /**
    * Constructs a stream cryptor. The cryptor to use to do the
    * encryptiong and decryption and the decrypt and encrypt chunk sizes must
    * be specified. The decrypt chunk size is the size of a chunk in
    * decrypt mode. The encrypt chunk size is the size of a chunk in
    * encrypt mode. Each size corresponds to the size that the internal
    * chunk buffer must reach before it is encrypted/decrypted.
    * 
    * @param cryptor the cryptor to use to do the encryption and decryption.
    * @param encryptChunkSize the size of a chunk to encrypt.
    * @param decryptChunkSize the size of a chunk to decrypt.
    * @param padding the padding size.
    */
   public StreamCryptor(Cryptor cryptor,
                        int encryptChunkSize, int decryptChunkSize,
                        int padding)
   {
      mCryptor = cryptor;
      
      // set chunk sizes, create chunk buffer, create chunk stream
      mEncryptChunkSize = encryptChunkSize;
      mDecryptChunkSize = decryptChunkSize;
      mPadding = padding;

      mNumBytesEncrypted = 0;
      mNumDecryptedBytes = 0;
      mLastDecryptChunkSize = 0;
      
      // default mode is encryption
      setStreamCryptMode(StreamCryptor.ENCRYPT);
   }
   
   /**
    * Converts a long to a byte array.
    * 
    * @param l the long to convert.
    * @return the byte array for the long.
    */
   protected byte[] convertToBytes(long l)
   {
      byte[] bytes = new byte[8];
      
      try
      {
         ByteArrayOutputStream baos = new ByteArrayOutputStream();
         DataOutputStream dos = new DataOutputStream(baos);
         dos.writeLong(l);
         bytes = baos.toByteArray();
         
         dos.close();
         baos.close();
      }
      catch(Throwable t)
      {
         getLogger().error("could not convert long to bytes!");
         getLogger().debug(Logger.getStackTrace(t));
      }
      
      return bytes;
   }
   
   /**
    * Encrypts a chunk of data.
    * 
    * @param data the data to encrypt.
    * @param last whether or not this is the last chunk of data in the stream.
    * @return the encrypted chunk of data.
    */
   protected byte[] encryptChunk(byte[] data, boolean last)
   {
      byte[] processed = null;
      
      // update the total bytes encrypted
      mNumBytesEncrypted += data.length;
      processed = getCryptor().encrypt(data);

      // check for last chunk
      if(data.length < getEncryptChunkSize() || last)
      {
         // append last decrypt chunk size and file size
         byte[] buffer = new byte[processed.length + 12];
         System.arraycopy(processed, 0, buffer, 0, processed.length);
         
         // processed length is the last decrypt chunk size
         byte[] bytes = convertToBytes(processed.length); 
         System.arraycopy(bytes, 0, buffer, processed.length, 4);
         
         // number of bytes encrypted is decrypted file size
         bytes = convertToBytes(getNumBytesEncrypted());
         System.arraycopy(bytes, 0, buffer, processed.length + 4, 8);
         
         processed = buffer;
      }
      
      return processed;
   }
   
   /**
    * Decrypts a chunk of data.
    * 
    * @param data the data to decrypt.
    * @param last whether or not this is the last chunk of data in the stream.
    * @return the decrypted chunk of data.
    */
   protected byte[] decryptChunk(byte[] data, boolean last)
   {
      byte[] processed = null;
      
      // see how many bytes left to decrypt
      long left = getNumBytesEncrypted() - getNumDecryptedBytes();

      if(left != 0)
      {
         // if fewer than decrypt chunk size bytes remaining,
         // then use last decrypt chunk size
         if(left < Integer.MAX_VALUE && left < getDecryptChunkSize())
         {
            if(data.length >= getLastDecryptChunkSize())
            {
               processed = getCryptor().decrypt(
                     data, 0, getLastDecryptChunkSize());
            }
            else
            {
               getLogger().error("not enough data to decrypt last chunk!");
            }
         }
         else
         {
            // decrypt data
            processed = getCryptor().decrypt(data);
         }
         
         // update total decrypted bytes
         if(processed != null)
         {
            mNumDecryptedBytes += processed.length;
         }
         else
         {
            getLogger().error("decrypted data is null!");
         }
      }
      
      return processed;
   }
   
   /**
    * Resets this stream cryptor.
    */
   public void reset()
   {
      // reset values
      mNumBytesEncrypted = 0;
      mNumDecryptedBytes = 0;
      mLastDecryptChunkSize = 0;
   }
   
   /**
    * Converts a byte array to an int.
    * 
    * @param bytes the byte array to convert.
    * @return the int for the byte array.
    */
   public static int convertToInt(byte[] bytes)
   {
      return convertToInt(bytes, 0);
   }
   
   /**
    * Converts a byte array to an int.
    * 
    * @param bytes the byte array to convert.
    * @param offset the offset to start converting at.
    * @return the int for the byte array.
    */
   public static int convertToInt(byte[] bytes, int offset)
   {
      int rval = 0;
      
      try
      {
         ByteArrayInputStream bais =
            new ByteArrayInputStream(bytes, offset, 4);
         
         DataInputStream dis = new DataInputStream(bais);
         rval = dis.readInt();
         
         dis.close();
         bais.close();
      }
      catch(Throwable t)
      {
         getLogger().error("could not convert bytes to int!");
         getLogger().debug(Logger.getStackTrace(t));
      }
      
      return rval;
   }      
   
   /**
    * Converts an int to a byte array.
    * 
    * @param i the int to convert.
    * @return the byte array for the int.
    */
   public static byte[] convertToBytes(int i)
   {
      byte[] bytes = new byte[8];
      
      try
      {
         ByteArrayOutputStream baos = new ByteArrayOutputStream();
         DataOutputStream dos = new DataOutputStream(baos);
         dos.writeInt(i);
         bytes = baos.toByteArray();
         
         dos.close();
         baos.close();
      }
      catch(Throwable t)
      {
         getLogger().error("could not convert int to bytes!");
         getLogger().debug(Logger.getStackTrace(t));
      }
      
      return bytes;
   }
   
   /**
    * Converts a byte array to a long.
    * 
    * @param bytes the byte array to convert.
    * @return the long for the byte array.
    */
   public static long convertToLong(byte[] bytes)
   {
      return convertToLong(bytes, 0);
   }
   
   /**
    * Converts a byte array to a long.
    * 
    * @param bytes the byte array to convert.
    * @param offset the offset to start at.
    * @return the long for the byte array.
    */
   public static long convertToLong(byte[] bytes, int offset)
   {
      long rval = 0;
      
      try
      {
         ByteArrayInputStream bais =
            new ByteArrayInputStream(bytes, offset, 8);
         
         DataInputStream dis = new DataInputStream(bais);
         rval = dis.readLong();
         
         dis.close();
         bais.close();
      }
      catch(Throwable t)
      {
         getLogger().error("could not convert bytes to long!");
         getLogger().debug(Logger.getStackTrace(t));
      }
      
      return rval;
   }   
   
   /**
    * Gets the last decrypt chunk size for an encrypted file. 
    * 
    * @param src the encrypted file.
    * @return the last decrypt chunk size.
    */
   public static int getLastDecryptChunkSize(File src)
   {
      int rval = 0;
      
      try
      {
         // seek to end of file and backup 8 bytes to get file size
         RandomAccessFile raf = new RandomAccessFile(src, "r");
         raf.seek(raf.length() - 12);
         byte[] bytes = new byte[4];
         raf.readFully(bytes);
         raf.close();
         
         rval = convertToInt(bytes);
      }
      catch(Throwable t)
      {
         getLogger().debug(Logger.getStackTrace(t));
      }
      
      return rval;
   }
   
   /**
    * Gets the decrypted file size for an encrypted file. 
    * 
    * @param src the encrypted file.
    * @return the decrypted file size.
    */
   public static long getDecryptedFileSize(File src)
   {
      long rval = 0;
      
      try
      {
         // seek to end of file and backup 8 bytes to get file size
         RandomAccessFile raf = new RandomAccessFile(src, "r");
         raf.seek(raf.length() - 8);
         byte[] bytes = new byte[8];
         raf.readFully(bytes);
         raf.close();
         
         rval = convertToLong(bytes);
      }
      catch(Throwable t)
      {
         getLogger().debug(Logger.getStackTrace(t));
      }
      
      return rval;
   }
   
   /**
    * Encrypts the entire passed stream of data and writes it to the
    * passed output stream. 
    * 
    * @param mis the managed input stream to read the data from.
    * @param outputStream the stream to write the encrypted data to.
    * @return true if successfully encrypted, false if not.
    */
   public boolean encrypt(ManagedInputStream mis, OutputStream outputStream)
   {
      boolean rval = false;

      setStreamCryptMode(ENCRYPT);
      
      // clear the total number of bytes encrypted
      clearNumBytesEncrypted();
      
      try
      {
         // get a packet buffer
         byte[] packet = new byte[mEncryptChunkSize];
         
         int numBytes = -1;
         
         // encrypt the data and write to output stream
         while((numBytes = mis.read(packet)) != -1)
         {
            outputStream.write(packet, 0, numBytes);
         }
         
         rval = true;
      }
      catch(Exception e)
      {
         getLogger().debug(Logger.getStackTrace(e));
      }      
      
      return rval;      
   }
   
   /**
    * Opens the passed source file, encrypts it, and writes it to the
    * destination file.
    * 
    * @param src the file to read the data from.
    * @param dest the file to write the encrypted data to.
    * @return true if successfully encrypted, false if not.
    */
   public boolean encrypt(File src, File dest)
   {
      boolean rval = false;
      
      try
      {
         // open the file streams
         FileInputStream fis = new FileInputStream(src);
         FileOutputStream fos = new FileOutputStream(dest);
         
         // create a managed input stream
         ManagedInputStream mis = new ManagedInputStream(fis, this, this);
         
         // encrypt the data and write to file
         rval = encrypt(mis, fos);
         
         // close the file streams
         fis.close();
         fos.close();
         
         rval = true;
      }
      catch(Exception e)
      {
         getLogger().debug(Logger.getStackTrace(e));
      }
      
      return rval;
   }
   
   /**
    * Opens the passed source file, encrypts it, and writes it to the
    * destination file.
    * 
    * @param src the file to read the data from.
    * @param dest the file to write the encrypted data to.
    * @return true if successfully encrypted, false if not.
    */
   public boolean encrypt(String src, String dest)
   {
      boolean rval = false;
      
      try
      {
         // open the file streams
         FileInputStream fis = new FileInputStream(src);
         FileOutputStream fos = new FileOutputStream(dest);
         
         // create a managed input stream
         ManagedInputStream mis = new ManagedInputStream(fis, this, this);
         
         // encrypt the data and write to file
         rval = encrypt(mis, fos);
         
         // close the file streams
         fis.close();
         fos.close();
         
         rval = true;
      }
      catch(Exception e)
      {
         getLogger().debug(Logger.getStackTrace(e));
      }
      
      return rval;
   }
   
   /**
    * Prepares the decrypt stream by setting the number of bytes to be
    * decrypted and the size of the last chunk to be decrypted (which
    * may be different from the other chunks).
    * 
    * @param src the source file that contains the data to be decrypted.
    */
   public void prepareDecryptStream(File src)
   {
      try
      {
         // seek to end of file and backup 12 bytes to get last
         // decrypt chunk size and decrypted file size
         RandomAccessFile raf = new RandomAccessFile(src, "r");
         raf.seek(raf.length() - 12);
         byte[] bytes = new byte[12];
         raf.readFully(bytes);
         raf.close();
         
         setLastDecryptChunkSize(convertToInt(bytes));
         setNumBytesEncrypted(convertToLong(bytes, 4));
      }
      catch(Throwable t)
      {
         getLogger().debug(Logger.getStackTrace(t));
      }
   }

   /**
    * Decrypts the entire passed stream of data. Before calling this method,
    * make sure to set the number of bytes to be decrypted and the size
    * of the last chunk to be decrypted. One way to set these values is
    * to call <code>prepareDecryptStream</code>. This method will throw
    * an exception if the number of encrypted bytes is zero or the
    * size of the last chunk to be decrypted is zero.
    * 
    * The decrypted data will be written to the passed output stream.
    * 
    * @param mis the managed input stream to read the data from.
    * @param outputStream the stream to write the decrypted data to.
    * @return true if successfully decrypted, false if not.
    * @throws Exception
    */ 
   public boolean decrypt(ManagedInputStream mis, OutputStream outputStream)
   throws Exception
   {
      boolean rval = false;
      
      if(getNumBytesEncrypted() != 0 && getLastDecryptChunkSize() != 0)
      {
         setStreamCryptMode(DECRYPT);
         
         try
         {
            // get a packet buffer
            byte[] packet = new byte[mDecryptChunkSize];

            int numBytes = -1;
            
            // decrypt the data and write to output stream
            while((numBytes = mis.read(packet)) != -1)
            {
               outputStream.write(packet, 0, numBytes);
            }
            
            rval = true;
         }
         catch(Exception e)
         {
            getLogger().debug(Logger.getStackTrace(e));
         }
      }
      else
      {
         Exception e = new Exception(
               "Number of bytes to be encrypted is zero or last decrypt " +
               "chunk size is zero. These values must be set to a value " +
               "other than zero before decryption. Try calling " +
               "\"prepareDecryptStream()\"");
         throw e;
      }
      
      return rval;
   }
   
   /**
    * Opens the passed source file, decrypts it, and writes it to the
    * destination file. The size of the decrypted data
    * in bytes must be passed to resize the file to remove extra
    * padding.
    * 
    * @param src the file to read the data from.
    * @param dest the file to write the decrypted data to.
    * @return true if successfully decrypted, false if not.
    */
   public boolean decrypt(File src, File dest)
   {
      boolean rval = false;
      
      try
      {
         // open the file streams
         FileInputStream fis = new FileInputStream(src);
         FileOutputStream fos = new FileOutputStream(dest);
         
         // create a managed input stream
         ManagedInputStream mis = new ManagedInputStream(fis, this, this);
         
         // prepare the decrypt stream
         prepareDecryptStream(src); 

         // decrypt the data and write to file
         rval = decrypt(mis, fos);
         
         // close the file stream
         fis.close();
         fos.close();
         
         rval = true;
      }
      catch(Exception e)
      {
         getLogger().debug(Logger.getStackTrace(e));
      }
      
      return rval;
   }
   
   /**
    * Opens the passed source file, decrypts it, and writes it to the
    * destination file. The size of the decrypted data
    * in bytes must be passed to resize the file to remove extra
    * padding.
    * 
    * @param src the file to read the data from.
    * @param dest the file to write the decrypted data to.
    * @return true if successfully decrypted, false if not.
    */
   public boolean decrypt(String src, String dest)
   {
      boolean rval = false;
      
      try
      {
         // open the file streams
         FileInputStream fis = new FileInputStream(src);
         FileOutputStream fos = new FileOutputStream(dest);
         
         // create a managed input stream
         ManagedInputStream mis = new ManagedInputStream(fis, this, this);
         
         // prepare the decrypt stream
         prepareDecryptStream(new File(src)); 
         
         // decrypt the data and write to file
         rval = decrypt(mis, fos);
         
         // close the file stream
         fis.close();
         fos.close();
         
         rval = true;
      }
      catch(Exception e)
      {
         getLogger().debug(Logger.getStackTrace(e));
      }
      
      return rval;
   }
   
   /**
    * Checks the passed buffer to see if it is ready to be
    * processed, if it isn't it returns the number of
    * additional bytes needed. If it is ready to be
    * processed, the number of extra bytes that shouldn't
    * be yet processed is returned (zero or a negative number).
    * 
    * @param data the data that has been read so far.
    * @param offset the offset for valid data in the buffer.
    * @param length the number of valid bytes of data in the data buffer.
    * @return the positive number of additional bytes required before
    *         the data can be processed, or the negative number of
    *         extra bytes to be saved until this method requests them, or
    *         zero indicating that the data is ready to be processed.
    */
   public int manageStreamData(byte[] data, int offset, int length)   
   {
      int requiredBytes = 0;
      
      // determine how much more to read based on crypt mode
      switch(getStreamCryptMode())
      {
         case ENCRYPT:
            requiredBytes = getEncryptChunkSize() - length;
            break;
         case DECRYPT:
            // see how many bytes left to decrypt
            long left = getNumBytesEncrypted() - getNumDecryptedBytes();
            
            if(left != 0)
            {
               // check to see which of the two is smaller, the decrypt
               // chunk size or what's required to get the remaining data
               int less = getDecryptChunkSize();
               if(left < Integer.MAX_VALUE && left < getDecryptChunkSize())
               {
                  // what's left is smaller, so this is the last chunk,
                  // so set less to the last decrypt chunk size
                  less = getLastDecryptChunkSize();
               }
               
               // set required bytes according to whichever was less
               requiredBytes = less - length;
            }

            break;
      }
      
      return requiredBytes;
   }
   
   /**
    * Processes the passed data and returns the resulting processed data.
    * 
    * The second parameter indicates whether the passed data is the
    * last data in the stream that is using this interface.
    * 
    * @param data the data to process.
    * @param last whether or not the passed data is the last data
    *        in the associated stream. 
    * @return the processed data.
    */
   public byte[] processStreamData(byte[] data, boolean last)
   {
      byte[] processed = null;
      
      if(data != null)
      {
         // encrypt or decrypt based on crypt mode
         switch(getStreamCryptMode())
         {
            case ENCRYPT:
               processed = encryptChunk(data, last);
               break;
            case DECRYPT:
               processed = decryptChunk(data, last);
               break;
         }
      }
      
      // return processed data
      return processed;
   }
   
   /**
    * Sets the cryptor used to do encryption and decryption.
    * 
    * @param cryptor the cryptor to set.
    */
   public void setCryptor(Cryptor cryptor)
   {
      mCryptor = cryptor;
   }

   /**
    * Gets the cryptor used to do encryption and decryption.
    * 
    * @return the cryptor.
    */
   public Cryptor getCryptor()
   {
      return mCryptor;
   }
   
   /**
    * Sets the current mode for this cryptor. Valid options
    * are ENCRYPT and DECRYPT.
    * 
    * @param mode the mode to set.
    */
   public void setStreamCryptMode(int mode)
   {
      mCryptMode = mode;
   }
   
   /**
    * Gets the current mode for this cryptor. Valid options
    * are ENCRYPT and DECRYPT.
    * 
    * @return the current stream crypt mode.
    */
   public int getStreamCryptMode()
   {
      return mCryptMode;
   }
   
   /**
    * Gets the encrypt chunk size.
    * 
    * @return the encrypt chunk size.
    */
   public int getEncryptChunkSize()
   {
      return mEncryptChunkSize;
   }

   /**
    * Gets the decrypt chunk size.
    * 
    * @return the decrypt chunk size.
    */
   public int getDecryptChunkSize()
   {
      return mDecryptChunkSize;
   }
   
   /**
    * Gets the padding size.
    * 
    * @return the padding size.
    */
   public int getPadding()
   {
      return mPadding;
   }
   
   /**
    * Sets the total number of bytes that have been encrypted.
    * 
    * @param numBytes the number of bytes that have been encrypted.
    */
   public void setNumBytesEncrypted(long numBytes)
   {
      mNumBytesEncrypted = numBytes;
   }
   
   /**
    * Gets the total number of bytes that have been encrypted.
    * 
    * @return the total number of bytes that have been encrypted.
    */
   public long getNumBytesEncrypted()
   {
      return mNumBytesEncrypted;
   }
   
   /**
    * Clears the total bytes encrypted so far.
    */
   public void clearNumBytesEncrypted()
   {
      mNumBytesEncrypted = 0;
   }
   
   /**
    * Gets the total number of decrypted bytes.
    * 
    * @return the total number of decrypted bytes.
    */
   public long getNumDecryptedBytes()
   {
      return mNumDecryptedBytes;
   }
   
   /**
    * Sets the last decrypt chunk size. This is the size of the last
    * chunk to be decrypted, which may differ from the other chunks.
    * 
    * @param size the last decrypt chunk size.
    */
   public void setLastDecryptChunkSize(int size)
   {
      mLastDecryptChunkSize = size;
   }
   
   /**
    * Gets the last decrypt chunk size. This is the size of the last
    * chunk to be decrypted, which may differ from the other chunks.
    * 
    * @return the last decrypt chunk size.
    */
   public int getLastDecryptChunkSize()
   {
      return mLastDecryptChunkSize;
   }
   
   /**
    * Gets the logger.
    * 
    * @return the logger.
    */
   public static Logger getLogger()
   {
      return LoggerManager.getLogger("dbcommon");
   }
}
