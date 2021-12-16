#include "reader.h"
#include "exception.h"
#include "types.h"
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace {
std::string match_bracket(const std::string &s) {
  if (s == "(") {
    return ")";
  }
  if (s == "[") {
    return "]";
  }
  return "";
}

std::unordered_map<std::string, std::string>
    dictionary({{"\\\n", "\n"}, {"\\\\", "\\"}, {"\\\"", "\""}});

std::string readably(const std::string &input) {
  for (size_t i = 0; i < input.size(); ++i) {
    if (input[i] == '\\') {
      if (i == input.size() - 1) {
        throw mal::ReaderException("EOF before string ends.");
      }
      const auto &key = input.substr(i, 2);
      const auto it = dictionary.find(key);
      if (it == dictionary.end()) {
        throw mal::ReaderException("EOF: wrong escape in string.");
      }
      return input.substr(0, i) + (it->second) +
             readably(input.substr(i + 2, input.size() - i - 2));
    }
  }
  return input;
}
} // namespace

Reader::Reader(const std::vector<std::string> &tokens) : tokens_(tokens) {}

bool Reader::has_more() { return pos_ < tokens_.size(); }

std::string Reader::tokens() {
  std::string ret;
  for (const auto &t : tokens_) {
    ret += t;
    ret += ", ";
  }
  ret += std::to_string(pos_);
  return ret;
}

std::string Reader::next() {
  if (has_more()) {
    return tokens_[pos_++];
  }
  return "";
}

std::string Reader::peak() {
  if (has_more()) {
    return tokens_[pos_];
  }
  return "";
}

mal::Type::Ptr Reader::read_form() {
  auto token = peak();
  if (token == "(" || token == "[") {
    return read_list();
  }
  if (token.empty()) {
    throw mal::ReaderException("");
  }
  return read_atom();
}

mal::List::Ptr Reader::read_list() {
  const auto &start_bracket = next(); // skip the "("
  auto end_bracket = match_bracket(start_bracket);
  auto ret = std::make_shared<mal::List>();
  while (true) {
    const auto &token = peak();
    if (token == end_bracket) {
      next(); // skip the ")"
      break;
    }
    if (token.size() == 0) {
      throw mal::ReaderException("Error: EOF before " + end_bracket);
    }
    ret->append(read_form());
  }
  return ret;
}

mal::Type::Ptr Reader::read_atom() {
  auto token = next();
  if (('0' <= token[0] && '9' >= token[0]) ||
      ((token[0] == '-' || token[0] == '+') && token.size() > 1)) {
    return std::make_shared<mal::Number>(std::stoi(token));
  }
  if (token[0] == '"') {
    if (token.size() < 2 || token[token.size() - 1] != '"') {
      throw mal::ReaderException("EOF before string ends. " + token);
    }
    return std::make_shared<mal::String>(readably(token.substr(1, token.size() - 2)));
  }
  if (token == "true") {
    return std::make_shared<mal::Boolean>(true);
  }
  if (token == "false") {
    return std::make_shared<mal::Boolean>(false);
  }
  if (token == "nil") {
    return std::make_shared<mal::Nil>();
  }
  return std::make_shared<mal::Symbol>(token);
}

std::vector<std::string> tokenize(const std::string &input) {
  std::vector<std::string> ret;
  std::string s = input;
  std::smatch m;
  std::regex e("[\\s,]*(~@|[\\[\\]{}()'`~^@]|\"(?:\\\\.|[^\\\\\"])*\"?|;.*|[^"
               "\\s\\[\\]{}('\"`,;)]+)");
  while (std::regex_search(s, m, e)) {
    if (m.size() != 2 || m[1].str().empty()) {
      throw mal::ReaderException("this should not happen: " + s);
    }
    if (m.position() != 0) {
      throw mal::ReaderException("EOF invalid input: " + s);
    }
    ret.emplace_back(m[1].str());
    s = m.suffix().str();
  }
  return ret;
}

//#include <iostream>
mal::Type::Ptr read_str(const std::string &input) {
  auto tokens = tokenize(input);
  auto reader = Reader(tokens);
  auto ret = reader.read_form();
  if (reader.has_more()) {
    throw mal::ReaderException("More than one expression. " + reader.tokens());
  }
  // std::cout << "!!!!!!!!!!!!!!!!!!!!" << std::endl;
  // std::cout << reader.tokens() << std::endl;
  // std::cout << "!!!!!!!!!!!!!!!!!!!!" << std::endl;
  return ret;
}
