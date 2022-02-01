#pragma once
#include <stdexcept>
#include <string>

namespace mal {

  class Exception: public std::runtime_error {
  public:
  Exception(const std::string& message): std::runtime_error(message) {}
  };

  class ReaderException: public Exception {
  public:
  ReaderException(const std::string& message):
    Exception("Read Error: " + message) {}
  };

  class SkipPrintingException: public Exception {
  public:
  SkipPrintingException(): Exception("Skip Printing") {}
  };
}
