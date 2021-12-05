#pragma once
#include "exception.h"
#include "types.h"
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>

namespace mal {

class BuiltinFunction : public Type {
public:
  using Ptr = std::shared_ptr<BuiltinFunction>;
  virtual Type::Ptr call(const std::vector<Type::Ptr>::const_iterator &start,
                         const std::vector<Type::Ptr>::const_iterator &end) = 0;
  virtual std::string to_string() const override {
    return "this is a function";
  }
};

class Add : public BuiltinFunction {
public:
  Type::Ptr call(const std::vector<Type::Ptr>::const_iterator &start,
                 const std::vector<Type::Ptr>::const_iterator &end) override {
    int64_t ret = 0;
    for (auto it = start; it != end; ++it) {
      auto ptr = dynamic_cast<Number *>((*it).get());
      if (ptr == nullptr) {
        throw Exception("All arguments are supposed be Number's");
      }
      ret += ptr->number;
    }
    return std::make_shared<Number>(ret);
  }
};

class Multiply : public BuiltinFunction {
public:
  Type::Ptr call(const std::vector<Type::Ptr>::const_iterator &start,
                 const std::vector<Type::Ptr>::const_iterator &end) override {
    int64_t ret = 1;
    for (auto it = start; it != end; ++it) {
      auto ptr = dynamic_cast<Number *>((*it).get());
      if (ptr == nullptr) {
        throw Exception("All arguments are supposed be Number's");
      }
      ret *= ptr->number;
    }
    return std::make_shared<Number>(ret);
  }
};

class Minus : public BuiltinFunction {
public:
  Type::Ptr call(const std::vector<Type::Ptr>::const_iterator &start,
                 const std::vector<Type::Ptr>::const_iterator &end) override {
    int64_t ret = 0;
    bool only_one_arg = false;
    if (start + 1 == end) {
      only_one_arg = true;
    }
    for (auto it = start; it != end; ++it) {
      auto ptr = dynamic_cast<Number *>((*it).get());
      if (ptr == nullptr) {
        throw Exception("All arguments are supposed be Number's");
      }
      if (it == start && !only_one_arg) {
        ret += ptr->number;
      } else {
        ret -= ptr->number;
      }
    }
    return std::make_shared<Number>(ret);
  }
};

class Divide : public BuiltinFunction {
public:
  Type::Ptr call(const std::vector<Type::Ptr>::const_iterator &start,
                 const std::vector<Type::Ptr>::const_iterator &end) override {
    int64_t ret = 1;
    bool only_one_arg = false;
    if (start + 1 == end) {
      only_one_arg = true;
    }
    for (auto it = start; it != end; ++it) {
      auto ptr = dynamic_cast<Number *>((*it).get());
      if (ptr == nullptr) {
        throw Exception("All arguments are supposed be Number's");
      }
      if (it == start && !only_one_arg) {
        ret *= ptr->number;
      } else {
        ret /= ptr->number;
      }
    }
    return std::make_shared<Number>(ret);
  }
};

class Env {
public:
  using Ptr = std::shared_ptr<Env>;

  Env(Env* parent = nullptr): parent_(parent) {}

  Type::Ptr get(const Symbol &symbol) const {
    auto env = find(symbol);
    if (env == nullptr) {
      throw Exception(symbol.name + " not found.");
    }
    return env->data_.at(symbol.name);
  }

  void set(const Symbol& key, Type::Ptr val) {
    data_[key.name] = val;
  }

  const Env* find(const Symbol& key) const{
    auto it = data_.find(key.name);
    if (it == data_.end()) {
      if (parent_ == nullptr) {
        return nullptr;
      }
      return parent_->find(key);
    }
    return this;
  }

private:
  std::unordered_map<std::string, Type::Ptr> data_;
  Env* parent_;
};
} // namespace mal
