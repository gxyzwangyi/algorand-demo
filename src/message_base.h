// message_base.h: base class of block message.

#pragma once
#include "xalgorand.h"
#include <boost/multiprecision/cpp_int.hpp>

ALGORAND_BEGIN
// Concrete message classes should inherit this class.
class MessageBase {
public:
    ~MessageBase() = default;
    virtual uint256_t hash() const = 0;
};

inline uint256_t hash_value(const MessageBase &msg) {
    return msg.hash();
}
ALGORAND_END
