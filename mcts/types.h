#pragma once
#include "utils/named_type.h"

#include <cstdint>

namespace mcts {
enum class StageType
{
    DECISION,
    CHANCE
};

using ActionId = NamedType<uint8_t, struct ActionTag>;
using ChanceEventId = NamedType<uint8_t, struct ChangeEventTag>;

}