#include "types.h"
#include "reader.h"
#include <memory>

std::string mal::List::to_string() const {
  std::string ret = "(";
  for (auto it = value_.cbegin(); it != value_.cend(); ++it) {
    if (it != value_.cbegin()) {
      ret += " ";
    }
    ret += (*it)->to_string();
  }
  return ret + ")";
}

mal::ParameterIter mal::List::parameter_iter() const{
  if (this->size() == 0) {
    throw std::runtime_error("call ParameterIter with a empty list");
  }
  auto first = (*this)[0];
  if (first == nullptr) {
    throw std::runtime_error("call ParameterIter with a list whose first element is nullptr");
  }
  return ParameterIter(this->begin() + 1, this->end(), first->to_string());
}

mal::ParameterIter::ParameterIter(const List::Iter &start,
                                  const List::Iter &end,
                                  const std::string &fn)
  : it_(start), end_(end), fn_(fn) {}


void mal::ParameterIter::next() {
  ++it_;
  ++n_;
}

void mal::ParameterIter::no_extra() const{
  if (! is_done()) {
    throw Exception(fn_ + " has extra argument.");
  }
}

mal::Type::Ptr mal::ParameterIter::pop() {
  if (it_ == end_) {
    throw Exception(fn_ + " has not enough number of arguments. Got " +
                    std::to_string(n_) + ".");
  }
  auto ret = *it_;
  next();
  return ret;
}

bool mal::List::operator==(const Type &rhs) const {
  auto other = dynamic_cast<const List *>(&rhs);
  if (other == nullptr) {
    return false;
  }
  if (size() != other->size()) {
    return false;
  }
  for (auto lhs_it = begin(), rhs_it = other->begin(); lhs_it != end();
       ++lhs_it, ++rhs_it) {
    if ((*lhs_it)->operator!=(**rhs_it)) {
      return false;
    }
  }
  return true;
}

std::string unescape(const std::string &s) {
  for (size_t i=0; i < s.size(); ++i) {
    auto find = Reader::unescape(s[i]);
    if (find.has_value()) {
      return s.substr(0, i) + find.value() +
             unescape(s.substr(i + 1, s.size() - i - 1));
    }
  }
  return s;
}

std::string mal::String::to_string(const bool readably) const {
  if (readably) {
    return "\"" + unescape(value_) + "\"";
  }
  return "\"" + value() + "\"";
}

mal::List::Ptr mal::List::cons(const Type::Ptr &exp) {
  auto ret = std::make_shared<List>();
  ret->append(exp);
  for (auto &item : *this) {
    ret->append(item);
  }
  return ret;
}

void mal::List::insert(const List::Ptr &list) {
  value_.insert(value_.end(), list->begin(), list->end());
}
