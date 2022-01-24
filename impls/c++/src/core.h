#pragma once
#include "env.h"
#include "types.h"
#include <memory>
#include <tuple>
#include <vector>

namespace mal {

  class Add : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "+";};
  };

  class Multiply : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "*";};
  };

  class Minus : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "-";};
  };

  class Divide : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "/";};
  };

  class Prn : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "prn";};
  };

  class ListFunction : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "list";};
  };

  class ListPredicate : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "list?";};
  };

  class EmptyPredicate : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "empty?";};
  };

  class Count : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "count";};
  };

  class Equal : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "=";};
  };

  class Less : public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "<";};
  };

  class LessEqual: public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "<=";};
  };

  class Larger: public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return ">";};
  };

  class LargerEqual: public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return ">=";};
  };

  class Eval: public Applicable {
  public:
    Eval(EnvFrame::Ptr env): env_(env) {};
    Type::Ptr apply(ParameterIter &) override;
    inline virtual std::string to_string() const override {return "eval";};
  private:
    EnvFrame::Ptr const env_;
  };

  EnvFrame::Ptr build_env();
  //
  //  //template<class T=void>
  //  //std::tuple<> get_arguments(const List::Iter &start, const List::Iter &end,
  //  //                           const std::string &fn) {
  //  //  if (start != end) {
  //  //    throw Exception(fn + " has extra argument.");
  //  //  }
  //  //  return std::make_tuple();
  //  //}
  //
  //  template<size_t n=0> // I cannot make it work without this size_t n=0;
  //  // get_arguments<Ts...> is calling get_arguments<> when Ts is empty.
  //  // However, I cannot define a template function get_arguments<>.
  //  auto get_arguments(const List::Iter &start, const List::Iter &end,
  //                             const std::string &fn) {
  //    if (start != end) {
  //      throw Exception(fn + " has extra argument.");
  //    }
  //    return std::make_tuple();
  //  }
  //
  //  template<class T, class... Ts, size_t n=0>
  //  auto get_arguments(const List::Iter &start, const List::Iter& end,
  //                     const std::string& fn) {
  //    if (start == end) {
  //      throw Exception(fn + " has not enough number of arguments.");
  //    }
  //    T* t = dynamic_cast<T*>((*start).get());
  //    if (t == nullptr) {
  //      throw Exception("Wrong argument type for " + fn + ": " + typeid(T).name());
  //    }
  //    return std::tuple_cat(std::make_tuple(t), get_arguments<Ts...>(start+1, end, fn));
  //  }

} // namespace mal
