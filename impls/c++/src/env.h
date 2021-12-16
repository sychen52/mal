#pragma once
#include "exception.h"
#include "types.h"
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mal {
  class Env {
  public:
    using Ptr = std::shared_ptr<Env>;

    Env(const Env* outer = nullptr): outer_(outer) {}
    Env(const Env *outer, const std::vector<std::string> &binds,
        ParameterIter& it);

    Type::Ptr get(const Symbol &symbol) const;

    inline void set(const Symbol& key, Type::Ptr val) {data_[key.value()] = val;}

    const Env *find(const Symbol &key) const;

  private:
    std::unordered_map<std::string, Type::Ptr> data_;
    const Env* outer_;
  };

  class Function : public Callable {
  public:
    Function(const Type::Ptr binds, const Type::Ptr ast, const Env *env);
    Type::Ptr call(ParameterIter&) override;
    inline std::string to_string() const override { return "#<function>"; }
  private:
    std::vector<std::string> binds_;
    const Type::Ptr ast_;
    const Env* env_;
  };

} // namespace mal
