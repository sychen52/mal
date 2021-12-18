#include "env.h"
#include "printer.h"
#include "reader.h"
#include "types.h"
#include "core.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::Env &env);

bool is_form(const mal::List* list, const std::string& key) {
  auto first = dynamic_cast<mal::Symbol*>((*list)[0].get());
  if (first == nullptr || first->value() != key) {
    return false;
  }
  return true;
}

mal::Type::Ptr def_bang(mal::ParameterIter&& it, mal::Env& env) {
  auto second = it.pop<mal::Symbol>();
  auto ret = it.pop();
  it.no_extra();
  env.set(*second, ret);
  return ret;
}

mal::Type::Ptr let_star(mal::ParameterIter&& it, mal::Env& env) {
  auto second = it.pop<mal::List>();
  auto third = it.pop();
  it.no_extra();
  if (second->size() % 2 == 1) {
    throw mal::Exception("let*'s first argument must be list with even number of item");
  }
  mal::Env new_env(&env);
  for (size_t i = 0; i < second->size(); i+=2) {
    auto symbol = dynamic_cast<mal::Symbol*>((*second)[i].get());
    if (symbol == nullptr) {
      throw mal::Exception("the odd item in let* list must be symbol");
    }
    new_env.set(*symbol, EVAL((*second)[i+1], new_env));
  }
  return EVAL(third, new_env);
}

mal::Type::Ptr do_form(mal::ParameterIter&& it, mal::Env& env) {
  if (it.is_done()) {
    throw mal::Exception("do must have at least 1 argument.");
  }
  mal::Type::Ptr ret = nullptr;
  while (!it.is_done()) {
    ret = EVAL(it.pop(), env);
  }
  return ret;
}

mal::Type::Ptr if_form(mal::ParameterIter&& it, mal::Env& env) {
  auto second = it.pop();
  auto third = it.pop();
  auto fourth = it.pop();
  it.no_extra();
  auto condiction = EVAL(second, env);
  if (dynamic_cast<mal::Nil*>(condiction.get()) != nullptr) {
    return EVAL(fourth, env);
  }
  if (auto f = dynamic_cast<mal::Boolean*>(condiction.get());
      f != nullptr && (!f->value())) {
    return EVAL(fourth, env);
  }
  return EVAL(third, env);
}

mal::Type::Ptr fn_star(mal::ParameterIter&& it, mal::Env& env) {
  auto second = it.pop();
  auto third = it.pop();
  it.no_extra();
  return std::make_shared<mal::Function>(second, third, &env);
}

mal::Type::Ptr apply(const mal::List *ret_list_ptr) {
  auto func_ptr = dynamic_cast<mal::Callable *>((*ret_list_ptr)[0].get());
  if (func_ptr == nullptr) {
    throw mal::Exception("First element is not a callable");
  }
  return func_ptr->call(ret_list_ptr->parameter_iter());
}

mal::Type::Ptr eval_ast(const mal::Type::Ptr ast, mal::Env &env) {
  // symbol
  auto symbol_ptr = dynamic_cast<const mal::Symbol *>(ast.get());
  if (symbol_ptr != nullptr) {
    // this is the whole point of using shared_ptr! Otherwise, unique_ptr is enough.
    return env.get(*symbol_ptr);
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

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::Env&& env) {
  return EVAL(ast, env);
}

std::string PRINT(mal::Type::Ptr input) { return pr_str(*input); }

std::string rep(const std::string& input, mal::Env& env) {
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
