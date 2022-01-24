#include "env.h"
#include "types.h"
#include <unordered_map>
#include <string>

mal::EnvFrame::EnvFrame(const EnvFrame::Ptr outer, const List::Ptr &binds,
              ParameterIter& it)
  : outer_(outer) {

  std::vector<std::string> names;
  names.reserve(binds->size());
  for (size_t i = 0; i < binds->size(); ++i) {
    auto symbol = dynamic_cast<mal::Symbol *>((*binds)[i].get());
    if (symbol == nullptr) {
      throw mal::Exception(
          "fn*'s first argument must be list of symbols. This is not: " +
          (*binds)[i]->to_string());
    }
    names.push_back(symbol->value());
  }

  for (const auto &key : names) {
    data_[key] = it.pop();
  }
  it.no_extra();
}

mal::Type::Ptr mal::EnvFrame::get(const Symbol &symbol) const {
  auto env = find(symbol);
  if (env == nullptr) {
    return nullptr;
  }
  return env->data_.at(symbol.value());
}

const mal::EnvFrame* mal::EnvFrame::find(const Symbol &key) const {
  auto it = data_.find(key.value());
  if (it == data_.end()) {
    if (outer_ == nullptr) {
      return nullptr;
    }
    return outer_->find(key);
  }
  return this;
}
