#include "env.h"
#include "printer.h"
#include "reader.h"
#include "types.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::Env &env);

mal::Type::Ptr def_bang(const mal::List *list, mal::Env& env) {
  auto first = dynamic_cast<mal::Symbol*>((*list)[0].get());
  mal::Symbol* second = nullptr;
  if (first == nullptr || first->name != "def!") {
    return nullptr;
  }
  if (list->size() != 3) {
    throw mal::Exception("def! must be a length 3 list. Got " +
                         std::to_string(list->size()));
  }
  second = dynamic_cast<mal::Symbol*>((*list)[1].get());
  if (second == nullptr) {
    throw mal::Exception("def!'s first argument must be symbol");
  }
  auto ret = EVAL((*list)[2], env);
  env.set(*second, ret);
  return ret;
}

mal::Type::Ptr let_star(const mal::List *list, mal::Env& env) {
  auto first = dynamic_cast<mal::Symbol*>((*list)[0].get());
  mal::List* second = nullptr;
  if (first == nullptr || first->name != "let*") {
    return nullptr;
  }
  if (list->size() != 3) {
    throw mal::Exception("let* must be a length 3 list. Got " +
                         std::to_string(list->size()));
  }
  second = dynamic_cast<mal::List*>((*list)[1].get());
  if (second == nullptr) {
    throw mal::Exception("let*'s first argument must be list");
  }
  if (second->size() % 2 == 1) {
    throw mal::Exception("let*'s first argument must be list with even number of item");
  }
  mal::Env new_env(&env);
  for (size_t i = 0; i < second->size(); i+=2) {
    auto symbol = dynamic_cast<mal::Symbol*>((*second)[i].get());
    std::cout << "!" << i << std::endl;
    if (symbol == nullptr) {
      throw mal::Exception("the odd item in let* list must be symbol");
    }
    new_env.set(*symbol, EVAL((*second)[i+1], new_env));
  }
  return EVAL((*list)[2], new_env);
}

mal::Type::Ptr apply(const mal::List *ret_list_ptr, mal::Env& env) {
  auto func_ptr = dynamic_cast<mal::BuiltinFunction *>((*ret_list_ptr)[0].get());
  if (func_ptr == nullptr) {
    throw mal::Exception("First element is not a function");
  }
  return func_ptr->call(ret_list_ptr->begin() + 1, ret_list_ptr->end());
}

mal::Type::Ptr eval_ast(const mal::Type::Ptr ast, mal::Env &env) {
  // symbol
  auto symbol_ptr = dynamic_cast<const mal::Symbol *>(ast.get());
  if (symbol_ptr != nullptr) {
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
  auto ret_def_bang = def_bang(list_ptr, env);
  if (ret_def_bang != nullptr) {
    return ret_def_bang;
  }

  // let*
  auto ret_let_star = let_star(list_ptr, env);
  if (ret_let_star != nullptr) {
    return ret_let_star;
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
  return apply(ret_list_ptr, env);
}

std::string PRINT(mal::Type::Ptr input) { return pr_str(*input); }

std::string rep(const std::string& input, mal::Env& env) {
  return PRINT(EVAL(READ(input), env));
}

int main() {
  std::string input;
  auto repl_env = mal::Env();
  repl_env.set(mal::Symbol("+"), std::make_shared<mal::Add>());
  repl_env.set(mal::Symbol("-"), std::make_shared<mal::Minus>());
  repl_env.set(mal::Symbol("*"), std::make_shared<mal::Multiply>());
  repl_env.set(mal::Symbol("/"), std::make_shared<mal::Divide>());

  std::string prompt = "user> ";
  std::cout << prompt;
  while (std::getline(std::cin, input)) {
    try {
      auto output = rep(input, repl_env);
      std::cout << output << std::endl;
    } catch (const mal::Exception &e) {
      std::cout << e.what() << std::endl;
    }
    std::cout << prompt;
  }
  return 0;
}
