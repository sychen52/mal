#include "env.h"
#include "exception.h"
#include "printer.h"
#include "reader.h"
#include "types.h"
#include "core.h"
#include "special_form.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::EnvFrame::WeakPtr env);

mal::Type::Ptr apply(const mal::List *ret_list_ptr) {
  auto func_ptr = dynamic_cast<mal::Applicable *>((*ret_list_ptr)[0].get());
  if (func_ptr == nullptr) {
    throw mal::Exception("First element is not a applicable");
  }
  return func_ptr->apply(ret_list_ptr->parameter_iter());
}

mal::Type::Ptr eval_ast(const mal::Type::Ptr ast, mal::EnvFrame::WeakPtr env) {
  // symbol
  auto symbol_ptr = dynamic_cast<const mal::Symbol *>(ast.get());
  if (symbol_ptr != nullptr) {
    // this is the whole point of using shared_ptr for mal::Type! Otherwise, unique_ptr is enough.
    auto ret = env.lock()->get(*symbol_ptr);
    if (ret == nullptr) {
      throw mal::Exception(symbol_ptr->value() + " not found.");
    }
    return ret;
  }

  // list
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

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::EnvFrame::WeakPtr env) {
  // not a list
  auto list_ptr = dynamic_cast<mal::List *>(ast.get());
  if (list_ptr == nullptr) {
    return eval_ast(ast, env);
  }
  // empty list
  if (list_ptr->empty()) {
    return ast;
  }

  auto special_form = mal::build_special_form(*list_ptr, env);
  if (special_form) {
    return (*special_form)();
  }

  // normal list
  // apply section
  auto ret = eval_ast(ast, env);
  auto ret_list_ptr = dynamic_cast<mal::List *>(ret.get());
  if (ret_list_ptr == nullptr) {
    throw std::invalid_argument(
        "The return of eval_ast with a list input should be another list");
  }
  if (ret_list_ptr->empty()) {
    throw std::invalid_argument("eval_ast return an empty list");
  }
  return apply(ret_list_ptr);
}

std::string PRINT(mal::Type::Ptr input) {
  return pr_str(*input, true);
}

std::string rep(const std::string& input, mal::EnvFrame::WeakPtr env) {
  return PRINT(EVAL(READ(input), env));
}

int main() {
  std::string input;
  auto repl_env = mal::build_env();
  rep("(def! not (fn* (a) (if a false true)))", repl_env);
  rep("(def! load-file (fn* (f) (eval (read-string (str \"(do \" (slurp f) \"\nnil)\")))))", repl_env);

  std::string prompt = "user> ";
  std::cout << prompt;
  while (std::getline(std::cin, input)) {
    try {
      auto output = rep(input, repl_env);
      std::cout << output << std::endl;
    }
    catch (const mal::SkipPrintingException &) {
    }
    catch (const mal::Exception &e) {
      std::cout << e.what() << std::endl;
    }
    std::cout << prompt;
  }
  return 0;
}
