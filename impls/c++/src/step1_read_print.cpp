#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>
#include "reader.h"
#include "printer.h"
#include "types.h"
#include "exception.h"

mal::Type::Ptr READ(const std::string& input) {
  return read_str(input);
}
mal::Type::Ptr EVAL(mal::Type::Ptr input) {
  return input;
}
std::string PRINT(mal::Type::Ptr input) {
  return pr_str(*input);
}
std::string rep(std::string input) {
  return PRINT(EVAL(READ(input)));
}

int main() {
  std::string input;
  std::string prompt = "user> ";
  std::cout << prompt;
  while (std::getline(std::cin, input)) {
    try {
      auto output = rep(input);
      std::cout << output << std::endl;
    }
    catch (const mal::ReaderException& e){
      std::cout << e.what() << std::endl;
    }
    std::cout << prompt;
  }
  return 0;
}
