#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "types.h"

class Reader {
public:
  Reader(const std::vector<std::string>& tokens);
  std::string next();
  std::string peak();
  mal::Type::Ptr read_form();
  mal::List::Ptr read_list();
  mal::Type::Ptr read_atom();
  bool has_more();
  std::string tokens();

  static std::optional<char> escape(const std::string &key);

  static std::optional<std::string> unescape(const char key);

private:
  static const std::array<std::pair<std::string, char>, 3> dictionary;
  std::vector<std::string> tokens_;
  size_t pos_ = 0;
};


std::vector<std::string> tokenize(const std::string& input);

mal::Type::Ptr read_str(const std::string& input);

