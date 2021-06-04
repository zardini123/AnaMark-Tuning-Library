// TUN_StringTools.h: Interface of string tool functions and class CStringParser.
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact form at www.mark-henning.de
//
// This file provides some string extensions for std::string
//
//////////////////////////////////////////////////////////////////////

#ifndef ANAMARK_TUNING_LIBRARY_SRC_TUN_STRING_TOOLS_H_
#define ANAMARK_TUNING_LIBRARY_SRC_TUN_STRING_TOOLS_H_

#include <iostream>
#include <list>
#include <string>

namespace TUN {

namespace strx {
//////////////////////////////////////////////////////////////////////
// Character tool functions
//////////////////////////////////////////////////////////////////////

/**
 * Replacement for Windows function __iscsymf.
 * @param  c The character.
 * @return   Non-zero value if c is a letter or an underscore, 0 if c is not a letter or
 * an underscore.
 */
int IsLetterOrUnderscore(int c);

//////////////////////////////////////////////////////////////////////
// String tool functions
//////////////////////////////////////////////////////////////////////

/**
 * List of characters which count as white spaces.
 * Returns a string literal of the following whitespace characters in this order:
 * - Horizontal tab (0x09, '\\t')
 * - Line feed (0x0a, '\\n')
 * - Vertical tab (0x0b, '\\v')
 * - Form feed (0x0c, '\\f')
 * - Carriage return (0x0d, '\\r')
 * - Space (0x20, ' ')
 *
 * Returned string is the same regardless of locale.
 *
 * @return String literal of the popular whitespace characters.
 */
const char *WhiteSpaceChars();

/**
 * Convert all characters to lower case using C++ function
 * [tolower](http://www.cplusplus.com/reference/cctype/tolower/).
 *
 * Uses "C" locale regardless of global locale.
 *
 * Conversion is done in-place.  Input string reference will be modified!
 * Make a copy of string first if you dont want to modify your source string.
 *
 * @param  str Reference to string that will be converted.
 * @return     Reference of the modified input string.
 */
std::string &ToLower(std::string &str);

/**
 * Apply ToLower function on string.  Return a copy, instead of a reference to the
 * modified input.
 * @param  str Reference to string that will be converted to lowercase.
 * @return     Lowercase version of str, returned as a copy.
 */
std::string GetAsLower(const std::string &str);

/**
 * Removes trailing and leading whitespace.
 *
 * The whitespace considered is the whitespace in character array returned by
 * WhiteSpaceChars().
 *
 * Conversion is done in-place.  Input string reference will be modified!
 * Make a copy of string first if you dont want to modify your source string.
 *
 * @param  str Reference of string to trim.
 * @return     Reference of the modified input string.
 */
std::string &Trim(std::string &str);

/**
 * Remove all spaces in str.
 *
 * Space characters are from that of the "C" locale.
 *
 * Conversion is done in-place.  Input string reference will be modified!
 * Make a copy of string first if you dont want to modify your source string.
 * @param  str [description]
 * @return     [description]
 */
std::string &RemoveSpaces(std::string &str);

/**
 * Escape string according to C-like syntax, e.g. tabulator -> "\t".
 *
 * Modification happens in-place (str is modiifed).
 * @param  str String to escape.
 * @return     Reference to @p str (now escaped).
 */
std::string &Escape(std::string &str);

/**
 * Unescape string according to C-like syntax, e.g. "\t" -> tabulator.
 *
 * Modification happens in-place (str is modiifed).
 * @param  str String to unescape.
 * @return     Reference to @p str (now unescaped).
 */
std::string &Unescape(std::string &str);

//////////////////////////////////////////////////////////////////////
// String evaluation functions
//////////////////////////////////////////////////////////////////////

/**
 * Evaluate string as a double with error checking.  Evaluation starts at input_string
 * index pos.
 * @param  input_string Referenced string to evaluate.
 * 											String
 * is not modified.
 * @param  pos          Index in input_string to start evaluating.
 * 											Passed
 * variable modified to be first character after the double (if function returns true).
 * @param  result       Double that was evaluated (if function returns true).
 * @return              False if string at pos was unable to be evaluated as a double.
 */
bool Eval(const std::string &input_string, std::string::size_type &pos, double &result);

/**
 * Evaluate string as a long with error checking.  Evaluation starts at input_string index
 * pos.
 * @param  input_string Referenced string to evaluate.
 * 											String
 * is not modified.
 * @param  pos          Index in input_string to start evaluating.
 * 											Passed
 * variable modified to be first character after the long (if function returns true).
 * @param  result       Long that was evaluated (if function returns true).
 * @return              False if string starting at pos was unable to be evaluated as a
 * long.
 */
bool Eval(const std::string &input_string, std::string::size_type &pos, long &result);

/**
 * Split *trimmed* string "key = value" into key and value and trim the results.
 * @param  input_string String to extract key and value strings from.
 * @param  keyOut       Used as output.  String will be the key part of input_string.
 * @param  valueOut     Used as output.  String will be the value part of input_string
 * converted to lower chars.
 * @return              Returns false if no '=' character in input_string, or the first
 * character of input_string is not a letter or underscore. Returns true otherwise.
 */
bool EvalKeyAndValue(std::string &input_string, std::string &keyOut,
                     std::string &valueOut);

/**
 * If string is like "[sectionname]", '[' and ']' are removed, the string is trimmed and
 * converted to lower. The return value is true.  Otherwise, the string keeps unchanged
 * and the return value is false.
 *
 * Modification happens in-place (str is modiifed).
 * @param  str Section name string.  Of form "[Section Name]".
 * @return     Returns false if the first character is not '[', the last character is not
 *             ']', or the string length is less than 2. Returns true otherwise.
 */
bool EvalSection(std::string &str);

/**
 * If string is like "(Param1, Param2)", '(' and ')' are removed.  The return value is
 * true.  Otherwise, the string keeps unchanged and the return value is false.
 *
 * Modification happens in-place (str is modiifed).
 * @param  str Function parameters string.  Of form "(function parameters)".
 * @return     Returns false if the first character is not '(', the last character is not
 *             ')', or the string length is less than 2. Returns true otherwise.
 */
bool EvalFunctionParam(std::string &str);

/**
 * If string is like "\"an escaped string\"", the double quotes are removed, the string is
 * unescaped and true is returned.  Otherwise, the string keeps unchanged and the return
 * value is false.
 *
 * Modification happens in-place (str is modiifed).
 * @param  str Function parameters string.  Of form "(function parameters)".
 * @return     Returns false if the first character is not '"', the last character is not
 *             '"', or the string length is less than 2. Returns true otherwise.
 */
bool EvalString(std::string &str);

/**
 * Splits a string at the seperator into a list of strings
 * @param input_string                   String to split.  Not modified.
 * @param separator                      Character to split input_string at.
 * @param resulting_strings_out          Each seperate string as a list.  List passed will
 *                                       be cleared to add the substrings!
 * @param trim_resulting_strings         Set to true to trim each split string.
 * @param ignore_any_empty_split_strings Set to true to not add any substrings to
 *                                       resulting_strings if the substring is empty (i.e.
 *                                       no characters).
 */
void Split(std::string &input_string, char separator,
           std::list<std::string> &resulting_strings_out, bool trim_resulting_strings,
           bool ignore_any_empty_split_strings);

//////////////////////////////////////////////////////////////////////
// String construction functions
//////////////////////////////////////////////////////////////////////

/**
 * Convert long to string.
 *
 * @param  value Long to convert.
 * @return       String of the long.
 */
std::string ltostr(long value);

/**
 * Converts double to string.
 *
 * @param  value Double to convert.
 * @return       String of the double.
 */
std::string dtostr(double value);

/**
 * Adds '[' and ']' to the begin and the end, respectively.
 *
 * Part of the TUN specification of having sections being declared with encompassing
 * brackets.
 * @param  str String to add brackets around.  Str is not modified.
 * @return     Returns copy of str with brackets added at beginning and end.
 */
std::string GetAsSection(const std::string &str);

/**
 * Escapes string (via call to function Escape()) and adds '\"' to the begin and the end
 * @param  str String to get as "string."  Str is not modified.
 * @return     Returns copy of str with quotes added to beginning and end, with special
 * characters escaped.
 */
std::string GetAsString(const std::string &str);

} // namespace strx

class CStringParser {
  // Private variables for stream handling
private:
  char EOL_character;
  long line_count;
  std::string line;

public:
  CStringParser() {
    Reset();
  }

  virtual ~CStringParser() {}

  //////////////////////////////////////////////////////////////////////
  // Tool functions for working with streams
  //////////////////////////////////////////////////////////////////////

  std::string &str() {
    return line;
  }

  const std::string &str() const {
    return line;
  }

  // Call this before start reading from the stream
  void Reset() {
    // '@' is a dummy character thats there until the first valid end-of-line character is
    // found, then EOL_character is set to that.
    EOL_character = '@';
    line_count = -1;
  }

  /**
   * Retrieve number of last read line or -1 if no line was read
   * @return The line count in 0-index.  First line is 0, -1 if no line was read.
   */
  long GetLineCount() const {
    return line_count;
  }

  /**
   * Reads from input_string until end-of-line characters \\0, \\r, or \\n are reached,
   * and stores the line read. The line then has its trailing/leading whitespace trimmed.
   *
   * The line read and trimmed can be accessed using str().
   *
   * @param  input_stream       Reference to a input stream.
   * @param  current_line_count A long that represents a line count.  Can be any value.
   * Will be incremented by 1 when a full line is read.
   * @return                    False if input_stream is invalid or EOF.  True otherwise.
   */
  bool GetLineAndTrim(std::istream &input_stream, long &current_line_count) {
    line = "";

    // @REVIEW:  Is reserving 1000 excessive?
    line.reserve(1000); // Should be enough in most cases

    if (!input_stream) {
      current_line_count = line_count;
      return false; // an error occurred or EOF
    }

    // Read from stream, until '\n', '\r', '\0' or EOF is reached
    while (input_stream) {
      char ch = '\0';
      input_stream.read(&ch, 1);
      if ((ch == '\0') || (ch == '\r') || (ch == '\n')) {
        // Remember first found EOL char for this and future encounters of that
        // end-of-line character results in line count increase.
        if (EOL_character == '@')
          EOL_character = ch;

        // Increase line count if trigger character is found
        if (ch == EOL_character)
          current_line_count = ++line_count;
        break; // Done. Line end character reached
      } else
        line.append(1, ch);
    }

    strx::Trim(line);

    return true;
  }

}; // class CStringParser

} // namespace TUN

#endif // ANAMARK_TUNING_LIBRARY_SRC_TUN_STRING_TOOLS_H_
