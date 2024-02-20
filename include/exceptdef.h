#pragma once

#include<stdexcept> // ʹ����std�����һЩ�쳣
#include<cassert> // ʹ���˶��Թ���

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