#ifndef IException_h
#define IException_h

/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2009/07/29 21:16:39 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <exception>

template <typename T> class QList;

/**
 * Macro for the filename and line number. This is typically used for the last
 *   arguments to constructing an IException.
 */
#define _FILEINFO_ __FILE__,__LINE__

namespace Isis {
  class iString;
  class Pvl;

  /**
   * @brief Isis exception class
   *
   * This class represents a general Isis exception. It contains an enumeration
   * for what type of error the exception represents, and can optionally track
   * what exceptions caused the current exception. This class also provides
   * access to string and PVL representations of the exception.
   *
   * Instances of this class should be thrown by value and caught be reference.
   * Please see the constructor documentation for code examples on how to create
   * and throw exceptions.
   *
   * All methods in this class are re-entrant.
   *
   * @author ????-??-?? Jeff Anderson
   *
   * @internal
   *   @history 2005-05-10 Leah Dahmer - Added class documentation
   *   @history 2005-12-28 Elizabeth Miller - Fixed bug in Pvl error output
   *   @history 2006-06-12 Tracie Sucharski - Change clear method to static
   *   @history 2006-11-02 Jeff Anderson - Fixed bug in Report method for
   *                           exit status
   *   @history 2007-12-31 Steven Lambright - Added stack trace
   *   @history 2008-05-23 Steven Lambright - Added stack trace
   *   @history 2008-06-18 Stuart Sides - Fixed doc error
   *   @history 2008-07-08 Steven Lambright - Changed memory cleanup; now uses
   *                           atexit
   *   @history 2008-10-30 Steven Lambright - iException::Message now takes a
   *                           const char* for the filename, instead of a char*,
   *                           issue pointed out by "novus0x2a" (Support Board
   *                           Member)
   *   @history 2008-12-15 Steven Lambright - iException::what no longer returns
   *                           deleted memory.
   *   @history 2009-07-29 Steven Lambright - Stack trace calculations moved to
   *                           IsisDebug.h
   *   @history 2011-08-05 Steven Lambright - Hacked to make multi-threaded
   *                           code which throws exceptions work.
   *   @history 2012-03-07 Jai Rideout and Steven Lambright - Completely
   *                           refactored how this class works so that it no
   *                           longer stores messages in static memory. This
   *                           refactoring was necessary for the upcoming
   *                           control network suite project to allow this class
   *                           to work with multi-threading. The ErrorType enum
   *                           was also shortened to include only four relevant
   *                           error types, as the other error types were often
   *                           misused, ambiguous, completely forgotten, and
   *                           not helpful to users. The code was updated
   *                           appropriately to follow the current Isis coding
   *                           standards.
   */
  class IException : public std::exception {
    public:
      /**
       * Contains a set of exception error types. These types indicate the
       * source of the error. For example, if the error type is User, this
       * indicates that the exception was thrown due to something the user did
       * wrong, such as provide a cube with no SPICE data to an application that
       * needed a spiceinit'd cube as input.
       */
      enum ErrorType {
        /**
         * A type of error that cannot be classified as any of the other error
         * types. This should be the most common error type.
         *
         * If in doubt, always use this error type.
         */
        Unknown = 1,

        /**
         * A type of error that could only have occurred due to a mistake on the
         * user's part (e.g. bad input to the application). You must be able to
         * guarantee that the user provided the input in the context that is
         * throwing the error.
         */
        User,

        /**
         * This error is for when a programmer made an API call that was
         * illegal.
         *
         * This includes:
         *   Making an API call with illegal input data that could and should
         *   have been checked before the call was made. For example, an out of
         *   array bounds exception is a programmer exception because the caller
         *   should have checked the size of the array.
         *
         *   Making an API call that requires a certain class state. For
         *   example, you need to call open before read.
         *
         * These categories have a lot of overlap, but they are what you are
         * looking for. A programmer exception is not appropriate if the caller
         * has no way to validate their inputs to a method ahead of time, or if
         * the caller is not expected to validate their inputs ahead of time.
         */
        Programmer,

        /**
         * A type of error that occurred when performing an actual I/O
         * operation. For example, fread/fwrite calls. This also includes
         * files not existing, inter-processes socket communications, and
         * network communications. This does not include a file not having the
         * expected values inside of it (for example, reading a corrupted PVL).
         */
        Io
      };

      IException();

      IException(ErrorType type, const char *message,
                 const char *fileName, int lineNumber);

      IException(ErrorType type, const iString &message,
                 const char *fileName, int lineNumber);

      IException(const IException &caughtException,
                 ErrorType newExceptionType, const char *message,
                 const char *fileName, int lineNumber);

      IException(const IException &caughtException,
                 ErrorType newExceptionType, const iString &message,
                 const char *fileName, int lineNumber);

      IException(const IException &other);

      ~IException() throw();

      const char *what() const throw();

      void append(const IException &exceptionSource);

      ErrorType errorType() const;
      void print() const;
      void print(bool printFileInfo) const;
      Pvl toPvl() const;
      iString toString() const;
      iString toString(bool printFileInfo) const;

      void swap(IException &other);
      IException &operator=(const IException &rhs);

    private:
      static IException createStackTrace();
      static iString errorTypeToString(ErrorType t);
      static ErrorType stringToErrorType(const iString &s);
      char *buildWhat() const;
      void deleteEmptyMemberStrings();

    private:
      /**
       * This is used to store the return value of what() in a way that
       *   guarantees the returned data will not be deleted as long as this
       *   instance is unmodified. Any changes to the current exception will
       *   reallocate this and invalidate old values of what().
       */
      char * m_what;

      /**
       * This exception's error source. If the source cannot be positively
       *   identified, then this should be set to Unknown.
       */
      ErrorType m_errorType;

      /**
       * The message associated with this exception. This will be NULL if the
       *   message is empty (or only contained whitespace).
       */
      iString * m_message;

      /**
       * The source code file that threw this exception.
       */
      iString * m_fileName;

      /**
       * The line in the source code file that threw this exception.
       */
      int m_lineNumber;

      /**
       * A list of exceptions that caused this exception.
       */
      QList<IException> * m_previousExceptions;
  };
};

#endif
