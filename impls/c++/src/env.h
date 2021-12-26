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

    EnvFrame(const EnvFrame::Ptr outer = nullptr): outer_(outer) {}
    EnvFrame(const EnvFrame::Ptr outer, const std::vector<std::string> &binds,
        ParameterIter& it);

    Type::Ptr get(const Symbol &symbol) const;

    inline void set(const Symbol& key, Type::Ptr val) {data_[key.value()] = val;}

    const EnvFrame *find(const Symbol &key) const;

  private:
    std::unordered_map<std::string, Type::Ptr> data_;
    const EnvFrame::Ptr outer_;
  };

  // Procedure is the result of EVAL fn* (aka. lambda)
  // The difference between user defined fn* and buildin Applicable is:
  // An extra frame of Env is created when each time it is applied.
  class Procedure : public Applicable {
  public:
    Procedure(const Type::Ptr binds, const Type::Ptr ast, const EnvFrame::Ptr env);
    Type::Ptr apply(ParameterIter&) override;
    inline std::string to_string() const override { return "#<procedure>"; }
  private:
    std::vector<std::string> binds_;
    const Type::Ptr ast_;
    const EnvFrame::Ptr env_;
  };

} // namespace mal
