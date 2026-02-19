#include "board.h"
#include "../types/piece.h"
#include "../movegen/movegen.h"
#include <cstddef>
#include <iostream>
#include <iomanip>

void Board::makeMove(Move currMove)
{
	moveHistory.push_back(currMove);

	previousMoves.push_back({getSquareType(currMove.getTo()) | !isWhiteTurn * Piece::BLACK,
				 passantSquare,
				 castlingRights,
				 currMove.getType()});

	passantSquare = -1;

	Bitboard fromBit = 1ULL << currMove.getFrom();
	Bitboard toBit = 1ULL << currMove.getTo();
	int fromType = getSquareType(currMove.getFrom());
	int toType = getSquareType(currMove.getTo());

	if (fromType == Piece::PAWN)
	{
		pawnMove(fromType, toType, fromBit, toBit, currMove);
	}
	else if ((fromType == Piece::KING || fromType == Piece::ROOK) && (castlingRights & (isWhiteTurn ? 0b1100 : 0b0011)))
	{
		rookKingMove(fromType, toType, fromBit, toBit, currMove);
	}
	else
	{
		normalMove(fromType, toType, fromBit, toBit, currMove);
	}

	castlingRights &= toBit == (1ULL << 7) ? 0b0111 : 0b1111;
	castlingRights &= toBit == (1ULL << 0) ? 0b1011 : 0b1111;
	castlingRights &= toBit == (1ULL << 63) ? 0b1101 : 0b1111;
	castlingRights &= toBit == (1ULL << 56) ? 0b1110 : 0b1111;

	bitboards[0][7] = 0;
	bitboards[1][7] = 0;
	for (int t = Piece::PAWN; t <= Piece::KING; t++)
	{
		bitboards[0][7] |= bitboards[0][t];
		bitboards[1][7] |= bitboards[1][t];
	}
	allCombined = bitboards[0][7] | bitboards[1][7];

	isWhiteTurn = !isWhiteTurn;
	fullmoveCounter += (int)isWhiteTurn;
}

void Board::normalMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove)
{
	bitboards[!isWhiteTurn][fromType] ^= fromBit | toBit;

	bitboards[isWhiteTurn][toType] ^= toBit;
}

void Board::rookKingMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove)
{

	switch ((currMove.getFrom() << 4) | (isWhiteTurn << 3) | fromType)
	{
	// White turn (1 << 3)
	case (0 << 4) | (1 << 3) | Piece::ROOK:
		castlingRights &= 0b1011;
		break;
	case (7 << 4) | (1 << 3) | Piece::ROOK:
		castlingRights &= 0b0111;
		break;
	case (4 << 4) | (1 << 3) | Piece::KING:
		castlingRights &= 0b0011;
		break;

	// Black turn (0 << 3)
	case (56 << 4) | (0 << 3) | Piece::ROOK:
		castlingRights &= 0b1110;
		break;
	case (63 << 4) | (0 << 3) | Piece::ROOK:
		castlingRights &= 0b1101;
		break;
	case (60 << 4) | (0 << 3) | Piece::KING:
		castlingRights &= 0b1100;
		break;
	default:
		break;
	}

	if (!currMove.isCastling())
	{
		normalMove(fromType, toType, fromBit, toBit, currMove);
		return;
	}

	bitboards[!isWhiteTurn][Piece::KING] = 1ULL << currMove.getTo();

	bitboards[!isWhiteTurn][Piece::ROOK] ^= castleRookMasks[currMove.getTo()];
}

void Board::pawnMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove)
{
	if (currMove.isPassant())
	{
		bitboards[!isWhiteTurn][Piece::PAWN] ^= fromBit;

		bitboards[!isWhiteTurn][Piece::PAWN] |= toBit;

		bitboards[isWhiteTurn][Piece::PAWN] ^= 1ULL << ((currMove.getFrom() & ~0b111) + (currMove.getTo() & 0b111));
	}
	else if (currMove.isPromotion())
	{
		bitboards[!isWhiteTurn][Piece::PAWN] ^= fromBit;

		bitboards[!isWhiteTurn][(currMove.getPromotion() >> 14) + 2] ^= toBit;

		bitboards[isWhiteTurn][toType] ^= toBit;
	}
	else
	{

		if (std::abs((currMove.getTo() / 8) - (currMove.getFrom() / 8)) == 2)
		{
			passantSquare = isWhiteTurn ? currMove.getTo() - 8 : currMove.getTo() + 8;
		}

		normalMove(fromType, toType, fromBit, toBit, currMove);
	}
}

void Board::unmakeMove()
{
	isWhiteTurn = !isWhiteTurn;
	castlingRights = previousMoves.back().castlingRights;
	passantSquare = previousMoves.back().passantSquare;

	Move currMove = moveHistory.back();
	moveHistory.pop_back();

	Bitboard fromBit = 1ULL << currMove.getFrom();
	Bitboard toBit = 1ULL << currMove.getTo();

	if (!previousMoves.back().moveType)
	{
		bitboards[!isWhiteTurn][getSquareType(currMove.getTo())] ^= fromBit | toBit;

		bitboards[isWhiteTurn][previousMoves.back().capturedPiece & Piece::TYPE_MASK] ^= toBit;
	}
	else if (previousMoves.back().moveType == 1)
	{
		bitboards[!isWhiteTurn][Piece::KING] = 1ULL << currMove.getFrom();

		bitboards[!isWhiteTurn][Piece::ROOK] ^= castleRookMasks[currMove.getTo()];
	}
	else if (previousMoves.back().moveType == 2)
	{
		bitboards[!isWhiteTurn][Piece::PAWN] ^= fromBit | toBit;

		bitboards[isWhiteTurn][Piece::PAWN] ^= 1ULL << (currMove.getTo() + (isWhiteTurn ? -8 : 8));
	}
	else if (previousMoves.back().moveType == 3)
	{
		bitboards[!isWhiteTurn][Piece::PAWN] ^= fromBit;

		bitboards[!isWhiteTurn][getSquareType(currMove.getTo())] ^= toBit;

		bitboards[isWhiteTurn][previousMoves.back().capturedPiece & Piece::TYPE_MASK] ^= toBit;
	}

	bitboards[0][7] = 0;
	bitboards[1][7] = 0;
	for (int t = Piece::PAWN; t <= Piece::KING; t++)
	{
		bitboards[0][7] |= bitboards[0][t];
		bitboards[1][7] |= bitboards[1][t];
	}
	allCombined = bitboards[0][7] | bitboards[1][7];

	previousMoves.pop_back();
}

int Board::getSquareType(int index)
{
	if (bitboards[0][7] & (1ULL << index))
	{
		for (int i = Piece::PAWN; i <= Piece::KING; i++)
		{
			if (bitboards[0][i] & (1ULL << index))
			{
				return i;
			}
		}
		return -1;
	}

	for (int i = Piece::PAWN; i <= Piece::KING; i++)
	{
		if (bitboards[1][i] & (1ULL << index))
		{
			return i;
		}
	}
	return -1;
}

void Board::printChessBoard()
{
	for (int rank = 7; rank >= 0; rank--)
	{
		std::cout << rank + 1 << "  ";
		for (int file = 0; file < 8; file++)
		{

			int square = rank * 8 + file;
			Bitboard mask = 1ULL << square;
			char piece = '.';

			if (bitboards[0][1] & mask)
				piece = 'P';
			else if (bitboards[0][2] & mask)
				piece = 'N';
			else if (bitboards[0][3] & mask)
				piece = 'B';
			else if (bitboards[0][4] & mask)
				piece = 'R';
			else if (bitboards[0][5] & mask)
				piece = 'Q';
			else if (bitboards[0][6] & mask)
				piece = 'K';

			else if (bitboards[1][1] & mask)
				piece = 'p';
			else if (bitboards[1][2] & mask)
				piece = 'n';
			else if (bitboards[1][3] & mask)
				piece = 'b';
			else if (bitboards[1][4] & mask)
				piece = 'r';
			else if (bitboards[1][5] & mask)
				piece = 'q';
			else if (bitboards[1][6] & mask)
				piece = 'k';

			std::cout << piece << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n   a b c d e f g h\n";

	/* Prints out bitboards
	std::cout << (uint64_t)allCombined << std::endl;
	for (int i = 0; i <= Piece::KING; i++)
	{
		std::cout << (uint64_t)bitboards[0][i] << " - " << (int)i << std::endl;
	}
	for (int i = 0; i <= Piece::KING; i++)
	{
		std::cout << (uint64_t)bitboards[1][i] << " - " << (int)i << std::endl;
	}
	*/
}

Board::Board(std::string fenString)
{
	startingFen = fenString;

	moveHistory.reserve(10);

	for (int color = 0; color < 2; color++)
	{
		for (int type = 0; type < 8; type++)
		{
			bitboards[color][type] = 0;
		}
	}

	allCombined = 0;

	castlingRights = 0b0000;
	passantSquare = 0;
	halfmoveClock = 0;
	fullmoveCounter = 1;
	isWhiteTurn = true;

	int rank = 7;
	int file = 0;
	size_t i = 0;

	while (i < fenString.length() && fenString[i] != ' ')
	{

		char currChar = fenString[i];

		if (currChar == '/')
		{
			rank--;
			file = 0;
			i++;
			continue;
		}

		if (isdigit(currChar))
		{
			file += currChar - '0';
			i++;
			continue;
		}

		int currSquare = rank * 8 + file;
		uint64_t bit = 1ULL << currSquare;

		int color = islower(currChar) ? 1 : 0;

		int type;
		char lower = tolower(currChar);
		if (lower == 'p')
			type = Piece::PAWN;
		else if (lower == 'n')
			type = Piece::KNIGHT;
		else if (lower == 'b')
			type = Piece::BISHOP;
		else if (lower == 'r')
			type = Piece::ROOK;
		else if (lower == 'q')
			type = Piece::QUEEN;
		else if (lower == 'k')
			type = Piece::KING;

		bitboards[color][type] |= bit;

		file++;
		i++;
	}
	i++;

	for (int t = 1; t <= 6; t++)
	{
		bitboards[0][7] |= bitboards[0][t];
		bitboards[1][7] |= bitboards[1][t];
	}
	allCombined = bitboards[0][7] | bitboards[1][7];

	if (i < fenString.length())
	{
		if (fenString[i] == 'b')
		{
			isWhiteTurn = false;
		}
		i += 2;
	}

	if (i < fenString.length())
	{
		while (i < fenString.length() && fenString[i] != ' ')
		{
			switch (fenString[i])
			{
			case 'K':
				castlingRights |= 0b1000;
				break;
			case 'Q':
				castlingRights |= 0b0100;
				break;
			case 'k':
				castlingRights |= 0b0010;
				break;
			case 'q':
				castlingRights |= 0b0001;
				break;
			}
			i++;
		}
		i++;
	}

	if (i < fenString.length())
	{
		if (fenString[i] != '-')
		{
			file = fenString[i] - 'a';
			rank = fenString[i + 1] - '1';
			passantSquare = (int)(rank * 8 + file);
			i += 2;
		}
		else
		{
			i++;
		}

		if (i < fenString.length() && fenString[i] == ' ')
			i++;
	}

	if (i < fenString.length())
	{
		std::string halfmoveStr;
		while (i < fenString.length() && isdigit(fenString[i]))
		{
			halfmoveStr += fenString[i++];
		}
		if (!halfmoveStr.empty())
			halfmoveClock = std::stoi(halfmoveStr);
		else
			halfmoveClock = 0;

		if (i < fenString.length() && fenString[i] == ' ')
			i++;
	}

	if (i < fenString.length())
	{
		std::string fullmoveStr;
		while (i < fenString.length() && isdigit(fenString[i]))
		{
			fullmoveStr += fenString[i++];
		}
		if (!fullmoveStr.empty())
			fullmoveCounter = std::stoi(fullmoveStr);
		else
			fullmoveCounter = 1;
	}
}

Move Board::parseMove(std::string moveStr)
{

	int fromFile = moveStr[0] - 'a';
	int fromRank = moveStr[1] - '1';
	int toFile = moveStr[2] - 'a';
	int toRank = moveStr[3] - '1';

	int fromSq = fromRank * 8 + fromFile;
	int toSq = toRank * 8 + toFile;

	int flag = 0;
	int promotionPiece = 0;

	if (moveStr.length() == 5)
	{
		flag = Move::PROMOTION;
		char p = moveStr[4];
		if (p == 'n')
			promotionPiece = Move::KNIGHT;
		else if (p == 'b')
			promotionPiece = Move::BISHOP;
		else if (p == 'r')
			promotionPiece = Move::ROOK;
		else if (p == 'q')
			promotionPiece = Move::QUEEN;
	}

	int piece = getSquareType(fromSq);
	int targetPiece = getSquareType(toSq);

	int pieceType = piece & Piece::TYPE_MASK;

	if (pieceType == Piece::KING && std::abs(fromFile - toFile) == 2)
	{
		flag = Move::CASTLING;
	}

	if (pieceType == Piece::PAWN && (fromFile != toFile) && targetPiece == 0)
	{
		flag = Move::PASSANT;
	}

	return Move(fromSq, toSq, flag, promotionPiece);
}

std::string Board::moveToString(Move move)
{

	char fromFile = 'a' + (move.getFrom() % 8);
	char fromRank = '1' + (move.getFrom() / 8);
	char toFile = 'a' + (move.getTo() % 8);
	char toRank = '1' + (move.getTo() / 8);

	std::string moveStr = {fromFile, fromRank, toFile, toRank};

	if (move.isPromotion())
	{
		char promotionType = 'q';

		switch (move.getPromotion())
		{
		case Move::QUEEN:
			promotionType = 'q';
			break;
		case Move::ROOK:
			promotionType = 'r';
			break;
		case Move::BISHOP:
			promotionType = 'b';
			break;
		case Move::KNIGHT:
			promotionType = 'n';
			break;
		}

		moveStr += promotionType;
	}

	return moveStr;
}

bool Board::isKingAttacked(bool isWhiteKing)
{
	Bitboard friendlyKing = isWhiteTurn ? bitboards[0][Piece::KING] : bitboards[1][Piece::KING];

	return MoveGen::isAttacked(*this, std::countr_zero(friendlyKing));
}

void Board::printMoveHistory()
{
	if (moveHistory.empty())
	{
		std::cout << "Move history is empty." << '\n';
		return;
	}

	std::cout << "Move History (Replay " << moveHistory.size() << " moves):" << '\n';
	std::cout << std::string(50, '-') << '\n';

	Board replayBoard(startingFen);

	for (size_t i = 0; i < moveHistory.size(); i++)
	{
		const Move &move = moveHistory[i];

		int from = move.getFrom();
		int to = move.getTo();

		char fromFile = 'a' + (from % 8);
		char fromRank = '1' + (from / 8);
		char toFile = 'a' + (to % 8);
		char toRank = '1' + (to / 8);

		std::cout << std::setw(3) << i + 1 << ". "
			  << fromFile << fromRank << toFile << toRank;

		if (move.isPromotion())
		{
			std::cout << "=";
			int promotion = move.getPromotion();
			if (promotion == Move::KNIGHT)
				std::cout << "N";
			else if (promotion == Move::BISHOP)
				std::cout << "B";
			else if (promotion == Move::ROOK)
				std::cout << "R";
			else if (promotion == Move::QUEEN)
				std::cout << "Q";
			std::cout << " (promotion)";
		}
		else if (move.isCastling())
		{
			std::cout << " (castling)";
		}
		else if (move.isPassant())
		{
			std::cout << " (en passant)";
		}

		replayBoard.makeMove(move);

		std::cout << '\n';
	}

	std::cout << std::string(50, '-') << '\n';
}