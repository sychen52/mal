#pragma once
#include "types.h"
#include "env.h"
#include <vector>

namespace mal {

  class Add : public Callable {
  public:
    Type::Ptr call(const std::vector<Type::Ptr>::const_iterator &start,
                   const std::vector<Type::Ptr>::const_iterator &end) override;
  };

  class Multiply : public Callable {
  public:
    Type::Ptr call(const std::vector<Type::Ptr>::const_iterator &start,
                   const std::vector<Type::Ptr>::const_iterator &end) override;
  };

  class Minus : public Callable {
  public:
    Type::Ptr call(const std::vector<Type::Ptr>::const_iterator &start,
                   const std::vector<Type::Ptr>::const_iterator &end) override;
  };

  class Divide : public Callable {
  public:
    Type::Ptr call(const std::vector<Type::Ptr>::const_iterator &start,
                   const std::vector<Type::Ptr>::const_iterator &end) override;
  };

  Env build_env();
}
