#include "special_form.h"
#include "procedure.h"
#include "types.h"
#include <memory>

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::EnvFrame::WeakPtr env);

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

bool is_form(const mal::List &list, const std::string &key) {
  auto first = dynamic_cast<mal::Symbol *>(list[0].get());
  if (first == nullptr || first->value() != key) {
    return false;
  }
  return true;
}

DefBang::DefBang(const mal::List &list, mal::EnvFrame::WeakPtr env)
    : SpecialForm(env) {
  auto it = list.parameter_iter();
  symbol_ = it.pop<mal::Symbol>();
  ast_ = it.pop();
  it.no_extra();
}

mal::Type::Ptr DefBang::operator()() {
  auto ret = EVAL(ast_, env_);
  env_.lock()->set(*symbol_, ret);
  return ret;
}

LetStar::LetStar(const mal::List &list, mal::EnvFrame::WeakPtr env)
    : SpecialForm(env) {
  auto it = list.parameter_iter();
  binds_ = it.pop<mal::List>();
  ast_ = it.pop();
  it.no_extra();
  if (binds_->size() % 2 == 1) {
    throw mal::Exception("let*'s first argument must be list with even number of item");
  }
}

mal::Type::Ptr LetStar::operator()() {
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

Do::Do(const mal::List &list, mal::EnvFrame::WeakPtr env)
  : SpecialForm(env), it_(list.parameter_iter()) {
  if (it_.is_done()) {
    throw mal::Exception("do must have at least 1 argument.");
  }
}

mal::Type::Ptr Do::operator()() {
  mal::Type::Ptr ret = nullptr;
  while (!it_.is_done()) {
    ret = EVAL(it_.pop(), env_);
  }
  return ret;
}

If::If(const mal::List &list, mal::EnvFrame::WeakPtr env)
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

mal::Type::Ptr If::operator()() {
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

FnStar::FnStar(const mal::List &list, mal::EnvFrame::WeakPtr env)
  : SpecialForm(env) {
  auto it = list.parameter_iter();
  parameters_ = it.pop<mal::List>();
  body_ = it.pop();
  it.no_extra();
}

mal::Type::Ptr FnStar::operator()() {
  return std::make_shared<mal::Procedure>(parameters_, body_, env_.lock());
}
std::unique_ptr<SpecialForm> build_special_form(const mal::List &list,
                                                mal::EnvFrame::WeakPtr env) {
  // def!
  if (is_form(list, "def!")) {
    return std::make_unique<DefBang>(list, env);
  }
  // let*
  if (is_form(list, "let*")) {
    return std::make_unique<LetStar>(list, env);
  }
  // do
  if (is_form(list, "do")) {
    return std::make_unique<Do>(list, env);
  }
  // if
  if (is_form(list, "if")) {
    return std::make_unique<If>(list, env);
  }
  // fn*
  if (is_form(list, "fn*")) {
    return std::make_unique<FnStar>(list, env);
  }
  return nullptr;
}

Variables get_variables(const mal::Type &ast) {
  auto symbol = dynamic_cast<const mal::Symbol *>(&ast);
  if (symbol) {
    Variables ret;
    ret.insert_free(*symbol);
    return ret;
  }
  auto list = dynamic_cast<const mal::List *>(&ast);
  if (!list) {
    return Variables();
  }
  auto env = std::make_shared<mal::EnvFrame>();
  auto special_form = build_special_form(*list, env);
  if (special_form) {
    return special_form->variables();
  }
  // list but not special form
  // The only case left, is a function call
  Variables ret;
  for (auto it = list->begin(); it != list->end(); ++it) {
    ret.sequential_merge(get_variables(**it));
  }
  return ret;
}
Variables DefBang::variables() {
  Variables ret;
  ret.insert_bound(*symbol_);
  return ret;
}
Variables LetStar::variables() {
  auto ret = get_variables(*ast_);
  for (size_t i = 0; i < binds_->size(); i += 2) {
    auto symbol = dynamic_cast<mal::Symbol *>((*binds_)[i].get());
    if (symbol == nullptr) {
      throw mal::Exception("the odd item in let* list must be symbol");
    }
    ret.erase_free(*symbol);
  }
  ret.clear_bound(); // bound variables inside let is no longer bound.
  return ret;
}
Variables Do::variables() {
  Variables ret;
  while (!it_.is_done()) {
    ret.sequential_merge(get_variables(*it_.pop()));
  }
  return ret;
}
inline void Variables::intersect_bound(const Variables::Set &s) {
  auto it = bound_.begin();
  while (it != bound_.end()) {
    if (s.find(*it) == s.end()) {
      it = bound_.erase(it);
    } else {
      ++it;
    }
  }
}
Variables If::variables() {
  auto ret = get_variables(*condition_);
  auto branch_variables = get_variables(*ast_true_);
  auto false_variables = get_variables(*ast_false_);
  branch_variables.insert_free(false_variables.free());
  branch_variables.intersect_bound(false_variables.bound());
  ret.sequential_merge(branch_variables);
  return ret;
}
inline void Variables::sequential_merge(const Variables &other) {
  for (const auto &item : other.free()) {
    if (bound_.find(item) == bound_.end()) {
      free_.insert(item);
    }
  }
}
Variables FnStar::variables() {
  auto ret = get_variables(*body_);
  for (size_t i = 0; i < parameters_->size(); ++i ) {
    auto symbol = dynamic_cast<mal::Symbol *>((*parameters_)[i].get());
    if (symbol == nullptr) {
      throw mal::Exception("the parameters in fn* must be symbols");
    }
    ret.erase_free(*symbol);
  }
  return ret;
}
