/*
 * utils.h
 *
 *  Created on: Apr 4, 2018
 *      Author: jd
 *
 * Some general helper functions
 *
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>


/**
 * Exception thrown in case of an erroneous conversion
 */

class BadConversion : public std::runtime_error {
public:
  BadConversion(const std::string& s)
    : std::runtime_error(s)
    { }
};

/**
 * Convert a string to a double value
 *
 * @param   s   The string
 * @return      A double value
 * @throws #BadConversion in case of an error
 */
inline double convertToDouble(const std::string& s)
{
  std::istringstream i(s);
  double x;
  if (!(i >> x))
    throw BadConversion("convertToDouble(\"" + s + "\")");
  return x;
}

/**
 * Convert a string to a long value
 *
 * @param   s   The string
 * @return      A long value
 * @throws #BadConversion in case of an error
 */
inline double convertToLong(const std::string& s)
{
  std::istringstream i(s);
  long x;
  if (!(i >> x))
    throw BadConversion("convertToLong(\"" + s + "\")");
  return x;
}

/**
 * Convert a string to an int value
 *
 * @param   s   The string
 * @return      An integer value
 * @throws #BadConversion in case of an error
 */
inline double convertToInt(const std::string& s)
{
  std::istringstream i(s);
  int x;
  if (!(i >> x))
    throw BadConversion("convertToInt(\"" + s + "\")");
  return x;
}

/**
 * Convert a string to a boolean value
 *
 * @param   s   The string
 * @return      A boolean value
 * @throws #BadConversion in case of an error
 */
inline bool convertToBool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}



#endif /* UTILS_H_ */
