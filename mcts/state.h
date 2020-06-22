#pragma once

#include <cstdint>
#include <iostream>

namespace mcts {
template <typename StateType>
class State
{
  public:
    /**
     * Returns the ID of the current player (i.e. the player that is next to act)
     */
    [[nodiscard]] uint8_t getCurrentPlayer() const { return current_player; }
    void setCurrentPlayer(uint8_t player) { current_player = player; }
    void increasePlayer(size_t max_player) { current_player = (current_player + 1) % max_player; }
    void print() const { asDerived().writeToStream(std::cout); };

    std::ostream& writeToStream(std::ostream& stream) const
    {
        stream << "Current: " << int(current_player) << "\n";
        return stream;
    }

  private:
    StateType& asDerived() { return static_cast<StateType&>(*this); }
    const StateType& asDerived() const { return static_cast<const StateType&>(*this); }

    uint8_t current_player = 0;
};

}