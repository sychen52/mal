#include "env.h"
#include "types.h"
#include "reader.h"
#include <fstream>
#include <memory>
#include <unordered_set>

bool is_form(const mal::List& list, const std::string &key);

mal::Type::Ptr read_string(const mal::String::Ptr string);

mal::Type::Ptr slurp(const mal::String::Ptr filename);

mal::Type::Ptr eval(mal::ParameterIter &&it, mal::EnvFrame::WeakPtr env);

class Variables {
public:
  using Set = std::unordered_set<std::string>;
  inline void insert_free(const mal::Symbol &s) { free_.insert(s.value()); }
  inline void insert_free(const Variables::Set& s) { free_.insert(s.begin(), s.end()); }
  inline void erase_free(const mal::Symbol &s) { free_.erase(s.value()); }
  inline void erase_free(const Variables::Set& s) { free_.insert(s.begin(), s.end()); }
  inline void insert_bound(const mal::Symbol &s) { bound_.insert(s.value()); }
  inline void insert_bound(const Variables::Set& s) { bound_.insert(s.begin(), s.end()); }
  inline void intersect_bound(const Variables::Set &s);
  inline void clear_bound() { bound_.clear(); }
  inline const auto& free() const { return free_; }
  inline const auto& bound() const { return bound_; }
  inline void sequential_merge(const Variables &other);

private:
  std::unordered_set<std::string> free_;
  std::unordered_set<std::string> bound_;
};

Variables get_variables(const mal::Type &ast);

/**
 * This series of class does not take ownership of env or ast
 */
class SpecialForm {
public:
  SpecialForm(mal::EnvFrame::WeakPtr env) : env_(env) {}
  virtual mal::Type::Ptr operator()() = 0;
  /**
   * This function will return free variables needed by the form and
   * bound variables created in the form.
   */
  virtual Variables variables() = 0;
protected:
  mal::EnvFrame::WeakPtr env_;
};

class DefBang : public SpecialForm {
public:
  DefBang(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;
  virtual Variables variables() override;

private:
  mal::Symbol::Ptr symbol_;
  mal::Type::Ptr ast_;
};

class LetStar : public SpecialForm {
public:
  LetStar(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;
  virtual Variables variables() override;

private:
  mal::Type::Ptr ast_;
  mal::List::Ptr binds_;
};

class Do : public SpecialForm {
public:
  Do(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;
  virtual Variables variables() override;

private:
  mal::ParameterIter it_;
};

class If : public SpecialForm {
public:
  If(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;
  virtual Variables variables() override;

private:
  mal::Type::Ptr condition_;
  mal::Type::Ptr ast_true_;
  mal::Type::Ptr ast_false_;
};

class FnStar : public SpecialForm {
public:
  FnStar(const mal::List &list, mal::EnvFrame::WeakPtr env);
  virtual mal::Type::Ptr operator()() override;
  virtual Variables variables() override;

private:
  mal::List::Ptr parameters_;
  mal::Type::Ptr body_;
};

std::unique_ptr<SpecialForm> build_special_form(const mal::List &list,
                                                mal::EnvFrame::WeakPtr env);
