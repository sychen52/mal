#include "core.h"
#include "exception.h"
#include "types.h"
#include "reader.h"
#include <iostream>
#include <memory>
#include <tuple>
#include <utility>
#include <fstream>
#include <sstream>

mal::Type::Ptr EVAL(const mal::Type::Ptr ast, mal::EnvFrame::WeakPtr env);

namespace mal {
Type::Ptr Add::apply(ParameterIter &it) {
  int64_t ret = 0;
  while (!it.is_done()) {
    auto ptr = it.pop<Number>();
    ret += ptr->value();
  }
  return std::make_shared<Number>(ret);
}

Type::Ptr Multiply::apply(ParameterIter &it) {
  int64_t ret = 1;
  while (!it.is_done()) {
    auto ptr = it.pop<Number>();
    ret *= ptr->value();
  }
  return std::make_shared<Number>(ret);
}

Type::Ptr Minus::apply(ParameterIter &it) {
  int64_t ret = 0;
  bool first_arg = true;
  while (!it.is_done()) {
    auto ptr = it.pop<Number>();
    if (first_arg && !it.is_done()) {
      ret += ptr->value();
    } else {
      ret -= ptr->value();
    }
    first_arg = false;
  }
  return std::make_shared<Number>(ret);
}

Type::Ptr Divide::apply(ParameterIter &it) {
  int64_t ret = 1;
  bool first_arg = true;
  while (!it.is_done()) {
    auto ptr = it.pop<Number>();
    if (first_arg && !it.is_done()) {
      ret *= ptr->value();
    } else {
      ret /= ptr->value();
    }
  }
  return std::make_shared<Number>(ret);
}

Type::Ptr Prn::apply(ParameterIter &it) {
  while (!it.is_done()) {
    std::cout << it.pop()->to_string() << std::endl;
  }
  return std::make_shared<Nil>();
}

Type::Ptr ListFunction::apply(ParameterIter &it) {
  auto ret = std::make_shared<List>();
  while (!it.is_done()) {
    ret->append(it.pop());
  }
  return ret;
}

Type::Ptr ListPredicate::apply(ParameterIter &it) {
  if (it.is_done()) {
    return std::make_shared<Boolean>(false);
  }
  auto second = it.pop();
  return std::make_shared<Boolean>(dynamic_cast<List *>(second.get()) !=
                                   nullptr);
}

Type::Ptr EmptyPredicate::apply(ParameterIter &it) {
  auto list = it.pop<List>();
  it.no_extra();
  return std::make_shared<Boolean>(list->size() == 0);
}

Type::Ptr Count::apply(ParameterIter &it) {
  auto item = it.pop();
  it.no_extra();
  if (auto list = dynamic_cast<List *>(item.get()); list != nullptr) {
    return std::make_shared<Number>(list->size());
  } else if (dynamic_cast<Nil *>(item.get()) != nullptr) {
    return std::make_shared<Number>(0);
  }
  throw Exception("Count only takes list or nil");
}

template <typename FuncType>
Type::Ptr compare_two_numbers(ParameterIter &it, const FuncType &func) {
  auto first = it.pop<Number>();
  auto second = it.pop<Number>();
  it.no_extra();
  return std::make_shared<Boolean>(func(first->value(), second->value()));
}

bool equal(const Type::Ptr &first, const Type::Ptr second) {
  return (*first) == (*second);
}

Type::Ptr Equal::apply(ParameterIter &it) {
  auto first = it.pop();
  auto second = it.pop();
  it.no_extra();
  return std::make_shared<Boolean>(equal(first, second));
}

Type::Ptr Less::apply(ParameterIter &it) {
  return compare_two_numbers(
      it, [](const Number::NumberType &lhs, const Number::NumberType &rhs) {
        return lhs < rhs;
      });
}

Type::Ptr LessEqual::apply(ParameterIter &it) {
  return compare_two_numbers(
      it, [](const Number::NumberType &lhs, const Number::NumberType &rhs) {
        return lhs <= rhs;
      });
}

Type::Ptr Larger::apply(ParameterIter &it) {
  return compare_two_numbers(
      it, [](const Number::NumberType &lhs, const Number::NumberType &rhs) {
        return lhs > rhs;
      });
}

Type::Ptr LargerEqual::apply(ParameterIter &it) {
  return compare_two_numbers(
      it, [](const Number::NumberType &lhs, const Number::NumberType &rhs) {
        return lhs >= rhs;
      });
}

Type::Ptr Str::apply(ParameterIter &it) {
  std::string ret;
  while (!it.is_done()) {
    ret += it.pop<String>()->value();
  }
  return std::make_shared<String>(ret);
}

Type::Ptr ReadString::apply(ParameterIter &it) {
  auto string = it.pop<String>();
  it.no_extra();
  return read_str(string->value());
}

Type::Ptr Slurp::apply(ParameterIter &it) {
  auto filename = it.pop<String>();
  it.no_extra();
  std::string content;
  std::ifstream file(filename->value());
  std::stringstream ret;
  ret << file.rdbuf();
  return std::make_shared<mal::String>(ret.str());
}

Type::Ptr Eval::apply(ParameterIter &it) {
  auto second = it.pop();
  it.no_extra();
  return EVAL(second, env_.lock());
};

Type::Ptr Cons::apply(ParameterIter &it) {
  auto exp = it.pop();
  auto list = it.pop<List>();
  it.no_extra();
  return list->cons(exp);
}

Type::Ptr Concat::apply(ParameterIter &it) {
  auto ret = std::make_shared<List>();
  while (!it.is_done()) {
    ret->insert(it.pop<List>());
  }
  return ret;
}

EnvFrame::Ptr build_env() {
  auto repl_env = std::make_shared<EnvFrame>();
  repl_env->add(std::make_shared<Add>());
  repl_env->add(std::make_shared<Minus>());
  repl_env->add(std::make_shared<Multiply>());
  repl_env->add(std::make_shared<Divide>());
  repl_env->add(std::make_shared<Prn>());
  repl_env->add(std::make_shared<ListFunction>());
  repl_env->add(std::make_shared<ListPredicate>());
  repl_env->add(std::make_shared<EmptyPredicate>());
  repl_env->add(std::make_shared<Count>());
  repl_env->add(std::make_shared<Equal>());
  repl_env->add(std::make_shared<Less>());
  repl_env->add(std::make_shared<LessEqual>());
  repl_env->add(std::make_shared<Larger>());
  repl_env->add(std::make_shared<LargerEqual>());
  repl_env->add(std::make_shared<Str>());
  repl_env->add(std::make_shared<ReadString>());
  repl_env->add(std::make_shared<Slurp>());
  repl_env->add(std::make_shared<Eval>(repl_env));
  repl_env->add(std::make_shared<Cons>());
  repl_env->add(std::make_shared<Concat>());
  return repl_env;
}
} // namespace mal
