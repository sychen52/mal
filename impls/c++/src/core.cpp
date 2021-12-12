#include "core.h"
#include "exception.h"

mal::Type::Ptr
mal::Add::call(const std::vector<Type::Ptr>::const_iterator &start,
               const std::vector<Type::Ptr>::const_iterator &end) {
  int64_t ret = 0;
  for (auto it = start; it != end; ++it) {
    auto ptr = dynamic_cast<Number *>((*it).get());
    if (ptr == nullptr) {
      throw Exception("All arguments are supposed be Number's");
    }
    ret += ptr->number;
  }
  return std::make_shared<Number>(ret);
}

mal::Type::Ptr
mal::Multiply::call(const std::vector<Type::Ptr>::const_iterator &start,
                    const std::vector<Type::Ptr>::const_iterator &end) {
  int64_t ret = 1;
  for (auto it = start; it != end; ++it) {
    auto ptr = dynamic_cast<Number *>((*it).get());
    if (ptr == nullptr) {
      throw Exception("All arguments are supposed be Number's");
    }
    ret *= ptr->number;
  }
  return std::make_shared<Number>(ret);
}

mal::Type::Ptr
mal::Minus::call(const std::vector<Type::Ptr>::const_iterator &start,
                 const std::vector<Type::Ptr>::const_iterator &end) {
  int64_t ret = 0;
  bool only_one_arg = false;
  if (start + 1 == end) {
    only_one_arg = true;
  }
  for (auto it = start; it != end; ++it) {
    auto ptr = dynamic_cast<Number *>((*it).get());
    if (ptr == nullptr) {
      throw Exception("All arguments are supposed be Number's");
    }
    if (it == start && !only_one_arg) {
      ret += ptr->number;
    } else {
      ret -= ptr->number;
    }
  }
  return std::make_shared<Number>(ret);
}

mal::Type::Ptr
mal::Divide::call(const std::vector<Type::Ptr>::const_iterator &start,
                  const std::vector<Type::Ptr>::const_iterator &end) {
  int64_t ret = 1;
  bool only_one_arg = false;
  if (start + 1 == end) {
    only_one_arg = true;
  }
  for (auto it = start; it != end; ++it) {
    auto ptr = dynamic_cast<Number *>((*it).get());
    if (ptr == nullptr) {
      throw Exception("All arguments are supposed be Number's");
    }
    if (it == start && !only_one_arg) {
      ret *= ptr->number;
    } else {
      ret /= ptr->number;
    }
  }
  return std::make_shared<Number>(ret);
}
mal::Env mal::build_env() {
  auto repl_env = mal::Env();
  repl_env.set(mal::Symbol("+"), std::make_shared<mal::Add>());
  repl_env.set(mal::Symbol("-"), std::make_shared<mal::Minus>());
  repl_env.set(mal::Symbol("*"), std::make_shared<mal::Multiply>());
  repl_env.set(mal::Symbol("/"), std::make_shared<mal::Divide>());
  return repl_env;
}
