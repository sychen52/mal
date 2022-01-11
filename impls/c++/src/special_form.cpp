#include "special_form.h"

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::EnvFrame::Ptr env);

mal::Type::Ptr slurp(const mal::String::Ptr filename) {
  std::string content;
  std::ifstream file(filename->value());
  if (file) {
    std::streampos filesize = file.tellg();
    content.reserve(filesize);
    file.seekg(0);
    while (!file.eof()) {
      content += file.get();
    }
  }
  return std::make_shared<mal::String>(content);
}

mal::Type::Ptr read_string(const mal::String::Ptr string) {
  return read_str(string->value());
}

mal::Type::Ptr fn_star(mal::ParameterIter &&it, mal::EnvFrame::Ptr env) {
  auto second = it.pop();
  auto third = it.pop();
  it.no_extra();
  return std::make_shared<mal::Procedure>(second, third, env);
}

mal::Type::Ptr if_form(mal::ParameterIter &&it, mal::EnvFrame::Ptr env) {
  auto second = it.pop();
  auto third = it.pop();
  mal::Type::Ptr fourth;
  if (it.is_done()) {
    fourth = std::make_shared<mal::Nil>();
  } else {
    fourth = it.pop();
    it.no_extra();
  }
  auto condiction = EVAL(second, env);
  if (dynamic_cast<mal::Nil *>(condiction.get()) != nullptr) {
    return EVAL(fourth, env);
  }
  if (auto f = dynamic_cast<mal::Boolean *>(condiction.get());
      f != nullptr && (!f->value())) {
    return EVAL(fourth, env);
  }
  return EVAL(third, env);
}

mal::Type::Ptr do_form(mal::ParameterIter &&it, mal::EnvFrame::Ptr env) {
  if (it.is_done()) {
    throw mal::Exception("do must have at least 1 argument.");
  }
  mal::Type::Ptr ret = nullptr;
  while (!it.is_done()) {
    ret = EVAL(it.pop(), env);
  }
  return ret;
}

mal::Type::Ptr let_star(mal::ParameterIter &&it, mal::EnvFrame::Ptr env) {
  auto second = it.pop<mal::List>();
  auto third = it.pop();
  it.no_extra();
  if (second->size() % 2 == 1) {
    throw mal::Exception(
                         "let*'s first argument must be list with even number of item");
  }
  auto new_env = std::make_shared<mal::EnvFrame>(env);
  for (size_t i = 0; i < second->size(); i += 2) {
    auto symbol = dynamic_cast<mal::Symbol *>((*second)[i].get());
    if (symbol == nullptr) {
      throw mal::Exception("the odd item in let* list must be symbol");
    }
    new_env->set(*symbol, EVAL((*second)[i + 1], new_env));
  }
  return EVAL(third, new_env);
}

mal::Type::Ptr def_bang(mal::ParameterIter &&it, mal::EnvFrame::Ptr env) {
  auto second = it.pop<mal::Symbol>();
  auto third = it.pop();
  auto ret = EVAL(third, env);
  it.no_extra();
  env->set(*second, ret);
  return ret;
}

bool is_form(const mal::List *list, const std::string &key) {
  auto first = dynamic_cast<mal::Symbol *>((*list)[0].get());
  if (first == nullptr || first->value() != key) {
    return false;
  }
  return true;
}
