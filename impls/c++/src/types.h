#pragma once
#include "exception.h"
#include <memory>
#include <string>
#include <vector>

#define EQUAL_WITH_VALUE(TOTYPE)                        \
  bool operator==(const Type& rhs) const override {     \
    auto other = dynamic_cast<const TOTYPE*>(&rhs);     \
    if (other == nullptr) {                             \
      return false;                                     \
    }                                                   \
    return value() == other->value();                   \
  }

#define EQUAL(TOTYPE)                                   \
  bool operator==(const Type& rhs) const override {     \
    auto other = dynamic_cast<const TOTYPE*>(&rhs);     \
    return other != nullptr;                            \
  }

namespace mal {
  class Type {
  public:
    using Ptr = std::shared_ptr<Type>;
    virtual std::string to_string() const = 0;
    virtual bool operator==(const Type& rhs) const = 0;
    inline virtual bool operator!=(const Type& rhs) const { return !this->operator==(rhs); };
  };

  template<class T>
  class TypeTemplate:public Type {
  public:
    TypeTemplate() {}
    TypeTemplate(const T& value):value_(value) {}
    inline const T& value() const {return value_;}
  protected:
    T value_;
  };

  class Number : public TypeTemplate<int64_t> {
  public:
    using Ptr = std::shared_ptr<Number>;
    using NumberType = int64_t;
    Number(const int64_t number) : TypeTemplate(number) {}
    inline virtual std::string to_string() const override {
      return std::to_string(value());
    }
    EQUAL_WITH_VALUE(Number)
  };

  class Symbol : public TypeTemplate<std::string> {
  public:
    using Ptr = std::shared_ptr<Symbol>;
    Symbol(const std::string &exp) : TypeTemplate(exp) {}
    virtual std::string to_string() const override { return value(); }
    EQUAL_WITH_VALUE(Symbol)
  };

  class String : public TypeTemplate<std::string> {
  public:
    String(const std::string &exp) : TypeTemplate(exp.substr(0, exp.size() - 2)) {}
    virtual std::string to_string() const override { return "\"" + value() + "\""; }
    EQUAL_WITH_VALUE(String)
  };

  class ParameterIter;

  class List : public TypeTemplate<std::vector<Type::Ptr>> {
  public:
    using Ptr = std::shared_ptr<List>;
    using Iter = std::vector<Type::Ptr>::const_iterator;
    inline void append(Type::Ptr exp) { value_.emplace_back(std::move(exp)); }
    virtual std::string to_string() const override;
    inline auto empty() const { return value_.empty(); }
    inline auto size() const { return value_.size(); }
    inline Type::Ptr operator[](const size_t i) const { return value_[i]; }
    // inline std::vector<Type::Ptr>::iterator begin() { return list_.begin(); }
    // inline std::vector<Type::Ptr>::iterator end() { return list_.end(); }
    inline Iter begin() const {return value_.begin();}
    inline Iter end() const {return value_.end();}
    ParameterIter parameter_iter() const;
    bool operator==(const Type &rhs) const override;
  };

  class ParameterIter {
  public:
    ParameterIter(const List::Iter &start, const List::Iter &end,
                         const std::string &fn);
    template<typename T>
    T* pop() {
      auto ptr = pop();
      T* t = dynamic_cast<T*>(ptr.get());
      if (t == nullptr) {
        throw Exception("Wrong argument type for " + fn_ + ": " + typeid(T).name());
      }
      return t;
    }
    Type::Ptr pop();
    void next();
    void no_extra() const;
    inline bool is_done() const { return it_ == end_; };

  private:
    size_t n_ = 0;
    List::Iter it_;
    const List::Iter end_;
    const std::string fn_;
  };

  class Boolean: public TypeTemplate<bool> {
    public:
    Boolean(const bool value): TypeTemplate(value) {}
    std::string to_string() const override{ return value()?"true":"false"; }
    EQUAL_WITH_VALUE(Boolean)
  };

  class Nil: public Type {
  public:
    Nil() {}
    std::string to_string() const override{ return "nil"; }
    EQUAL(Nil)
  };

  class Applicable : public Type {
  public:
    using Ptr = std::shared_ptr<Applicable>;
    virtual Type::Ptr apply(ParameterIter& it) = 0;
    virtual Type::Ptr apply(ParameterIter&& it) {return apply(it);};
    virtual std::string to_string() const override {
      return "this is an applicable";
    }
    inline bool operator==(const Type &) const override {
      throw mal::Exception("Callable cannot be compared");
      return false;
    }
  };
} // namespace mal
