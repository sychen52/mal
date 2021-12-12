#include "env.h"
#include <unordered_map>
#include <string>

// forward declaration
mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::Env&& env);


mal::Env::Env(const Env *outer, const std::vector<std::string> &binds,
              const std::vector<Type::Ptr>::const_iterator &start,
              const std::vector<Type::Ptr>::const_iterator &end)
    : outer_(outer) {
  auto it = start;
  for (const auto &key : binds) {
    if (it == end) {
      throw Exception("number of exprs less than number of binds");
    }
    data_[key] = *it;
    ++it;
  }
  if (it != end) {
    throw Exception("number of exprs more than number of binds");
  }
}

mal::Type::Ptr mal::Env::get(const Symbol &symbol) const {
  auto env = find(symbol);
  if (env == nullptr) {
    throw Exception(symbol.name + " not found.");
  }
  return env->data_.at(symbol.name);
}
const mal::Env *mal::Env::find(const Symbol &key) const {
  auto it = data_.find(key.name);
  if (it == data_.end()) {
    if (outer_ == nullptr) {
      return nullptr;
    }
    return outer_->find(key);
  }
  return this;
}

mal::Function::Function(const Type::Ptr binds, const Type::Ptr ast,
                        const Env *env)
    : ast_(ast), env_(env) {
  auto binds_ptr = dynamic_cast<mal::List *>(binds.get());
  if (binds_ptr == nullptr) {
    throw mal::Exception("fn*'s fire argument must be list.");
  }
  binds_.reserve(binds_ptr->size());
  for (size_t i = 0; i < binds_ptr->size(); ++i) {
    auto symbol = dynamic_cast<mal::Symbol *>((*binds_ptr)[i].get());
    if (symbol == nullptr) {
      throw mal::Exception(
          "fn*'s first argument must be list of symbols. This is not: " +
          (*binds_ptr)[i]->to_string());
    }
    binds_.push_back(symbol->name);
  }
}

mal::Type::Ptr
mal::Function::call(const std::vector<Type::Ptr>::const_iterator &start,
                    const std::vector<Type::Ptr>::const_iterator &end) {
  // This is the ealiest place to check the number of exprs equals to number of
  // binds. I did this in Env constructor. Checking of the validity of binds are
  // during Functions constructor
  return EVAL(ast_, Env(env_, binds_, start, end));
}
