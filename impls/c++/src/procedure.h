#include "env.h"

namespace mal {
  // Procedure is the result of EVAL fn* (aka. lambda)
  // The difference between user defined fn* and buildin Applicable is:
  // An extra frame of Env is created when each time it is applied.
  class Procedure : public Applicable {
  public:
    Procedure(const List::Ptr binds, const Type::Ptr ast, const EnvFrame::Ptr env);
    Type::Ptr apply(ParameterIter&) override;
    inline std::string to_string() const override { return "#<procedure>"; }
  private:
    const List::Ptr binds_;
    const Type::Ptr ast_;
    EnvFrame::Ptr env_;
  };
}
