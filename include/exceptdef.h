#pragma once

#include<stdexcept> // 使用了std定义的一些异常
#include<cassert> // 使用了断言功能

namespace esgstl {

#define ESGSTL_DEBUG(expr) \
  assert(expr)

#define ESGSTL_LENGTH_ERROR_IF(expr,what) \
  if ((expr)) throw std::length_error(what)

#define ESGSTL_OUT_OF_RANGE_IF(expr,what) \
  if ((expr)) throw std::out_of_range(what)

#define ESGSTL_RUNTIME_ERROR_IF(expr,what) \
  if ((expr)) throw std::runtime_error(what)

}