/* @(#)anaexception.hpp
 */

#pragma once

#include <exception>
#include <string>

class AnaException : public std::exception
{
 public:
  AnaException(const std::string &message, int code=0): _msg(message), _code(code)
  {
  }

  AnaException(const char* message, int code=0):_msg(message), _code(code)
  {
  }

  virtual ~AnaException() throw ()
    {
    }

  virtual const char* what() const throw ()
    {
      return _msg.c_str();
    }

  int code() const
  {
    return _code;
  }

 protected:
  std::string _msg;
  int _code;
};

