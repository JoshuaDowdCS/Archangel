#include "board.h"
#include "piece.h"
#include "movegen.h"
#include <cstddef>
#include <iostream>
#include <iomanip>

void Board::makeMove(Move currMove)
{
	moveHistory.push_back(currMove);
	previousMoves.push_back({-1, -1, -1, -1});
	passantSquare = -1;

	Bitboard fromBit = 1ULL << currMove.getFrom();
	Bitboard toBit = 1ULL << currMove.getTo();
	int fromType = getSquareType(currMove.getFrom());
	int toType = getSquareType(currMove.getTo());

	previousMoves.back().capturedPiece = getSquareType(currMove.getTo()) | (isSquareWhite(currMove.getTo()) ? Piece::WHITE : Piece::BLACK);

	if ((fromType == Piece::ROOK || fromType == Piece::KING))
	{
		if (isWhiteTurn && (castlingRights & 0b1100))
		{
			rookKingMove(fromType, toType, fromBit, toBit, currMove);
		}
		else if (!isWhiteTurn && (castlingRights & 0b0011))
		{
			rookKingMove(fromType, toType, fromBit, toBit, currMove);
		}
		else
		{
			normalMove(fromType, toType, fromBit, toBit, currMove);
		}
	}
	else if (fromType == Piece::KNIGHT || fromType == Piece::BISHOP || fromType == Piece::QUEEN)
	{
		normalMove(fromType, toType, fromBit, toBit, currMove);
	}
	else
	{
		pawnMove(fromType, toType, fromBit, toBit, currMove);
	}

	castlingRights &= toBit == (1ULL << 7) ? 0b0111 : 0b1111;
	castlingRights &= toBit == (1ULL << 0) ? 0b1011 : 0b1111;
	castlingRights &= toBit == (1ULL << 63) ? 0b1101 : 0b1111;
	castlingRights &= toBit == (1ULL << 56) ? 0b1110 : 0b1111;

	bitboards[0][0] = 0;
	bitboards[1][0] = 0;
	for (int t = 1; t <= 6; t++)
	{
		bitboards[0][0] |= bitboards[0][t]; // White combined
		bitboards[1][0] |= bitboards[1][t]; // Black combined
	}
	allCombined = bitboards[0][0] | bitboards[1][0];

	previousMoves.back().castlingRights = castlingRights;
	previousMoves.back().passantSquare = passantSquare;

	isWhiteTurn = !isWhiteTurn;
	fullmoveCounter += (int)isWhiteTurn;
}

void Board::normalMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove)
{
	previousMoves.back().moveType = 0;

	Bitboard *friendlyPieces = isWhiteTurn ? bitboards[0] : bitboards[1];
	Bitboard *opponentPieces = isWhiteTurn ? bitboards[1] : bitboards[0];
	// Updates friendly piece and combined Bitboards
	friendlyPieces[fromType] ^= fromBit;

	friendlyPieces[fromType] ^= toBit;

	// Updates opponents piece and combined Bitboards if there is a capture
	if (toType)
	{
		opponentPieces[toType] ^= toBit;
	}
}

void Board::rookKingMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove)
{
	Bitboard *friendlyPieces = isWhiteTurn ? bitboards[0] : bitboards[1];

	if (isSquareWhite(currMove.getFrom()))
	{
		if (fromType == Piece::KING)
		{
			castlingRights &= 0b0011;
		}
		else if (currMove.getFrom() == 0 && Piece::ROOK)
		{
			castlingRights &= 0b1011;
		}
		else if (currMove.getFrom() == 7 && Piece::ROOK)
		{
			castlingRights &= 0b0111;
		}
	}
	else
	{
		if (fromType == Piece::KING)
		{
			castlingRights &= 0b1100;
		}
		else if (currMove.getFrom() == 56 && Piece::ROOK)
		{
			castlingRights &= 0b1110;
		}
		else if (currMove.getFrom() == 63 && Piece::ROOK)
		{
			castlingRights &= 0b1101;
		}
	}

	if (currMove.isCastling())
	{
		previousMoves.back().moveType = 2;
		if (isSquareWhite(currMove.getFrom()))
		{
			friendlyPieces[Piece::KING] = 0;
			if (currMove.getTo() == 2)
			{
				friendlyPieces[Piece::KING] = 1ULL << 2;

				friendlyPieces[Piece::ROOK] |= 1ULL << 3;

				friendlyPieces[Piece::ROOK] ^= 1ULL << 0;
			}
			else
			{
				friendlyPieces[Piece::KING] = 1ULL << 6;

				friendlyPieces[Piece::ROOK] |= 1ULL << 5;

				friendlyPieces[Piece::ROOK] ^= 1ULL << 7;
			}
		}
		else
		{
			friendlyPieces[Piece::KING] = 0;
			if (currMove.getTo() == 58)
			{
				friendlyPieces[Piece::KING] = 1ULL << 58;

				friendlyPieces[Piece::ROOK] |= 1ULL << 59;

				friendlyPieces[Piece::ROOK] ^= 1ULL << 56;
			}
			else
			{
				friendlyPieces[Piece::KING] = 1ULL << 62;

				friendlyPieces[Piece::ROOK] |= 1ULL << 61;

				friendlyPieces[Piece::ROOK] ^= 1ULL << 63;
			}
		}
	}
	else
	{
		normalMove(fromType, toType, fromBit, toBit, currMove);
	}
}

void Board::pawnMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove)
{
	Bitboard *friendlyPieces = isWhiteTurn ? bitboards[0] : bitboards[1];
	Bitboard *opponentPieces = isWhiteTurn ? bitboards[1] : bitboards[0];
	if (currMove.isPassant())
	{
		previousMoves.back().moveType = 1;
		int oppositeSquare = (currMove.getFrom() & ~0b111) + (currMove.getTo() & 0b111);

		// Updates friendly piece and combined Bitboards
		friendlyPieces[fromType] ^= fromBit;

		friendlyPieces[fromType] |= toBit;

		// Updates opponents piece and combined Bitboards if there is a capture
		opponentPieces[Piece::PAWN] ^= 1ULL << oppositeSquare;
	}
	else if (currMove.isPromotion())
	{
		previousMoves.back().moveType = 3;
		int newColor = isWhiteTurn ? Piece::WHITE : Piece::BLACK;

		friendlyPieces[Piece::PAWN] ^= fromBit;

		if (currMove.getPromotion() == Move::QUEEN)
		{
			friendlyPieces[Piece::QUEEN] ^= toBit;
		}
		else if (currMove.getPromotion() == Move::ROOK)
		{
			friendlyPieces[Piece::ROOK] ^= toBit;
		}
		else if (currMove.getPromotion() == Move::BISHOP)
		{
			friendlyPieces[Piece::BISHOP] ^= toBit;
		}
		else
		{
			friendlyPieces[Piece::KNIGHT] ^= toBit;
		}

		if (toType)
		{
			opponentPieces[toType] ^= toBit;
		}
	}
	else
	{

		if (std::abs((currMove.getTo() / 8) - (currMove.getFrom() / 8)) == 2)
		{
			passantSquare = isSquareWhite(currMove.getFrom()) ? currMove.getTo() - 8 : currMove.getTo() + 8;
		}

		normalMove(fromType, toType, fromBit, toBit, currMove);
	}
}

void Board::unmakeMove()
{
	isWhiteTurn = !isWhiteTurn;

	Move currMove = moveHistory.back();
	moveHistory.pop_back();

	Bitboard *friendlyPieces = isWhiteTurn ? bitboards[0] : bitboards[1];
	Bitboard *opponentPieces = isWhiteTurn ? bitboards[1] : bitboards[0];

	Bitboard fromBit = 1ULL << currMove.getFrom();
	Bitboard toBit = 1ULL << currMove.getTo();

	if (!previousMoves.back().moveType)
	{
		int fromType = getSquareType(currMove.getTo());
		int toType = previousMoves.back().capturedPiece & Piece::TYPE_MASK;

		// Updates friendly piece and combined Bitboards
		friendlyPieces[fromType] ^= fromBit;

		friendlyPieces[fromType] ^= toBit;

		// Updates opponents piece and combined Bitboards if there is a capture
		if (toType)
		{
			opponentPieces[toType] ^= toBit;
		}

		// Updates the piece array
	}
	else if (previousMoves.back().moveType == 1)
	{
		int promotionType = getSquareType(currMove.getTo());
		int toType = previousMoves.back().capturedPiece & Piece::TYPE_MASK;

		// Updates friendly piece and combined Bitboards
		friendlyPieces[Piece::PAWN] ^= fromBit;

		friendlyPieces[promotionType] ^= toBit;

		// Updates opponents piece and combined Bitboards if there is a capture
		if (toType)
		{
			opponentPieces[toType] ^= toBit;
		}
	}
	else if (previousMoves.back().moveType == 2)
	{
		// TODO : Write castling unmake logic
	}
	else if (previousMoves.back().moveType == 3)
	{
		int promotionType = getSquareType(currMove.getTo());
		int toType = previousMoves.back().capturedPiece & Piece::TYPE_MASK;

		// Updates friendly piece and combined Bitboards
		friendlyPieces[Piece::PAWN] ^= fromBit;

		friendlyPieces[promotionType] ^= toBit;

		// Updates opponents piece and combined Bitboards if there is a capture
		if (toType)
		{
			opponentPieces[toType] ^= toBit;
		}
	}

	bitboards[0][0] = 0;
	bitboards[1][0] = 0;
	for (int t = 1; t <= 6; t++)
	{
		bitboards[0][0] |= bitboards[0][t]; // White combined
		bitboards[1][0] |= bitboards[1][t]; // Black combined
	}
	allCombined = bitboards[0][0] | bitboards[1][0];

	// Reset pesky variables
	castlingRights = previousMoves.back().castlingRights;
	passantSquare = previousMoves.back().passantSquare;

	previousMoves.pop_back();
}

bool Board::isSquareWhite(int index)
{
	if (bitboards[0][0] & (1ULL << index))
	{
		return true;
	}
	return false;
}

int Board::getSquareType(int index)
{
	if (bitboards[0][0] & (1ULL << index))
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

			// Check every board manually.
			// Index 1=P, 2=N, 3=B, 4=R, 5=Q, 6=K
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

	std::cout << (uint64_t)allCombined << std::endl;
	for (int i = 0; i <= Piece::KING; i++)
	{
		std::cout << (uint64_t)bitboards[0][i] << " - " << (int)i << std::endl;
	}
	for (int i = 0; i <= Piece::KING; i++)
	{
		std::cout << (uint64_t)bitboards[1][i] << " - " << (int)i << std::endl;
	}
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

	// 1. Ensure rank/file starts correctly (Rank 7 is top of FEN, Rank 0 is bottom)

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
		uint64_t bit = 1ULL << currSquare; // THE CORRECT WAY TO GET BIT

		// Determine color and piece type
		int color = islower(currChar) ? 1 : 0; // lowercase = black (1), uppercase = white (0)

		// Map char to piece type (using a small helper or manual switch)
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

		// SET THE BITS
		bitboards[color][type] |= bit;

		file++;
		i++;
	}
	i++;

	// Combine everything correctly
	for (int t = 1; t <= 6; t++)
	{
		bitboards[0][0] |= bitboards[0][t]; // White combined
		bitboards[1][0] |= bitboards[1][t]; // Black combined
	}
	allCombined = bitboards[0][0] | bitboards[1][0];

	// Section 2: Active color
	if (i < fenString.length())
	{
		if (fenString[i] == 'b')
		{
			isWhiteTurn = false;
		}
		i += 2;
	}

	// Section 3: Castling rights
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

	// Section 4: En passant square
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

	// Section 5: Halfmove clock
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

	// Section 6: Fullmove counter
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
	// 1. Parse the squares (e.g., "e2e4")
	int fromFile = moveStr[0] - 'a';
	int fromRank = moveStr[1] - '1';
	int toFile = moveStr[2] - 'a';
	int toRank = moveStr[3] - '1';

	// Convert coordinates to 0-63 index (Adjust formula if your board is flipped)
	int fromSq = fromRank * 8 + fromFile;
	int toSq = toRank * 8 + toFile;

	int flag = 0;
	int promotionPiece = 0;

	// 2. Handle Promotion (e.g., "a7a8q")
	if (moveStr.length() == 5)
	{
		flag = Move::PROMOTION; // Use your bitshift logic
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

	// 3. Special Case Handling (En Passant / Castling)
	// Perftree sends these as standard moves (e.g., e1g1 for white kingside castle).
	// To set the flags correctly, we check the piece types on the current board.

	int piece = getSquareType(fromSq); // Assume you have a way to get the piece type
	int targetPiece = getSquareType(toSq);

	// Extract the type (Pawn, King, etc.) by masking out the color
	int pieceType = piece & Piece::TYPE_MASK;

	// Castling check: Is it a king moving 2 squares horizontally?
	if (pieceType == Piece::KING && std::abs(fromFile - toFile) == 2)
	{
		flag = Move::CASTLING;
	}

	// En Passant check: Is it a pawn moving diagonally to an empty square?
	// (In chess, a diagonal pawn move to an empty square is always an En Passant capture)
	if (pieceType == Piece::PAWN && (fromFile != toFile) && targetPiece == 0) // assuming 0 is empty
	{
		flag = Move::PASSANT;
	}

	return Move(fromSq, toSq, flag, promotionPiece);
}

std::string Board::moveToString(Move move)
{
	// 1. Calculate basic coordinates (always present)
	char fromFile = 'a' + (move.getFrom() % 8);
	char fromRank = '1' + (move.getFrom() / 8);
	char toFile = 'a' + (move.getTo() % 8);
	char toRank = '1' + (move.getTo() / 8);

	// 2. Build the base string (e.g., "e7e8")
	std::string moveStr = {fromFile, fromRank, toFile, toRank};

	// 3. Handle promotions
	if (move.isPromotion())
	{
		char promotionType = 'q'; // Default to queen just in case

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

		// Append the piece character (e.g., "e7e8q")
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

	// 3. Iterate through history and replay moves
	for (size_t i = 0; i < moveHistory.size(); i++)
	{
		const Move &move = moveHistory[i];

		// --- PRINTING LOGIC (Algebraic Notation) ---
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