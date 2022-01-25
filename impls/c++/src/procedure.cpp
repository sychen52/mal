#include "procedure.h"
#include "special_form.h"
#include "types.h"
#include <memory>

// forward declaration
mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::EnvFrame::WeakPtr env);



mal::Procedure::Procedure(const List::Ptr binds, const Type::Ptr ast,
                          const EnvFrame::Ptr env)
  : binds_(binds), ast_(ast), env_(env) {}

mal::Type::Ptr mal::Procedure::apply(ParameterIter& it) {
  // This is the ealiest place to check the number of exprs equals to number of
  // binds. I did this in Env constructor. Checking of the validity of binds are
  // during Functions constructor
  // binds_ can only by verified here when you consider optinal arguments and variatic arguments
  auto new_env = std::make_shared<EnvFrame>(env_, binds_, it);
  return EVAL(ast_, new_env);
}
