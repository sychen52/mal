#include "printer.h"
#include <iostream>

std::string pr_str(const mal::Type &exp, const bool print_readably) {
  const auto string = dynamic_cast<const mal::String *>(&exp);
  if (string) {
    return string->to_string(print_readably);
  }
  return exp.to_string();
}
