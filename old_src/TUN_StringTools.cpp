// TUN_StringTools.h: Implementation of string tool functions
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact form at www.mark-henning.de
//
// You may use this code for free. If you find an error or make some
// interesting changes, please let me know.
//
//////////////////////////////////////////////////////////////////////

#include "TUN_StringTools.h"

#include <cassert>
#include <cctype>
#include <cstring>
#include <iostream>
#include <list>
#include <locale>
#include <string>

namespace TUN {

namespace strx {

//////////////////////////////////////////////////////////////////////
// Character tool functions
//////////////////////////////////////////////////////////////////////

int IsLetterOrUnderscore(int c) {
  return std::isalpha(static_cast<char>(c), std::locale("C")) || c == '_';
}

//////////////////////////////////////////////////////////////////////
// String tool functions
//////////////////////////////////////////////////////////////////////

const char *WhiteSpaceChars() {
  return "\x09\x0a\x0b\x0c\x0d\x20";
}

std::string &ToLower(std::string &str) {
  for (std::string::size_type l = 0; l < str.size(); ++l)
    str.at(l) = static_cast<char>(tolower(str.at(l), std::locale("C")));
  return str;
}

std::string GetAsLower(const std::string &str) {
  std::string copy = std::string(str);
  return ToLower(copy);
}

std::string &Trim(std::string &str) {
  std::string::size_type posFirst = str.find_first_not_of(WhiteSpaceChars());
  if (posFirst == std::string::npos)
    str = "";
  else
    str = str.substr(posFirst, str.find_last_not_of(WhiteSpaceChars()) - posFirst + 1);
  return str;
}

std::string &RemoveSpaces(std::string &str) {
  // We're doing in-place conversion here:
  std::string::size_type read_position = 0;
  std::string::size_type write_position = 0;

  while (read_position < str.size()) {
    char ch = str.at(read_position++);
    if (!isspace(ch, std::locale("C")))
      str.at(write_position++) = ch;
  } // while ( read_position < size() )

  // Remove the remaining chars of the original string
  str.erase(write_position);

  return str;
}

std::string &Escape(std::string &str) {
  std::string escaped_string;
  escaped_string.reserve(str.size() * 2 + 1); // Avoids reallocation in most cases

  for (std::string::size_type l = 0; l < str.size(); ++l) {
    switch (str.at(l)) {
    case '\0': escaped_string += "\\0"; break;  // Nullbyte
    case '\a': escaped_string += "\\a"; break;  // Bell (alert)
    case '\b': escaped_string += "\\b"; break;  // Backspace
    case '\f': escaped_string += "\\f"; break;  // Formfeed
    case '\n': escaped_string += "\\n"; break;  // New line
    case '\r': escaped_string += "\\r"; break;  // Carriage return
    case '\t': escaped_string += "\\t"; break;  // Horizontal tab
    case '\v': escaped_string += "\\v"; break;  // Vertical tab
    case '\'': escaped_string += "\\\'"; break; // Single quotation mark
    case '\"': escaped_string += "\\\""; break; // Double quotation mark
    case '\\': escaped_string += "\\\\"; break; // Backslash
    case '\?': escaped_string += "\\?"; break;  // Literal question mark
    default:
      // Characters less than 0x20 are all control characters
      // @REVIEW: Why is character 0xff (i.e. ÿ ) part of this escape case?
      // If char is a control character or is ÿ (Latin small letter y with diaeresis)
      if ((static_cast<unsigned char>(str.at(l)) < 0x20) ||
          (static_cast<unsigned char>(str.at(l)) == 0xff)) {
        // @REVIEW:  Why do hexidecimal escape sequences need a 0 after the \x?
        escaped_string += "\\x0";
        unsigned char ch = static_cast<unsigned char>(str.at(l));

        // Removed hexidecimal ltoa usage
        // Original usage:
        // 	 escaped_string += ltoa(static_cast<unsigned char>(str.at(l)), szHex, 16);
        char *hex_char_array = nullptr;

        // @FIXME: Snprintf locale dependent!!

        // Get the required length of the string
        int required_size = snprintf(hex_char_array, 0, "%x", ch);

        hex_char_array = (char *)malloc(required_size * sizeof(char));
        snprintf(hex_char_array, required_size, "%x", ch);

        free(hex_char_array);

        escaped_string += hex_char_array;
      } else // Otherwise just add the character normally (without escaping)
        escaped_string += str.at(l);
    }
  }

  str = escaped_string;

  return str;
}

std::string &Unescape(std::string &str) {
  // We're doing in-place conversion here:
  std::string::size_type posR = 0;
  std::string::size_type posW = 0;

  while (posR < str.size()) {
    char ch = str.at(posR++);

    if ((ch == '\\') && (posR < str.size()))
      switch (ch = str.at(posR++)) {
      case '0': ch = '\0'; break;  // Nullbyte
      case 'a': ch = '\a'; break;  // Bell (alert)
      case 'b': ch = '\b'; break;  // Backspace
      case 'f': ch = '\f'; break;  // Formfeed
      case 'n': ch = '\n'; break;  // New line
      case 'r': ch = '\r'; break;  // Carriage return
      case 't': ch = '\t'; break;  // Horizontal tab
      case 'v': ch = '\v'; break;  // Vertical tab
      case '\'': ch = '\''; break; // Single quotation mark
      case '\"': ch = '\"'; break; // Double quotation mark
      case '\\': ch = '\\'; break; // Backslash
      case '?': ch = '\?'; break;  // Literal question mark
      case 'x':                    // Hex representation
        ch = strtol(("0x0" + str.substr(posR, 3)).c_str(), NULL, 16);
        posR += 3;
        break;
      } // switch ( at(posR) )

    str.at(posW++) = ch;
  } // while ( posR < size() )

  // Remove the remaining chars of the original string
  str.erase(posW);

  return str;
}

//////////////////////////////////////////////////////////////////////
// String evaluation functions
//////////////////////////////////////////////////////////////////////

bool Eval(const std::string &input_string, std::string::size_type &pos, double &result) {
  const char *beginning_ptr = input_string.c_str() + pos;
  char *character_after_double_ptr;

  // Convert string to double at beginning_ptr, and sets character_after_double_ptr to the
  // first character after the double
  // FIXME: Locale dependence!!
  result = strtod(beginning_ptr, &character_after_double_ptr); // conversion
  // Add distance of beginning to end of double (in bytes a.k.a chars) to pos
  // Will provide users with position of first character after the double
  pos += character_after_double_ptr - beginning_ptr;

  // Return false if an error occurred when converting double
  return (beginning_ptr != character_after_double_ptr);
}

bool Eval(const std::string &input_string, std::string::size_type &pos, long &result) {
  const char *beginning_ptr = input_string.c_str() + pos;
  char *character_after_long_ptr;

  // Convert string to long at beginning_ptr, and sets character_after_long_ptr to the
  // first character after the long
  // FIXME: Locale dependence!!
  result = strtol(beginning_ptr, &character_after_long_ptr, 10); // conversion
  // Add distance of beginning to end of double (in bytes a.k.a chars) to pos
  // Will provide users with position of first character after the double
  pos += character_after_long_ptr - beginning_ptr;

  // Return false if an error occurred when converting long
  return (beginning_ptr != character_after_long_ptr);
}

bool EvalKeyAndValue(std::string &input_string, std::string &keyOut,
                     std::string &valueOut) {
  std::string::size_type pos = input_string.find('=');

  // IsLetterOrUnderscore replaced __iscsymf
  if ((pos == std::string::npos) || (!IsLetterOrUnderscore(input_string.at(0))))
    return false; // error: no '=' or first char of key is invalid

  // Split key and value around the '=' character
  std::string keyTemp = input_string.substr(0, pos);
  keyOut = ToLower(Trim(keyTemp));

  std::string valueTemp = input_string.substr(pos + 1);
  valueOut = Trim(valueTemp);
  return true;
}

bool EvalSection(std::string &str) {
  if ((str.size() < 2) || (str.at(0) != '[') || (str.at(str.size() - 1) != ']'))
    return false;

  std::string temp = str.substr(1, str.size() - 2);
  str = ToLower(Trim(temp));
  return true;
}

bool EvalFunctionParam(std::string &str) {
  if ((str.size() < 2) || (str.at(0) != '(') || (str.at(str.size() - 1) != ')'))
    return false;
  str = str.substr(1, str.size() - 2);
  return true;
}

bool EvalString(std::string &str) {
  if ((str.size() < 2) || (str.at(0) != '\"') || (str.at(str.size() - 1) != '\"'))
    return false;

  std::string temp = str.substr(1, str.size() - 2);
  str = Unescape(temp);
  return true;
}

void Split(std::string &input_string, char separator,
           std::list<std::string> &resulting_strings_out, bool trim_resulting_strings,
           bool ignore_any_empty_split_strings) {
  // Initialize list
  resulting_strings_out.clear();

  // Split string
  std::string::size_type current_find_start_index = 0;
  std::string::size_type first_seperator_found_position = 0;

  while (true) {
    if (first_seperator_found_position == std::string::npos)
      return;

    // Find the next separator and extract the item
    first_seperator_found_position =
        input_string.find(separator, current_find_start_index);
    std::string current_substring;
    if (first_seperator_found_position == std::string::npos)
      current_substring = input_string.substr(current_find_start_index);
    else
      current_substring =
          input_string.substr(current_find_start_index,
                              first_seperator_found_position - current_find_start_index);
    current_find_start_index = first_seperator_found_position + 1;

    // Process the item
    if (trim_resulting_strings)
      Trim(current_substring);
    if (ignore_any_empty_split_strings && current_substring.empty())
      continue;
    resulting_strings_out.push_back(current_substring);
  }
}

//////////////////////////////////////////////////////////////////////
// String construction functions
//////////////////////////////////////////////////////////////////////

std::string ltostr(long value) {
  // @CHANGED:  Removed ltoa usage
  // Original usage:
  // char	sz[11]; // Enough for 32-Bit long
  // 	...ltoa(value, sz, 10)...

  // @FIXME:  Locale dependence!
  return std::to_string(value);
}

std::string dtostr(double value) {
  // Original call was _gcvt, where _gcvt was Windows only

  char *value_as_string = nullptr;

  // @FIXME: Snprintf locale dependent!!

  // Get the required length of the string
  int required_size = snprintf(value_as_string, 0, "%.20g", value);

  value_as_string = (char *)malloc(required_size * sizeof(char));
  snprintf(value_as_string, required_size, "%.20g", value);

  std::string final(value_as_string);

  free(value_as_string);

  return final;
}

std::string GetAsSection(const std::string &str) {
  return "[" + str + "]";
}

std::string GetAsString(const std::string &str) {
  std::string temp = std::string(str);
  return "\"" + Escape(temp) + "\"";
}

} // namespace strx

} // namespace TUN
