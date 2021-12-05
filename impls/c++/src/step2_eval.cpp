#include "env.h"
#include "printer.h"
#include "reader.h"
#include "types.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::Env &env);

mal::Type::Ptr eval_ast(const mal::Type::Ptr ast, mal::Env &env) {
  auto symbol_ptr = dynamic_cast<const mal::Symbol *>(ast.get());
  if (symbol_ptr != nullptr) {
    return env.get(*symbol_ptr);
  }
  auto list_ptr = dynamic_cast<const mal::List *>(ast.get());
  if (list_ptr != nullptr) {
    auto ret = std::make_shared<mal::List>();
    for (auto it = list_ptr->begin(); it != list_ptr->end(); ++it) {
      ret->append(EVAL(*it, env));
    }
    return ret;
  }
  return ast;
}

mal::Type::Ptr READ(const std::string &input) { return read_str(input); }

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::Env &env) {
  // not a list
  auto list_ptr = dynamic_cast<mal::List *>(ast.get());
  if (list_ptr == nullptr) {
    return eval_ast(ast, env);
  }
  // empty list
  if (list_ptr->empty()) {
    return ast;
  }

  auto ret = eval_ast(ast, env);
  auto ret_list_ptr = dynamic_cast<mal::List *>(ret.get());
  if (ret_list_ptr == nullptr) {
    throw std::invalid_argument(
        "The return of eval_ast with a list input should be another list");
  }
  if (ret_list_ptr->empty()) {
    throw std::invalid_argument("eval_ast return an empty list");
  }
  auto func_ptr = dynamic_cast<mal::BuiltinFunction *>((*ret_list_ptr)[0]);
  if (func_ptr == nullptr) {
    throw mal::Exception("First element is not a function");
  }
  return func_ptr->call(ret_list_ptr->begin() + 1, ret_list_ptr->end());
}

std::string PRINT(mal::Type::Ptr input) { return pr_str(*input); }

std::string rep(const std::string& input, mal::Env& env) {
  return PRINT(EVAL(READ(input), env));
}

int main() {
  std::string input;
  auto env = mal::Env();
  env.set(mal::Symbol("+"), std::make_shared<mal::Add>());
  env.set(mal::Symbol("-"), std::make_shared<mal::Minus>());
  env.set(mal::Symbol("*"), std::make_shared<mal::Multiply>());
  env.set(mal::Symbol("/"), std::make_shared<mal::Divide>());

  std::string prompt = "user> ";
  std::cout << prompt;
  while (std::getline(std::cin, input)) {
    try {
      auto output = rep(input, env);
      std::cout << output << std::endl;
    } catch (const mal::Exception &e) {
      std::cout << e.what() << std::endl;
    }
    std::cout << prompt;
  }
  return 0;
}
