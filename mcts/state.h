// MIT License
//
// Copyright (c) 2020 Lenzebo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
    [[nodiscard]] uint8_t getCurrentPlayer() const { return currentPlayer_; }
    void setCurrentPlayer(uint8_t player) { currentPlayer_ = player; }
    void increasePlayer(size_t max_player) { currentPlayer_ = (currentPlayer_ + 1) % max_player; }
    void print() const { asDerived().writeToStream(std::cout); };

    std::ostream& writeToStream(std::ostream& stream) const
    {
        stream << "Current: " << int(currentPlayer_) << "\n";
        return stream;
    }

  private:
    StateType& asDerived() { return static_cast<StateType&>(*this); }
    const StateType& asDerived() const { return static_cast<const StateType&>(*this); }

    uint8_t currentPlayer_ = 0;
};

}  // namespace mcts