#include "reader.h"
#include "exception.h"
#include "types.h"
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>

std::string match_bracket(const std::string &s) {
  if (s == "(") {
    return ")";
  }
  if (s == "[") {
    return "]";
  }
  return "";
}

std::string readably(const std::string &input) {
  for (size_t i = 0; i < input.size(); ++i) {
    if (input[i] == '\\') {
      if (i == input.size() - 1) {
        throw mal::ReaderException("EOF before string ends.");
      }
      const auto &key = input.substr(i, 2);
      const auto it = Reader::escape(key);
      if (!it.has_value()) {
        throw mal::ReaderException("EOF: wrong escape in string.");
      }
      return input.substr(0, i) + (it.value()) +
             readably(input.substr(i + 2, input.size() - i - 2));
    }
  }
  return input;
}

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
  if (token == "'") {
    next();
    return std::make_shared<mal::List>(std::vector<mal::Type::Ptr>{
        std::make_shared<mal::Symbol>("quote"), read_form()});
  }
  if (token == "`") {
    next();
    return std::make_shared<mal::List>(std::vector<mal::Type::Ptr>{
        std::make_shared<mal::Symbol>("quasiquote"), read_form()});
  }
  if (token == "~") {
    next();
    return std::make_shared<mal::List>(std::vector<mal::Type::Ptr>{
        std::make_shared<mal::Symbol>("unquote"), read_form()});
  }
  if (token == "~@") {
    next();
    return std::make_shared<mal::List>(std::vector<mal::Type::Ptr>{
        std::make_shared<mal::Symbol>("splice-unquote"), read_form()});
  }
  if (token.empty()) {
    throw mal::SkipPrintingException();
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
  try {
    return std::make_shared<mal::Number>(std::stoi(token));
  }
  catch (const std::invalid_argument&) {}
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
    const auto& token = m[1].str();
    if (token.size() >= 1 && token[0] == ';') {
      //skip comments
    }
    else {
      ret.emplace_back(token);
    }
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
  //std::cout << "!!!!!!!!!!!!!!!!!!!!" << std::endl;
  //std::cout << reader.tokens() << std::endl;
  //std::cout << "!!!!!!!!!!!!!!!!!!!!" << std::endl;
  return ret;
}

const std::array<std::pair<std::string, char>, 3> Reader::dictionary = {
    {{"\\n", '\n'}, {"\\\\", '\\'}, {"\\\"", '"'}}};

std::optional<std::string> Reader::unescape(const char key) {
  for (const auto &pair : dictionary) {
    if (pair.second == key) {
      return std::make_optional<std::string>(pair.first);
    }
  }
  return {};
}

std::optional<char> Reader::escape(const std::string &key) {
  for (const auto &pair : dictionary) {
    if (pair.first == key) {
      return std::make_optional<char>(pair.second);
    }
  }
  return {};
}
