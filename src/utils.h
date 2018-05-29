/*
 * utils.h
 *
 *  Created on: Apr 4, 2018
 *      Author: jd
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

class BadConversion : public std::runtime_error {
public:
  BadConversion(const std::string& s)
    : std::runtime_error(s)
    { }
};

inline double convertToDouble(const std::string& s)
{
  std::istringstream i(s);
  double x;
  if (!(i >> x))
    throw BadConversion("convertToDouble(\"" + s + "\")");
  return x;
}

inline double convertToLong(const std::string& s)
{
  std::istringstream i(s);
  long x;
  if (!(i >> x))
    throw BadConversion("convertToLong(\"" + s + "\")");
  return x;
}

inline double convertToInt(const std::string& s)
{
  std::istringstream i(s);
  int x;
  if (!(i >> x))
    throw BadConversion("convertToInt(\"" + s + "\")");
  return x;
}


inline bool convertToBool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}



#endif /* UTILS_H_ */
