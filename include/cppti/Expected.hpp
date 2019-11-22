#ifndef CPPTI_EXPECTED
#define CPPTI_EXPECTED

#include <string>

#include <tl/expected.hpp>

namespace cppti
{
enum class ErrCode
{
  DB_FAILED_TO_OPEN,
  DB_INVALID_FORMAT
};

struct ErrContext
{
  ErrCode code;
  std::string what;
};

template <typename T>
using Expected = tl::expected<T, ErrContext>;
}

#endif /* !CPPTI_EXPECTED */
