#include "search.h"

Move Search::findBestMove(Board &board, uint8_t depth)
{
        MoveList list;

        MoveGen::generateMoves(board, list);

        int bestDiff = -1000;
        int bestIndex = -1;

        for (int i = 0; i < list.count; i++)
        {
                int currDif = board.materialDifference(list.moveList[i]);

                if (currDif > bestDiff)
                {
                        bestDiff = currDif;
                        bestIndex = i;
                }
        }

        return list.moveList[bestIndex];
}