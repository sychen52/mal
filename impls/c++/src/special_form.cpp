#include "special_form.h"
#include "exception.h"
#include "procedure.h"
#include "types.h"
#include <memory>
#include <string>

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::EnvFrame::WeakPtr env);

bool is_form(const mal::List &list, const std::string &key) {
  auto first = dynamic_cast<mal::Symbol *>(list[0].get());
  if (first == nullptr || first->value() != key) {
    return false;
  }
  return true;
}

mal::DefBang::DefBang(const mal::List &list, mal::EnvFrame::WeakPtr env)
    : SpecialForm(env) {
  auto it = list.parameter_iter();
  symbol_ = it.pop<mal::Symbol>();
  ast_ = it.pop();
  it.no_extra();
}

mal::Type::Ptr mal::DefBang::operator()() {
  auto ret = EVAL(ast_, env_);
  env_.lock()->set(*symbol_, ret);
  return ret;
}

mal::LetStar::LetStar(const mal::List &list, mal::EnvFrame::WeakPtr env)
    : SpecialForm(env) {
  auto it = list.parameter_iter();
  binds_ = it.pop<mal::List>();
  ast_ = it.pop();
  it.no_extra();
  if (binds_->size() % 2 == 1) {
    throw mal::Exception("let*'s first argument must be list with even number of item");
  }
}

mal::Type::Ptr mal::LetStar::operator()() {
  auto new_env_ = std::make_shared<mal::EnvFrame>(env_.lock());
  for (size_t i = 0; i < binds_->size(); i += 2) {
    auto symbol = dynamic_cast<mal::Symbol *>((*binds_)[i].get());
    if (symbol == nullptr) {
      throw mal::Exception("the odd item in let* list must be symbol");
    }
    new_env_->set(*symbol, EVAL((*binds_)[i + 1], new_env_));
  }
  return EVAL(ast_, new_env_);
}

mal::Do::Do(const mal::List &list, mal::EnvFrame::WeakPtr env)
  : SpecialForm(env), it_(list.parameter_iter()) {
  if (it_.is_done()) {
    throw mal::Exception("do must have at least 1 argument.");
  }
}

mal::Type::Ptr mal::Do::operator()() {
  mal::Type::Ptr ret = nullptr;
  while (!it_.is_done()) {
    ret = EVAL(it_.pop(), env_);
  }
  return ret;
}

mal::If::If(const mal::List &list, mal::EnvFrame::WeakPtr env)
  : SpecialForm(env) {
  auto it = list.parameter_iter();
  condition_ = it.pop();
  ast_true_ = it.pop();
  if (it.is_done()) {
    ast_false_ = std::make_shared<mal::Nil>();
  } else {
    ast_false_ = it.pop();
    it.no_extra();
  }
}

mal::Type::Ptr mal::If::operator()() {
  auto condiction = EVAL(condition_, env_);
  if (dynamic_cast<mal::Nil *>(condiction.get()) != nullptr) {
    return EVAL(ast_false_, env_);
  }
  if (auto f = dynamic_cast<mal::Boolean *>(condiction.get());
      f != nullptr && (!f->value())) {
    return EVAL(ast_false_, env_);
  }
  return EVAL(ast_true_, env_);
}

mal::FnStar::FnStar(const mal::List &list, mal::EnvFrame::WeakPtr env)
  : SpecialForm(env) {
  auto it = list.parameter_iter();
  parameters_ = it.pop<mal::List>();
  body_ = it.pop();
  it.no_extra();
}

mal::Type::Ptr mal::FnStar::operator()() {
  return std::make_shared<mal::Procedure>(parameters_, body_, env_.lock());
}

mal::Quote::Quote(const mal::List &list) {
  auto it = list.parameter_iter();
  ast_ = it.pop();
  it.no_extra();
}

mal::Type::Ptr mal::Quote::operator()() {
  return ast_;
}

mal::Type::Ptr quasiquote(const mal::Type::Ptr& ast) {
  auto list = dynamic_cast<mal::List*>(ast.get());
  if (list != nullptr) {
    // unquote
    if((!list->empty()) && ((*((*list)[0])) == mal::Symbol("unquote"))) {
      if (list->size() != 2) {
        throw mal::Exception("unquote can only take 1 element, given " +
                             std::to_string(list->size()-1));
      }
      return (*list)[1];
    }
    // normal list
    auto ret = std::make_shared<mal::List>();
    for (size_t i = 0; i < list->size(); ++i) {
      const auto& elt = (*list)[list->size()-i-1];
      auto elt_list = dynamic_cast<mal::List *>(elt.get());
      if (elt_list != nullptr && (!elt_list->empty()) &&
          ((*((*elt_list)[0])) == mal::Symbol("splice-unquote"))) {
        if (elt_list->size() != 2) {
          throw mal::Exception("splice-unquote can only take 1 element, given " +
                               std::to_string(elt_list->size() - 1));
        }
        ret = std::make_shared<mal::List>(std::vector<mal::Type::Ptr>{
            std::make_shared<mal::Symbol>("concat"), (*elt_list)[1], ret});
      }
      else {
        ret = std::make_shared<mal::List>(std::vector<mal::Type::Ptr>{
            std::make_shared<mal::Symbol>("cons"), quasiquote(elt), ret});
      }
    }
    return ret;
  }
  // symbol or map
  // map is skipped for now
  auto symbol = dynamic_cast<mal::Symbol*>(ast.get());
  if (symbol != nullptr) {
    return std::make_shared<mal::List>(std::vector<mal::Type::Ptr>{
        std::make_shared<mal::Symbol>("quote"), ast});
  }
  // default
  return ast;
}

mal::QuasiQuote::QuasiQuote(const mal::List &list, mal::EnvFrame::WeakPtr env)
  : SpecialForm(env) {
  auto it = list.parameter_iter();
  ast_ = it.pop();
  it.no_extra();
}

mal::Type::Ptr mal::QuasiQuote::operator()() {
  //return quasiquote(ast_);
  return EVAL(quasiquote(ast_), env_);
}

std::unique_ptr<mal::SpecialForm>
mal::build_special_form(const mal::List &list, mal::EnvFrame::WeakPtr env) {
  // def!
  if (is_form(list, "def!")) {
    return std::make_unique<mal::DefBang>(list, env);
  }
  // let*
  if (is_form(list, "let*")) {
    return std::make_unique<mal::LetStar>(list, env);
  }
  // do
  if (is_form(list, "do")) {
    return std::make_unique<mal::Do>(list, env);
  }
  // if
  if (is_form(list, "if")) {
    return std::make_unique<mal::If>(list, env);
  }
  // fn*
  if (is_form(list, "fn*")) {
    return std::make_unique<mal::FnStar>(list, env);
  }
  // quote
  if (is_form(list, "quote")) {
    return std::make_unique<mal::Quote>(list);
  }
  // quasiquote
  if (is_form(list, "quasiquote")) {
    return std::make_unique<mal::QuasiQuote>(list, env);
  }
  return nullptr;
}
