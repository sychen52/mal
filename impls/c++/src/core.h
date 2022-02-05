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

  class Str: public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "str";};
  };

  class ReadString: public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "read-string";};
  };

  class Slurp: public Applicable {
  public:
    Type::Ptr apply(ParameterIter&) override;
    inline virtual std::string to_string() const override {return "slurp";};
  };

  class Eval: public Applicable {
  public:
    Eval(EnvFrame::WeakPtr env): env_(env) {};
    Type::Ptr apply(ParameterIter &) override;
    inline virtual std::string to_string() const override {return "eval";};
  private:
    EnvFrame::WeakPtr env_;
  };

  class Cons: public Applicable {
  public:
    Type::Ptr apply(ParameterIter &) override;
    inline virtual std::string to_string() const override {return "cons";};
  };

  class Concat: public Applicable {
  public:
    Type::Ptr apply(ParameterIter &) override;
    inline virtual std::string to_string() const override {return "concat";};
  };

  EnvFrame::Ptr build_env();
} // namespace mal
