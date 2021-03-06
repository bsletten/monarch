/*
 * Copyright (c) 2008-2010 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_io_FileFunctions_H
#define monarch_io_FileFunctions_H

#include <cstdio>
#include <unistd.h>

/**
 * This header provides implementations for POSIX C functions that do not
 * already have implementations on certain platforms (i.e. Windows). The
 * functions are for file and directory querying/manipulation.
 *
 * @author Manu Sporny
 * @author Dave Longley
 */
#ifdef WIN32

// include windows headers for obtaining time
#include <sys/stat.h>
#include <windows.h>

// define file type for symbolic link
#define S_IFLNK   0xA000

/**
 * In windows, there are no symbolic links so lstat does the same
 * thing as stat(), namely, it gets information about a particular file.
 *
 * @param path the path to the file to stat.
 * @param buf the stat structure to populate.
 *
 * @return 0 if the stat was successful, -1 if not, with errno set.
 */
inline static int lstat(const char* path, struct stat* buf)
{
   return stat(path, buf);
}

/**
 * In windows, all file permissions are defaulted to 777, there is no
 * file permission parameter for mkdir().
 *
 * @param path the name of the path to create.
 * @param mode the file permissions.
 *
 * @return 0 on success, -1 with errno set on failure.
 */
inline static int mkdir(const char *path, mode_t mode)
{
   return mkdir(path);
}

// FIXME: access() needs to be defined for windows

#endif // end of defined WIN32

#if defined(WIN32) || defined(MACOS)

#include <cstdlib>

/**
 * Gets a line, delimited by the specified char, from a file. If the passed
 * lineptr is too small or NULL, it will be resized via realloc -- the caller
 * must call free() on the returned lineptr.
 *
 * @param lineptr the line pointer to update.
 * @param n the size of the line to update.
 * @param delim the delimiter character.
 * @param stream the file handle to read from.
 *
 * @return the number of bytes read, including the delimiter, but not
 *         including the null-terminator.
 */
inline static ssize_t getdelim(
   char **lineptr, size_t *n, int delim, FILE *stream)
{
   ssize_t rval = 0;

   // fill string with characters until EOF or delim
   std::string s;
   int c;
   while((c = fgetc(stream)) != EOF)
   {
      s.push_back(c);
      ++rval;

      if(c == delim)
      {
         break;
      }
   }

   // check for read error OR (EOF + no data read, spec says return -1 for that)
   if(ferror(stream) != 0 || (c == EOF && rval == 0))
   {
      rval = -1;
   }
   else
   {
      // get length required for buffer
      ssize_t len = rval + 1;

      // ensure there is enough room in lineptr
      if(*lineptr == NULL || *n < (size_t)rval)
      {
         // reallocate lineptr
         *lineptr = (char*)realloc(*lineptr, len);
         if(*lineptr != NULL)
         {
            // update length
            *n = len;
         }
      }

      // copy string into lineptr
      if(*lineptr != NULL)
      {
         strcpy(*lineptr, s.c_str());
      }
   }

   return rval;
}

#endif // end of defined WIN32 or MACOS

#endif
