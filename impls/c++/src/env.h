#pragma once
#include "exception.h"
#include "types.h"
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mal {
  class EnvFrame {
  public:
    using Ptr = std::shared_ptr<EnvFrame>;
    using WeakPtr = std::weak_ptr<EnvFrame>;
    EnvFrame(const EnvFrame::Ptr outer = nullptr): outer_(outer) {}
    EnvFrame(const EnvFrame::Ptr outer, const List::Ptr &binds,
        ParameterIter& it);

    Type::Ptr get(const Symbol &symbol) const;

    inline void set(const Symbol& key, Type::Ptr val) {data_[key.value()] = val;}

    const EnvFrame* find(const Symbol &key) const;

  private:
    std::unordered_map<std::string, Type::Ptr> data_;
    const EnvFrame::Ptr outer_;
  };
} // namespace mal
