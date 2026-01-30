#include "movegen.h"
#include "board.h"
#include <iostream>
#include <bit>
#include <iostream>
#include <chrono>

uint64_t MoveGen::perft(Board &board, int depth, bool isRoot)
{

	// Base case: if we reached the target depth, this leaf counts as 1 position
	if (depth == 0)
	{
		return 1;
	}

	MoveList list;
	generateMoves(board, list);
	uint64_t totalNodes = 0;

	for (int i = 0; i < list.count; ++i)
	{
		Move move = list.moveList[i];

		board.makeMove(move);

		uint64_t branchNodes = perft(board, depth - 1, false);
		totalNodes += branchNodes;
		board.unmakeMove();

		if (isRoot)
		{
			std::cout << board.moveToString(move) << " " << branchNodes << std::endl;
		}
	}
	return totalNodes;
}

void MoveGen::generateMoves(Board &board, MoveList &list, bool onlyCaptures)
{
	Bitboard *friendlyPieces = board.isWhiteTurn ? board.bitboards[0] : board.bitboards[1];
	for (int i = 0; i < 64; i++)
	{
		Bitboard bitPos = 1ULL << i;

		if ((bitPos & friendlyPieces[0]) != 0)
		{
			switch (board.getSquareType(i))
			{
			case Piece::PAWN:
				pawnGen(board, i, bitPos, list);
				break;
			case Piece::KNIGHT:
				knightGen(board, i, list);
				break;
			case Piece::BISHOP:
				bishopGen(board, i, bitPos, list);
				break;
			case Piece::ROOK:
				rookGen(board, i, bitPos, list);
				break;
			case Piece::QUEEN:
				bishopGen(board, i, bitPos, list);
				rookGen(board, i, bitPos, list);
				break;
			case Piece::KING:
				kingGen(board, i, bitPos, list);
				break;
			default:
				std::cout << "You have reached the default case. This is bad." << std::endl;
				board.printChessBoard();
				board.printMoveHistory();
				std::cout << (int)i << std::endl;
				throw std::exception();
			}
		}
	}

	if (onlyCaptures)
	{
		for (int i = list.count; i >= 0; i++)
		{
			int pieceCount = std::popcount(board.allCombined);
			board.makeMove(list[i]);
			pieceCount -= std::popcount(board.allCombined);
			board.unmakeMove();
			if (pieceCount != 0)
			{
				list.erase(i);
			}
		}
	}
}

void MoveGen::pawnGen(Board &board, int startPos, Bitboard bitPos, MoveList &list)
{

	Bitboard opponentPieces = board.isWhiteTurn ? board.bitboards[1][0] : board.bitboards[0][0];

	if (board.isSquareWhite(startPos))
	{
		// Possible promotion moves
		if (startPos % 8 != 0 && ((bitPos << 7) & opponentPieces) != 0 && tryMove(board, Move(startPos, startPos + 7)))
		{
			addPawnMove(startPos, startPos + 7, startPos / 8 == 6, list);
		}
		if (startPos % 8 != 7 && ((bitPos << 9) & opponentPieces) != 0 && tryMove(board, Move(startPos, startPos + 9)))
		{
			addPawnMove(startPos, startPos + 9, startPos / 8 == 6, list);
		}
		if (((bitPos << 8) & board.allCombined) == 0 && tryMove(board, Move(startPos, startPos + 8)))
		{
			addPawnMove(startPos, startPos + 8, startPos / 8 == 6, list);
		}
		// Promotion not possible
		if ((startPos / 8) == 1 && ((bitPos << 8) & board.allCombined) == 0 && (bitPos << 16 & board.allCombined) == 0 && tryMove(board, Move(startPos, startPos + 16)))
		{
			list.push_back(Move(startPos, startPos + 16));
		}
		if (startPos % 8 != 0 && board.passantSquare == startPos + 7 && tryMove(board, Move(startPos, startPos + 7, Move::PASSANT)))
		{
			list.push_back(Move(startPos, startPos + 7, Move::PASSANT));
		}
		if (startPos % 8 != 7 && board.passantSquare == startPos + 9 && tryMove(board, Move(startPos, startPos + 9, Move::PASSANT)))
		{
			list.push_back(Move(startPos, startPos + 9, Move::PASSANT));
		}
	}
	else
	{
		if (startPos % 8 != 7 && ((bitPos >> 7) & opponentPieces) != 0 && tryMove(board, Move(startPos, startPos - 7)))
		{
			addPawnMove(startPos, startPos - 7, startPos / 8 == 1, list);
		}
		if (startPos % 8 != 0 && ((bitPos >> 9) & opponentPieces) != 0 && tryMove(board, Move(startPos, startPos - 9)))
		{
			addPawnMove(startPos, startPos - 9, startPos / 8 == 1, list);
		}
		if (((bitPos >> 8) & board.allCombined) == 0 && tryMove(board, Move(startPos, startPos - 8)))
		{
			addPawnMove(startPos, startPos - 8, startPos / 8 == 1, list);
		}
		if ((startPos / 8) == 6 && ((bitPos >> 8) & board.allCombined) == 0 && (bitPos >> 16 & board.allCombined) == 0 && tryMove(board, Move(startPos, startPos - 16)))
		{
			list.push_back(Move(startPos, startPos - 16));
		}
		if (startPos % 8 != 7 && board.passantSquare == startPos - 7 && tryMove(board, Move(startPos, startPos - 7, Move::PASSANT)))
		{
			list.push_back(Move(startPos, startPos - 7, Move::PASSANT));
		}
		if (startPos % 8 != 0 && board.passantSquare == startPos - 9 && tryMove(board, Move(startPos, startPos - 9, Move::PASSANT)))
		{
			list.push_back(Move(startPos, startPos - 9, Move::PASSANT));
		}
	}
}

void MoveGen::knightGen(Board &board, int startPos, MoveList &list)
{
	Bitboard friendlyPieces = board.isWhiteTurn ? board.bitboards[0][0] : board.bitboards[1][0];
	for (int i = 0; i < 8 && knightMoves[startPos][i] != 255; i++)
	{

		int endPos = knightMoves[startPos][i];
		if ((friendlyPieces & (1ULL << endPos)) == 0 && tryMove(board, Move(startPos, endPos)))
		{
			list.push_back(Move(startPos, endPos));
		}
	}
}

void MoveGen::bishopGen(Board &board, int startPos, Bitboard bitPos, MoveList &list)
{
	Bitboard friendlyPieces = board.isWhiteTurn ? board.bitboards[0][0] : board.bitboards[1][0];
	Bitboard opponentPieces = board.isWhiteTurn ? board.bitboards[1][0] : board.bitboards[0][0];
	for (int i = 4; i < 8; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(startPos, directions[i]) + 1; magnitude++)
		{
			int targetPos = startPos + directions[i] * magnitude;

			int shift = directions[i] * magnitude;
			Bitboard endBitPos = i % 2 == 0 ? bitPos >> std::abs(shift) : bitPos << std::abs(shift);

			if ((friendlyPieces & endBitPos) != 0)
			{
				break;
			}
			else if ((opponentPieces & endBitPos) != 0)
			{
				if (tryMove(board, Move(startPos, targetPos)))
				{
					list.push_back(Move(startPos, targetPos));
				}
				break;
			}
			else if ((board.allCombined & endBitPos) == 0 && tryMove(board, Move(startPos, targetPos)))
			{
				list.push_back(Move(startPos, targetPos));
				continue;
			}
		}
	}
}

void MoveGen::rookGen(Board &board, int startPos, Bitboard bitPos, MoveList &list)
{
	Bitboard friendlyPieces = board.isWhiteTurn ? board.bitboards[0][0] : board.bitboards[1][0];
	Bitboard opponentPieces = board.isWhiteTurn ? board.bitboards[1][0] : board.bitboards[0][0];
	for (int i = 0; i < 4; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(startPos, directions[i]) + 1; magnitude++)
		{
			int targetPos = startPos + directions[i] * magnitude;

			int shift = directions[i] * magnitude;

			Bitboard endBitPos = i % 2 == 0 ? bitPos >> std::abs(shift) : bitPos << std::abs(shift);

			if ((friendlyPieces & endBitPos) != 0)
			{
				break;
			}
			else if ((opponentPieces & endBitPos) != 0)
			{
				if (tryMove(board, Move(startPos, targetPos)))
				{
					list.push_back(Move(startPos, targetPos));
				}
				break;
			}
			else if ((board.allCombined & endBitPos) == 0 && tryMove(board, Move(startPos, targetPos)))
			{
				list.push_back(Move(startPos, targetPos));
				continue;
			}
		}
	}
}

void MoveGen::kingGen(Board &board, int startPos, Bitboard bitPos, MoveList &list)
{
	Bitboard friendlyPieces = board.isWhiteTurn ? board.bitboards[0][0] : board.bitboards[1][0];
	Bitboard opponentPieces = board.isWhiteTurn ? board.bitboards[1][0] : board.bitboards[0][0];

	for (int i = 0; i < 8 && kingMoves[startPos][i] != 255; i++)
	{
		int endPos = kingMoves[startPos][i];

		if ((friendlyPieces & (1ULL << endPos)) == 0 && tryMove(board, Move(startPos, endPos), endPos))
		{
			list.push_back(Move(startPos, endPos));
		}
	}

	// Handles castling
	if (board.isSquareWhite(startPos))
	{
		if ((board.castlingRights & 0b1000) &&
		    (board.allCombined & whiteShortEmpty) == 0 &&
		    !kingIsAttacked(board, 4) && !kingIsAttacked(board, 5) && !kingIsAttacked(board, 6) &&
		    tryMove(board, Move(4, 6, Move::CASTLING)))
		{
			list.push_back(Move(4, 6, Move::CASTLING));
		}

		if ((board.castlingRights & 0b0100) &&
		    (board.allCombined & whiteLongEmpty) == 0 &&
		    !kingIsAttacked(board, 4) && !kingIsAttacked(board, 2) && !kingIsAttacked(board, 3) &&
		    tryMove(board, Move(4, 2, Move::CASTLING)))
		{
			list.push_back(Move(4, 2, Move::CASTLING));
		}
	}
	else
	{

		if ((board.castlingRights & 0b0010) &&
		    (board.allCombined & blackShortEmpty) == 0 &&
		    !kingIsAttacked(board, 60) && !kingIsAttacked(board, 61) && !kingIsAttacked(board, 62) && tryMove(board, Move(60, 62, Move::CASTLING)))
		{
			list.push_back(Move(60, 62, Move::CASTLING));
		}

		if ((board.castlingRights & 0b0001) &&
		    (board.allCombined & blackLongEmpty) == 0 &&
		    !kingIsAttacked(board, 60) && !kingIsAttacked(board, 59) && !kingIsAttacked(board, 58) && tryMove(board, Move(60, 58, Move::CASTLING)))
		{
			list.push_back(Move(60, 58, Move::CASTLING));
		}
	}
}

bool MoveGen::isAttacked(Board &board, int targetSquare)
{
	Bitboard *friendlyPieces = !board.isWhiteTurn ? board.bitboards[0] : board.bitboards[1];
	Bitboard *opponentPieces = !board.isWhiteTurn ? board.bitboards[1] : board.bitboards[0];

	// Bishops
	for (int i = 4; i < 8; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(targetSquare, directions[i]) + 1; magnitude++)
		{
			int shift = std::abs(directions[i] * magnitude);
			Bitboard endBitPos = i % 2 == 0 ? (1ULL << targetSquare) >> shift : (1ULL << targetSquare) << shift;

			if (((opponentPieces[Piece::BISHOP] & endBitPos) != 0 || (opponentPieces[Piece::QUEEN] & endBitPos) != 0))
			{
				return true;
			}
			if ((board.allCombined & endBitPos) != 0)
			{
				break;
			}
		}
	}

	// Rooks
	for (int i = 0; i < 4; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(targetSquare, directions[i]) + 1; magnitude++)
		{
			int shift = std::abs(directions[i] * magnitude);

			Bitboard endBitPos = i % 2 == 0 ? (1ULL << targetSquare) >> shift : (1ULL << targetSquare) << shift;

			if (((opponentPieces[Piece::ROOK] & endBitPos) != 0 || (opponentPieces[Piece::QUEEN] & endBitPos) != 0))
			{
				return true;
			}
			if ((board.allCombined & endBitPos) != 0)
			{
				break;
			}
		}
	}

	// Knights
	for (int i = 0; i < 8 && knightMoves[targetSquare][i] != 255; i++)
	{
		int endPos = knightMoves[targetSquare][i];
		if ((opponentPieces[Piece::KNIGHT] & (1ULL << endPos)) != 0)
		{
			return true;
		}
	}

	// Kings
	for (int i = 0; i < 8 && kingMoves[targetSquare][i] != 255; i++)
	{
		int endPos = kingMoves[targetSquare][i];
		if ((opponentPieces[Piece::KING] & (1ULL << endPos)) != 0)
		{
			return true;
		}
	}

	// Pawns
	Bitboard bitTargetSquare = (1ULL << targetSquare);
	if (!board.isWhiteTurn &&
	    (((opponentPieces[Piece::PAWN] & (bitTargetSquare << 7)) != 0 && (targetSquare % 8) != 0) ||
	     ((opponentPieces[Piece::PAWN] & (bitTargetSquare << 9)) != 0 && (targetSquare % 8) != 7)))
	{
		return true;
	}
	else if (board.isWhiteTurn &&
		 (((opponentPieces[Piece::PAWN] & (bitTargetSquare >> 7)) != 0 && (targetSquare % 8) != 7) ||
		  ((opponentPieces[Piece::PAWN] & (bitTargetSquare >> 9)) != 0 && (targetSquare % 8) != 0)))
	{
		return true;
	}

	return false;
}

bool MoveGen::kingIsAttacked(Board &board, int targetSquare)
{
	Bitboard *friendlyPieces = board.isWhiteTurn ? board.bitboards[0] : board.bitboards[1];
	Bitboard *opponentPieces = board.isWhiteTurn ? board.bitboards[1] : board.bitboards[0];

	// Bishops
	for (int i = 4; i < 8; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(targetSquare, directions[i]) + 1; magnitude++)
		{
			int shift = std::abs(directions[i] * magnitude);
			Bitboard endBitPos = i % 2 == 0 ? (1ULL << targetSquare) >> shift : (1ULL << targetSquare) << shift;

			if (((opponentPieces[Piece::BISHOP] & endBitPos) != 0 || (opponentPieces[Piece::QUEEN] & endBitPos) != 0))
			{
				return true;
			}
			if ((board.allCombined & endBitPos) != 0)
			{
				break;
			}
		}
	}

	// Rooks
	for (int i = 0; i < 4; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(targetSquare, directions[i]) + 1; magnitude++)
		{
			int shift = std::abs(directions[i] * magnitude);

			Bitboard endBitPos = i % 2 == 0 ? (1ULL << targetSquare) >> shift : (1ULL << targetSquare) << shift;

			if (((opponentPieces[Piece::ROOK] & endBitPos) != 0 || (opponentPieces[Piece::QUEEN] & endBitPos) != 0))
			{
				return true;
			}
			if ((board.allCombined & endBitPos) != 0)
			{
				break;
			}
		}
	}

	// Knights
	for (int i = 0; i < 8 && knightMoves[targetSquare][i] != 255; i++)
	{
		int endPos = knightMoves[targetSquare][i];
		if ((opponentPieces[Piece::KNIGHT] & (1ULL << endPos)) != 0)
		{
			return true;
		}
	}

	// Kings
	for (int i = 0; i < 8 && kingMoves[targetSquare][i] != 255; i++)
	{
		int endPos = kingMoves[targetSquare][i];
		if ((opponentPieces[Piece::KING] & (1ULL << endPos)) != 0)
		{
			return true;
		}
	}

	// Pawns
	Bitboard bitTargetSquare = (1ULL << targetSquare);
	if (board.isWhiteTurn && ((opponentPieces[Piece::PAWN] & (bitTargetSquare << 7)) != 0 || (opponentPieces[Piece::PAWN] & (bitTargetSquare << 9)) != 0))
	{
		return true;
	}
	else if (!board.isWhiteTurn && ((opponentPieces[Piece::PAWN] & (bitTargetSquare >> 7)) != 0 || (opponentPieces[Piece::PAWN] & (bitTargetSquare >> 9)) != 0))
	{
		return true;
	}

	return false;
}

bool MoveGen::tryMove(Board &board, Move testMove, int kingSquare)
{

	Bitboard friendlyKing;
	if (kingSquare == 255)
	{
		friendlyKing = board.isWhiteTurn ? board.bitboards[0][Piece::KING] : board.bitboards[1][Piece::KING];
	}
	else
	{
		friendlyKing = 1ULL << kingSquare;
	}

	board.makeMove(testMove);

	bool legal = !isAttacked(board, std::countr_zero(friendlyKing));

	board.unmakeMove();
	return legal;
}

int MoveGen::disToEdge(int startPos, int direction)
{
	int file = startPos % 8;
	int rank = startPos / 8;

	// 1. Check Cardinal Directions
	if (direction == -1)
		return file; // West
	if (direction == 1)
		return 7 - file; // East
	if (direction == -8)
		return rank; // South
	if (direction == 8)
		return 7 - rank; // North

	// 2. Check Diagonal Directions
	if (direction == -9)
		return std::min(rank, file); // South-West
	if (direction == 9)
		return std::min(7 - rank, 7 - file); // North-East
	if (direction == -7)
		return std::min(rank, 7 - file); // South-East
	if (direction == 7)
		return std::min(7 - rank, file); // North-West

	// 3. Invalid Direction Catch
	return -1;
}

void MoveGen::addPawnMove(int startPos, int endPos, bool isPromotion, MoveList &list)
{
	if (isPromotion)
	{
		list.push_back(Move(startPos, endPos, Move::PROMOTION, Move::KNIGHT));
		list.push_back(Move(startPos, endPos, Move::PROMOTION, Move::BISHOP));
		list.push_back(Move(startPos, endPos, Move::PROMOTION, Move::ROOK));
		list.push_back(Move(startPos, endPos, Move::PROMOTION, Move::QUEEN));
	}
	else
	{
		list.push_back(Move(startPos, endPos));
	}
}