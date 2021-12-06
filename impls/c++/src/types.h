#pragma once
#include <memory>
#include <string>
#include <vector>

namespace mal {
  class Type {
  public:
    using Ptr = std::shared_ptr<Type>;
    virtual std::string to_string() const = 0;
  };

  class Number : public Type {
  public:
    using Ptr = std::shared_ptr<Number>;
    Number(const int64_t number) : number(number) {}
    virtual std::string to_string() const override {
      return std::to_string(number);
    }
    int64_t number;
  };

  class Symbol : public Type {
  public:
    using Ptr = std::shared_ptr<Symbol>;
    Symbol(const std::string &exp) : name(exp) {}
    virtual std::string to_string() const override { return name; }
    std::string name;
  };

  class String : public Type {
  public:
    String(const std::string &exp) : str_(exp.substr(0, exp.size() - 2)) {}
    virtual std::string to_string() const override { return "\"" + str_ + "\""; }

  private:
    std::string str_;
  };

  class List : public Type {
  public:
    using Ptr = std::shared_ptr<List>;
    inline void append(Type::Ptr exp) { list_.emplace_back(std::move(exp)); }
    virtual std::string to_string() const override;
    inline auto empty() const { return list_.empty(); }
    inline auto size() const { return list_.size(); }
    inline Type::Ptr operator[](const size_t i) const { return list_[i]; }
    // inline std::vector<Type::Ptr>::iterator begin() { return list_.begin(); }
    // inline std::vector<Type::Ptr>::iterator end() { return list_.end(); }
    inline std::vector<Type::Ptr>::const_iterator begin() const {
      return list_.begin();
    }
    inline std::vector<Type::Ptr>::const_iterator end() const {
      return list_.end();
    }

  private:
    std::vector<Type::Ptr> list_;
  };

  class True: public Type {
  public:
    std::string to_string() const { return "true"; }
  };

  class False: public Type {
  public:
    std::string to_string() const { return "false"; }
  };

  class Nil: public Type {
  public:
    std::string to_string() const { return "nil"; }
  };

  class Function: public Type {
  public:
    std::string to_string() const { return "#<function>"; }
  };
} // namespace mal
