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

void MoveGen::generateMoves(Board &board, MoveList &list)
{
	Bitboard *friendlyPieces = board.isWhiteTurn ? board.bitboards[0] : board.bitboards[1];
	for (uint8_t i = 0; i < 64; i++)
	{
		Bitboard bitPos = 1ULL << i;

		if ((bitPos & friendlyPieces[0]) != 0)
		{
			int currPiece = board.pieceArray[i];

			switch (currPiece & 7)
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
				for (int i = 0; i < 4; i++)
				{
					std::cout << (int)board.moveHistory[i].getFrom() << std::endl;
					std::cout << (int)board.moveHistory[i].getTo() << std::endl;
				}
				std::cout << (int)i << std::endl;
				throw std::exception();
			}
		}
	}
}

void MoveGen::pawnGen(Board &board, uint8_t startPos, Bitboard bitPos, MoveList &list)
{

	Bitboard opponentPieces = board.isWhiteTurn ? board.bitboards[1][0] : board.bitboards[0][0];

	if ((board.pieceArray[startPos] & Piece::COLOR_MASK) == Piece::WHITE)
	{
		if (startPos % 8 != 0 && ((bitPos << 7) & opponentPieces) != 0 && tryMove(board, Move(startPos, startPos + 7)))
		{
			if (startPos / 8 == 6)
			{
				list.add(Move(startPos, startPos + 7, Move::PROMOTION, Move::KNIGHT));
				list.add(Move(startPos, startPos + 7, Move::PROMOTION, Move::BISHOP));
				list.add(Move(startPos, startPos + 7, Move::PROMOTION, Move::ROOK));
				list.add(Move(startPos, startPos + 7, Move::PROMOTION, Move::QUEEN));
			}
			else
			{
				list.add(Move(startPos, startPos + 7));
			}
		}
		if (startPos % 8 != 7 && ((bitPos << 9) & opponentPieces) != 0 && tryMove(board, Move(startPos, startPos + 9)))
		{
			if (startPos / 8 == 6)
			{
				list.add(Move(startPos, startPos + 9, Move::PROMOTION, Move::KNIGHT));
				list.add(Move(startPos, startPos + 9, Move::PROMOTION, Move::BISHOP));
				list.add(Move(startPos, startPos + 9, Move::PROMOTION, Move::ROOK));
				list.add(Move(startPos, startPos + 9, Move::PROMOTION, Move::QUEEN));
			}
			else
			{
				list.add(Move(startPos, startPos + 9));
			}
		}
		if (((bitPos << 8) & board.allCombined) == 0 && tryMove(board, Move(startPos, startPos + 8)))
		{
			if (startPos / 8 == 6)
			{
				list.add(Move(startPos, startPos + 8, Move::PROMOTION, Move::KNIGHT));
				list.add(Move(startPos, startPos + 8, Move::PROMOTION, Move::BISHOP));
				list.add(Move(startPos, startPos + 8, Move::PROMOTION, Move::ROOK));
				list.add(Move(startPos, startPos + 8, Move::PROMOTION, Move::QUEEN));
			}
			else
			{
				list.add(Move(startPos, startPos + 8));
			}
		}
		if ((startPos / 8) == 1 && ((bitPos << 8) & board.allCombined) == 0 && (bitPos << 16 & board.allCombined) == 0 && tryMove(board, Move(startPos, startPos + 16)))
		{
			list.add(Move(startPos, startPos + 16));
		}
		if (startPos % 8 != 0 && board.passantSquare == startPos + 7 && tryMove(board, Move(startPos, startPos + 7, Move::PASSANT)))
		{
			list.add(Move(startPos, startPos + 7, Move::PASSANT));
		}
		if (startPos % 8 != 7 && board.passantSquare == startPos + 9 && tryMove(board, Move(startPos, startPos + 9, Move::PASSANT)))
		{
			list.add(Move(startPos, startPos + 9, Move::PASSANT));
		}
	}
	else
	{
		if (startPos % 8 != 7 && ((bitPos >> 7) & opponentPieces) != 0 && tryMove(board, Move(startPos, startPos - 7)))
		{
			if (startPos / 8 == 1)
			{
				list.add(Move(startPos, startPos - 7, Move::PROMOTION, Move::KNIGHT));
				list.add(Move(startPos, startPos - 7, Move::PROMOTION, Move::BISHOP));
				list.add(Move(startPos, startPos - 7, Move::PROMOTION, Move::ROOK));
				list.add(Move(startPos, startPos - 7, Move::PROMOTION, Move::QUEEN));
			}
			else
			{
				list.add(Move(startPos, startPos - 7));
			}
		}
		if (startPos % 8 != 0 && ((bitPos >> 9) & opponentPieces) != 0 && tryMove(board, Move(startPos, startPos - 9)))
		{

			if (startPos / 8 == 1)
			{
				list.add(Move(startPos, startPos - 9, Move::PROMOTION, Move::KNIGHT));
				list.add(Move(startPos, startPos - 9, Move::PROMOTION, Move::BISHOP));
				list.add(Move(startPos, startPos - 9, Move::PROMOTION, Move::ROOK));
				list.add(Move(startPos, startPos - 9, Move::PROMOTION, Move::QUEEN));
			}
			else
			{
				list.add(Move(startPos, startPos - 9));
			}
		}
		if (((bitPos >> 8) & board.allCombined) == 0 && tryMove(board, Move(startPos, startPos - 8)))
		{
			if (startPos / 8 == 1)
			{
				list.add(Move(startPos, startPos - 8, Move::PROMOTION, Move::KNIGHT));
				list.add(Move(startPos, startPos - 8, Move::PROMOTION, Move::BISHOP));
				list.add(Move(startPos, startPos - 8, Move::PROMOTION, Move::ROOK));
				list.add(Move(startPos, startPos - 8, Move::PROMOTION, Move::QUEEN));
			}
			else
			{
				list.add(Move(startPos, startPos - 8));
			}
		}
		if ((startPos / 8) == 6 && ((bitPos >> 8) & board.allCombined) == 0 && (bitPos >> 16 & board.allCombined) == 0 && tryMove(board, Move(startPos, startPos - 16)))
		{
			list.add(Move(startPos, startPos - 16));
		}
		if (startPos % 8 != 7 && board.passantSquare == startPos - 7 && tryMove(board, Move(startPos, startPos - 7, Move::PASSANT)))
		{
			list.add(Move(startPos, startPos - 7, Move::PASSANT));
		}
		if (startPos % 8 != 0 && board.passantSquare == startPos - 9 && tryMove(board, Move(startPos, startPos - 9, Move::PASSANT)))
		{
			list.add(Move(startPos, startPos - 9, Move::PASSANT));
		}
	}
}

void MoveGen::knightGen(Board &board, uint8_t startPos, MoveList &list)
{
	Bitboard friendlyPieces = board.isWhiteTurn ? board.bitboards[0][0] : board.bitboards[1][0];
	for (int i = 0; i < 8 && knightMoves[startPos][i] != 255; i++)
	{

		uint8_t endPos = knightMoves[startPos][i];
		if ((friendlyPieces & board.bitPositions[endPos]) == 0 && tryMove(board, Move(startPos, endPos)))
		{
			list.add(Move(startPos, endPos));
		}
	}
}

void MoveGen::bishopGen(Board &board, uint8_t startPos, Bitboard bitPos, MoveList &list)
{
	Bitboard friendlyPieces = board.isWhiteTurn ? board.bitboards[0][0] : board.bitboards[1][0];
	Bitboard opponentPieces = board.isWhiteTurn ? board.bitboards[1][0] : board.bitboards[0][0];
	for (uint8_t i = 4; i < 8; i++)
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
					list.add(Move(startPos, targetPos));
				}
				break;
			}
			else if ((board.allCombined & endBitPos) == 0 && tryMove(board, Move(startPos, targetPos)))
			{
				list.add(Move(startPos, targetPos));
				continue;
			}
		}
	}
}

void MoveGen::rookGen(Board &board, uint8_t startPos, Bitboard bitPos, MoveList &list)
{
	Bitboard friendlyPieces = board.isWhiteTurn ? board.bitboards[0][0] : board.bitboards[1][0];
	Bitboard opponentPieces = board.isWhiteTurn ? board.bitboards[1][0] : board.bitboards[0][0];
	for (uint8_t i = 0; i < 4; i++)
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
					list.add(Move(startPos, targetPos));
				}
				break;
			}
			else if ((board.allCombined & endBitPos) == 0 && tryMove(board, Move(startPos, targetPos)))
			{
				list.add(Move(startPos, targetPos));
				continue;
			}
		}
	}
}

void MoveGen::kingGen(Board &board, uint8_t startPos, Bitboard bitPos, MoveList &list)
{
	Bitboard friendlyPieces = board.isWhiteTurn ? board.bitboards[0][0] : board.bitboards[1][0];
	Bitboard opponentPieces = board.isWhiteTurn ? board.bitboards[1][0] : board.bitboards[0][0];

	for (int i = 0; i < 8 && kingMoves[startPos][i] != 255; i++)
	{
		uint8_t endPos = kingMoves[startPos][i];

		if ((friendlyPieces & board.bitPositions[endPos]) == 0 && tryMove(board, Move(startPos, endPos), endPos))
		{
			list.add(Move(startPos, endPos));
		}
	}

	// Handles castling
	if ((board.pieceArray[startPos] & Piece::COLOR_MASK) == Piece::WHITE)
	{
		if (board.whiteShortCastle &&
		    (board.allCombined & whiteShortEmpty) == 0 &&
		    !kingIsAttacked(board, 4) && !kingIsAttacked(board, 5) && !kingIsAttacked(board, 6) &&
		    tryMove(board, Move(4, 6, Move::CASTLING)))
		{
			list.add(Move(4, 6, Move::CASTLING));
		}

		if (board.whiteLongCastle &&
		    (board.allCombined & whiteLongEmpty) == 0 &&
		    !kingIsAttacked(board, 4) && !kingIsAttacked(board, 2) && !kingIsAttacked(board, 3) &&
		    tryMove(board, Move(4, 2, Move::CASTLING)))
		{
			list.add(Move(4, 2, Move::CASTLING));
		}
	}
	else
	{

		if (board.blackShortCastle &&
		    (board.allCombined & blackShortEmpty) == 0 &&
		    !kingIsAttacked(board, 60) && !kingIsAttacked(board, 61) && !kingIsAttacked(board, 62) && tryMove(board, Move(60, 62, Move::CASTLING)))
		{
			list.add(Move(60, 62, Move::CASTLING));
		}

		if (board.blackLongCastle &&
		    (board.allCombined & blackLongEmpty) == 0 &&
		    !kingIsAttacked(board, 60) && !kingIsAttacked(board, 59) && !kingIsAttacked(board, 58) && tryMove(board, Move(60, 58, Move::CASTLING)))
		{
			list.add(Move(60, 58, Move::CASTLING));
		}
	}
}

bool MoveGen::isAttacked(Board &board, uint8_t targetSquare)
{
	Bitboard *friendlyPieces = !board.isWhiteTurn ? board.bitboards[0] : board.bitboards[1];
	Bitboard *opponentPieces = !board.isWhiteTurn ? board.bitboards[1] : board.bitboards[0];

	// Bishops
	for (uint8_t i = 4; i < 8; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(targetSquare, directions[i]) + 1; magnitude++)
		{
			uint8_t shift = std::abs(directions[i] * magnitude);
			Bitboard endBitPos = i % 2 == 0 ? board.bitPositions[targetSquare] >> shift : board.bitPositions[targetSquare] << shift;

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
	for (uint8_t i = 0; i < 4; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(targetSquare, directions[i]) + 1; magnitude++)
		{
			uint8_t shift = std::abs(directions[i] * magnitude);

			Bitboard endBitPos = i % 2 == 0 ? board.bitPositions[targetSquare] >> shift : board.bitPositions[targetSquare] << shift;

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
		uint8_t endPos = knightMoves[targetSquare][i];
		if ((opponentPieces[Piece::KNIGHT] & board.bitPositions[endPos]) != 0)
		{
			return true;
		}
	}

	// Kings
	for (int i = 0; i < 8 && kingMoves[targetSquare][i] != 255; i++)
	{
		uint8_t endPos = kingMoves[targetSquare][i];
		if ((opponentPieces[Piece::KING] & board.bitPositions[endPos]) != 0)
		{
			return true;
		}
	}

	// Pawns
	Bitboard bitTargetSquare = board.bitPositions[targetSquare];
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

bool MoveGen::kingIsAttacked(Board &board, uint8_t targetSquare)
{
	Bitboard *friendlyPieces = board.isWhiteTurn ? board.bitboards[0] : board.bitboards[1];
	Bitboard *opponentPieces = board.isWhiteTurn ? board.bitboards[1] : board.bitboards[0];

	// Bishops
	for (uint8_t i = 4; i < 8; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(targetSquare, directions[i]) + 1; magnitude++)
		{
			uint8_t shift = std::abs(directions[i] * magnitude);
			Bitboard endBitPos = i % 2 == 0 ? board.bitPositions[targetSquare] >> shift : board.bitPositions[targetSquare] << shift;

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
	for (uint8_t i = 0; i < 4; i++)
	{
		for (int magnitude = 1; magnitude < disToEdge(targetSquare, directions[i]) + 1; magnitude++)
		{
			uint8_t shift = std::abs(directions[i] * magnitude);

			Bitboard endBitPos = i % 2 == 0 ? board.bitPositions[targetSquare] >> shift : board.bitPositions[targetSquare] << shift;

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
		uint8_t endPos = knightMoves[targetSquare][i];
		if ((opponentPieces[Piece::KNIGHT] & board.bitPositions[endPos]) != 0)
		{
			return true;
		}
	}

	// Kings
	for (int i = 0; i < 8 && kingMoves[targetSquare][i] != 255; i++)
	{
		uint8_t endPos = kingMoves[targetSquare][i];
		if ((opponentPieces[Piece::KING] & board.bitPositions[endPos]) != 0)
		{
			return true;
		}
	}

	// Pawns
	Bitboard bitTargetSquare = board.bitPositions[targetSquare];
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

bool MoveGen::tryMove(Board &board, Move testMove, uint8_t kingSquare)
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

int MoveGen::disToEdge(uint8_t startPos, int direction)
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
