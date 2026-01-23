#pragma once

#include "move.h"
#include <assert.h> // For bounds checking in debug mode

struct MoveList
{
        Move moveList[256];
        int count = 0;

        // Overload [] for access and modification
        Move &operator[](int i)
        {
                assert(i >= 0 && i < count); // Optional: ensures you don't access uninitialized moves
                return moveList[i];
        }

        // Overload [] for read-only access
        const Move &operator[](int i) const
        {
                assert(i >= 0 && i < count);
                return moveList[i];
        }

        void push_back(Move m)
        {
                moveList[count++] = m;
        }

        void erase(int i)
        {
                assert(i >= 0 && i < count);
                // Replace current move with the one at the end of the list
                moveList[i] = moveList[count - 1];
                // Shrink the count
                count--;
        }

        Move *begin() { return &moveList[0]; }
        Move *end() { return &moveList[count]; }

        const Move *begin() const { return &moveList[0]; }
        const Move *end() const { return &moveList[count]; }

        void clear() { count = 0; }
        void printMoveList();
};