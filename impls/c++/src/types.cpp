#include "types.h"

std::string mal::List::to_string() const {
  std::string ret = "(";
  for (auto it = list_.cbegin(); it != list_.cend(); ++it) {
    if (it != list_.cbegin()) {
      ret += " ";
    }
    ret += (*it)->to_string();
  }
  return ret + ")";
}
