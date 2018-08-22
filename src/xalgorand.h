// xalgorand.h: basic macros for Algorand project.

#pragma once
#include <boost/multiprecision/cpp_int.hpp>

// Note: it is recommended to use the following macros to access the namespace
//      in case we need to change the namespace's name.
#define IS_ALGORAND_INLINE false    // TODO: change this to true in the future
#define ALGORAND algorand::
#if IS_ALGORAND_INLINE
#define ALGORAND_BEGIN namespace algorand {
#else
#define ALGORAND_BEGIN inline namespace algorand {
#endif
#define ALGORAND_END }
#define ALGORAND_USING using namespace algorand

ALGORAND_BEGIN
// Alias for hashing
using uint256_t = typename boost::multiprecision::uint256_t;
ALGORAND_END


