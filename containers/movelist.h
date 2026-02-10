#pragma once

#include "move.h"
#include <iostream>
#include <assert.h>

struct MoveList
{
	Move moveList[256];
	int count = 0;

	Move &operator[](int i)
	{
		if (i < 0 || i >= count)
		{

			std::cerr << "Error: MoveList index " << i << " out of bounds (count: " << count << ")" << std::endl;
			return moveList[0];
		}
		return moveList[i];
	}

	const Move &operator[](int i) const
	{
		if (i < 0 || i >= count)
		{

			std::cerr << "Error: MoveList index " << i << " out of bounds (count: " << count << ")" << std::endl;
			return moveList[0];
		}
		return moveList[i];
	}

	void push_back(Move m)
	{
		moveList[count++] = m;
	}

	void erase(int i)
	{
		if (i < 0 || i >= count)
		{

			std::cerr << "Error: MoveList index " << i << " out of bounds (count: " << count << ")" << std::endl;
			return;
		}

		moveList[i] = moveList[count - 1];

		count--;
	}

	Move *begin() { return &moveList[0]; }
	Move *end() { return &moveList[count]; }

	const Move *begin() const { return &moveList[0]; }
	const Move *end() const { return &moveList[count]; }

	void clear() { count = 0; }
	void printMoveList();
};
