#include "env.h"
#include "printer.h"
#include "reader.h"
#include "types.h"
#include "core.h"
#include "special_form.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>


mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::EnvFrame::Ptr env);

mal::Type::Ptr apply(const mal::List *ret_list_ptr) {
  auto func_ptr = dynamic_cast<mal::Applicable *>((*ret_list_ptr)[0].get());
  if (func_ptr == nullptr) {
    throw mal::Exception("First element is not a applicable");
  }
  return func_ptr->apply(ret_list_ptr->parameter_iter());
}

mal::Type::Ptr eval_ast(const mal::Type::Ptr ast, mal::EnvFrame::Ptr env) {
  // symbol
  auto symbol_ptr = dynamic_cast<const mal::Symbol *>(ast.get());
  if (symbol_ptr != nullptr) {
    // this is the whole point of using shared_ptr! Otherwise, unique_ptr is enough.
    return env->get(*symbol_ptr);
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

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::EnvFrame::Ptr env) {
  // not a list
  auto list_ptr = dynamic_cast<mal::List *>(ast.get());
  if (list_ptr == nullptr) {
    return eval_ast(ast, env);
  }
  // empty list
  if (list_ptr->empty()) {
    return ast;
  }
  // def!
  if (is_form(list_ptr, "def!")) {
     return def_bang(list_ptr->parameter_iter(), env);
  }
  // let*
  if (is_form(list_ptr, "let*")) {
    return let_star(list_ptr->parameter_iter(), env);
  }
  // do
  if (is_form(list_ptr, "do")) {
    return do_form(list_ptr->parameter_iter(), env);
  }
  // if
  if (is_form(list_ptr, "if")) {
    return if_form(list_ptr->parameter_iter(), env);
  }
  // fn*
  if (is_form(list_ptr, "fn*")) {
    return fn_star(list_ptr->parameter_iter(), env);
  }

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

std::string PRINT(mal::Type::Ptr input) { return pr_str(*input); }

std::string rep(const std::string& input, mal::EnvFrame::Ptr env) {
  return PRINT(EVAL(READ(input), env));
}

int main() {
  std::string input;
  auto repl_env = mal::build_env();

  std::string prompt = "user> ";
  std::cout << prompt;
  while (std::getline(std::cin, input)) {
    try {
      auto output = rep(input, repl_env);
      std::cout << output << std::endl;
    }
    catch (const mal::Exception &e) {
      std::cout << e.what() << std::endl;
    }
    std::cout << prompt;
  }
  return 0;
}
