#include "env.h"
#include "types.h"
#include "reader.h"
#include <fstream>

bool is_form(const mal::List *list, const std::string &key);

mal::Type::Ptr def_bang(mal::ParameterIter &&it, mal::EnvFrame::Ptr env);

mal::Type::Ptr let_star(mal::ParameterIter &&it, mal::EnvFrame::Ptr env);

mal::Type::Ptr do_form(mal::ParameterIter &&it, mal::EnvFrame::Ptr env);

mal::Type::Ptr if_form(mal::ParameterIter &&it, mal::EnvFrame::Ptr env);

mal::Type::Ptr fn_star(mal::ParameterIter &&it, mal::EnvFrame::Ptr env);

mal::Type::Ptr read_string(const mal::String::Ptr string);

mal::Type::Ptr slurp(const mal::String::Ptr filename);
