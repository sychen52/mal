#pragma once
#include "env.h"
#include "types.h"
#include <tuple>
#include <vector>

namespace mal {

  class Add : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "+";};
  };

  class Multiply : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "*";};
  };

  class Minus : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "-";};
  };

  class Divide : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "/";};
  };

  class Prn : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "prn";};
  };

  class ListFunction : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "list";};
  };

  class ListPredicate : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "list?";};
  };

  class EmptyPredicate : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "empty?";};
  };

  class Count : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "count";};
  };

  class Equal : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "=";};
  };

  class Less : public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "<";};
  };

  class LessEqual: public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "<=";};
  };

  class Larger: public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return ">";};
  };

  class LargerEqual: public Callable {
  public:
    Type::Ptr call(ParameterIter&) override;
    inline virtual std::string to_string() const override {return ">=";};
  };

  Env::Ptr build_env();
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
