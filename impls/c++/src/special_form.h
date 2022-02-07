#include "env.h"
#include "types.h"
#include "reader.h"
#include <memory>
#include <unordered_set>

bool is_form(const mal::List& list, const std::string &key);

mal::Type::Ptr eval(mal::ParameterIter &&it, mal::EnvFrame::WeakPtr env);

/**
 * This series of class does not take ownership of env or ast
 */
namespace mal {
class SpecialForm {
public:
  SpecialForm() = default;
  SpecialForm(mal::EnvFrame::WeakPtr env) : env_(env) {}
  virtual mal::Type::Ptr operator()() = 0;
  /**
   * This function will return free variables needed by the form and
   * bound variables created in the form.
   */
protected:
  mal::EnvFrame::WeakPtr env_;
};

class DefBang : public SpecialForm {
public:
  DefBang(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;

private:
  mal::Symbol::Ptr symbol_;
  mal::Type::Ptr ast_;
};

class LetStar : public SpecialForm {
public:
  LetStar(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;

private:
  mal::Type::Ptr ast_;
  mal::List::Ptr binds_;
};

class Do : public SpecialForm {
public:
  Do(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;

private:
  mal::ParameterIter it_;
};

class If : public SpecialForm {
public:
  If(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;

private:
  mal::Type::Ptr condition_;
  mal::Type::Ptr ast_true_;
  mal::Type::Ptr ast_false_;
};

class FnStar : public SpecialForm {
public:
  FnStar(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;

private:
  mal::List::Ptr parameters_;
  mal::Type::Ptr body_;
};

class Quote : public SpecialForm {
public:
  Quote(const mal::List &list);
  virtual mal::Type::Ptr operator()() override;

private:
  mal::Type::Ptr ast_;
};

class QuasiQuote : public SpecialForm {
public:
  QuasiQuote(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;

private:
  mal::Type::Ptr ast_;
};

class DefMacroBang : public SpecialForm {
public:
  DefMacroBang(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;

private:
  mal::Symbol::Ptr symbol_;
  mal::Type::Ptr ast_;
};

class MacroExpand : public SpecialForm {
public:
  MacroExpand(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;

private:
  mal::Type::Ptr ast_;
};
}

mal::Type::Ptr macroexpand(mal::Type::Ptr ast, mal::EnvFrame::WeakPtr env);
std::unique_ptr<mal::SpecialForm> build_special_form(const mal::List &list,
                                                mal::EnvFrame::WeakPtr env);
