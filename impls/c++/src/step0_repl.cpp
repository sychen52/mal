#include <string>
#include <iostream>

std::string read(std::string input) {
  return input;
}
std::string eval(std::string input) {
  return input;
}
std::string print(std::string input) {
  return input;
}
std::string rep(std::string input) {
  return print(eval(read(input)));
}


int main() {
  std::string input;
  std::string prompt = "user> ";
  std::cout << prompt;
  while (std::getline(std::cin, input)) {
    auto output = rep(input);
    std::cout << output << std::endl;
    std::cout << prompt;
  }
  return 0;
}
