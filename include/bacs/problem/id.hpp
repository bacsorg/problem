#pragma once

#include <bacs/problem/error.hpp>

#include <string>

namespace bacs {
namespace problem {

/// problem id
using id = std::string;

bool is_allowed_id(const id &id_);

struct invalid_id_error : virtual error {
  using id = boost::error_info<struct tag_id, problem::id>;
};

/// \throws invalid_id_error if !is_allowed_id(id_)
void validate_id(const id &id_);

}  // namespace problem
}  // namespace bacs
