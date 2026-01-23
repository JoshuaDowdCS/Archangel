#include "board.h"
#include "piece.h"
#include "movegen.h"
#include <cstddef>
#include <iostream>
#include <iomanip>

void Board::makeMove(Move currMove)
{
	moveHistory.push_back(currMove);

	Bitboard fromBit = 1ULL << currMove.getFrom();
	Bitboard toBit = 1ULL << currMove.getTo();
	int fromType = pieceArray[currMove.getFrom()] & Piece::TYPE_MASK;
	int toType = pieceArray[currMove.getTo()] & Piece::TYPE_MASK;

	passantSquare = -1;

	if ((fromType == Piece::ROOK || fromType == Piece::KING))
	{
		if (isWhiteTurn && (whiteShortCastle || whiteLongCastle))
		{
			rookKingMove(fromType, toType, fromBit, toBit, currMove);
		}
		else if (!isWhiteTurn && (blackLongCastle || blackShortCastle))
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

	if (whiteShortCastle && toBit == (1ULL << 7))
	{
		whiteShortCastle = false;
	}
	if (whiteLongCastle && toBit == (1ULL << 0))
	{
		whiteLongCastle = false;
	}
	if (blackShortCastle && toBit == (1ULL << 63))
	{
		blackShortCastle = false;
	}
	if (blackLongCastle && toBit == (1ULL << 56))
	{
		blackLongCastle = false;
	}

	isWhiteTurn = !isWhiteTurn;

	if (isWhiteTurn)
	{
		fullmoveCounter++;
	}
}

void Board::normalMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove)
{
	Bitboard *friendlyPieces = isWhiteTurn ? bitboards[0] : bitboards[1];
	Bitboard *opponentPieces = isWhiteTurn ? bitboards[1] : bitboards[0];
	// Updates friendly piece and combined Bitboards
	friendlyPieces[fromType] ^= fromBit;
	friendlyPieces[0] ^= fromBit;

	friendlyPieces[fromType] ^= toBit;
	friendlyPieces[0] ^= toBit;

	// Updates opponents piece and combined Bitboards if there is a capture
	if (toType)
	{
		opponentPieces[toType] ^= toBit;
		opponentPieces[0] ^= toBit;
		allCombined ^= toBit;
	}

	// Updates combined board
	allCombined = bitboards[0][0] | bitboards[1][0];

	// Updates the piece array
	pieceArray[currMove.getTo()] = pieceArray[currMove.getFrom()] & 0b1111;
	pieceArray[currMove.getFrom()] = 0;
}

void Board::rookKingMove(int fromType, int toType, Bitboard fromBit, Bitboard toBit, Move currMove)
{
	Bitboard *friendlyPieces = isWhiteTurn ? bitboards[0] : bitboards[1];
	if ((pieceArray[currMove.getFrom()] & Piece::COLOR_MASK) == Piece::WHITE)
	{
		if (fromType == Piece::KING)
		{
			whiteShortCastle = whiteLongCastle = false;
		}
		else if (currMove.getFrom() == 0 && Piece::ROOK)
		{
			whiteLongCastle = false;
		}
		else if (currMove.getFrom() == 7 && Piece::ROOK)
		{
			whiteShortCastle = false;
		}
	}
	else
	{
		if (fromType == Piece::KING)
		{
			blackLongCastle = blackShortCastle = false;
		}
		else if (currMove.getFrom() == 56 && Piece::ROOK)
		{
			blackLongCastle = false;
		}
		else if (currMove.getFrom() == 63 && Piece::ROOK)
		{
			blackShortCastle = false;
		}
	}

	if (currMove.isCastling())
	{
		if ((pieceArray[currMove.getFrom()] & Piece::COLOR_MASK) == Piece::WHITE)
		{
			pieceArray[4] = 0;
			friendlyPieces[Piece::KING] = 0;
			friendlyPieces[0] ^= 1ULL << 4;
			allCombined ^= 1ULL << 4;
			if (currMove.getTo() == 2)
			{
				pieceArray[2] = Piece::KING | Piece::WHITE;
				pieceArray[3] = Piece::ROOK | Piece::WHITE;
				pieceArray[0] = 0;

				friendlyPieces[Piece::KING] = 1ULL << 2;
				friendlyPieces[0] |= 1ULL << 2;
				allCombined |= 1ULL << 2;

				friendlyPieces[Piece::ROOK] |= 1ULL << 3;
				friendlyPieces[0] |= 1ULL << 3;
				allCombined |= 1ULL << 3;

				friendlyPieces[Piece::ROOK] ^= 1ULL << 0;
				friendlyPieces[0] ^= 1ULL << 0;
				allCombined ^= 1ULL << 0;
			}
			else
			{
				pieceArray[6] = Piece::KING | Piece::WHITE;
				pieceArray[5] = Piece::ROOK | Piece::WHITE;
				pieceArray[7] = 0;

				friendlyPieces[Piece::KING] = 1ULL << 6;
				friendlyPieces[0] |= 1ULL << 6;
				allCombined |= 1ULL << 6;

				friendlyPieces[Piece::ROOK] |= 1ULL << 5;
				friendlyPieces[0] |= 1ULL << 5;
				allCombined |= 1ULL << 5;

				friendlyPieces[Piece::ROOK] ^= 1ULL << 7;
				friendlyPieces[0] ^= 1ULL << 7;
				allCombined ^= 1ULL << 7;
			}
		}
		else
		{
			pieceArray[60] = 0;
			friendlyPieces[Piece::KING] = 0;
			friendlyPieces[0] ^= 1ULL << 60;
			allCombined ^= 1ULL << 60;
			if (currMove.getTo() == 58)
			{
				pieceArray[58] = Piece::KING | Piece::WHITE;
				pieceArray[59] = Piece::ROOK | Piece::WHITE;
				pieceArray[56] = 0;

				friendlyPieces[Piece::KING] = 1ULL << 58;
				friendlyPieces[0] |= 1ULL << 58;
				allCombined |= 1ULL << 58;

				friendlyPieces[Piece::ROOK] |= 1ULL << 59;
				friendlyPieces[0] |= 1ULL << 59;
				allCombined |= 1ULL << 59;

				friendlyPieces[Piece::ROOK] ^= 1ULL << 56;
				friendlyPieces[0] ^= 1ULL << 56;
				allCombined ^= 1ULL << 56;
			}
			else
			{
				pieceArray[61] = Piece::ROOK | Piece::BLACK;
				pieceArray[62] = Piece::KING | Piece::BLACK;
				pieceArray[63] = 0;

				friendlyPieces[Piece::KING] = 1ULL << 62;
				friendlyPieces[0] |= 1ULL << 62;
				allCombined |= 1ULL << 62;

				friendlyPieces[Piece::ROOK] |= 1ULL << 61;
				friendlyPieces[0] |= 1ULL << 61;
				allCombined |= 1ULL << 61;

				friendlyPieces[Piece::ROOK] ^= 1ULL << 63;
				friendlyPieces[0] ^= 1ULL << 63;
				allCombined ^= 1ULL << 63;
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
		int oppositeSquare = (currMove.getFrom() & ~0b111) + (currMove.getTo() & 0b111);

		// Updates friendly piece and combined Bitboards
		friendlyPieces[fromType] ^= fromBit;
		friendlyPieces[0] ^= fromBit;

		friendlyPieces[fromType] |= toBit;
		friendlyPieces[0] |= toBit;

		// Updates opponents piece and combined Bitboards if there is a capture
		opponentPieces[Piece::PAWN] ^= 1ULL << oppositeSquare;
		opponentPieces[0] ^= 1ULL << oppositeSquare;
		allCombined ^= 1ULL << oppositeSquare;

		// Updates combined board
		allCombined ^= fromBit;
		allCombined ^= toBit;

		// Updates the piece array
		pieceArray[currMove.getTo()] = pieceArray[currMove.getFrom()] & 0b1111;
		pieceArray[currMove.getFrom()] = 0;
		pieceArray[oppositeSquare] = 0;
	}
	else if (currMove.isPromotion())
	{
		int newColor = isWhiteTurn ? Piece::WHITE : Piece::BLACK;

		friendlyPieces[Piece::PAWN] ^= fromBit;
		friendlyPieces[0] ^= fromBit;
		friendlyPieces[0] ^= toBit;

		pieceArray[currMove.getFrom()] = 0;

		if (currMove.getPromotion() == Move::QUEEN)
		{
			friendlyPieces[Piece::QUEEN] ^= toBit;
			pieceArray[currMove.getTo()] = Piece::QUEEN | newColor;
		}
		else if (currMove.getPromotion() == Move::ROOK)
		{
			friendlyPieces[Piece::ROOK] ^= toBit;
			pieceArray[currMove.getTo()] = Piece::ROOK | newColor;
		}
		else if (currMove.getPromotion() == Move::BISHOP)
		{
			friendlyPieces[Piece::BISHOP] ^= toBit;
			pieceArray[currMove.getTo()] = Piece::BISHOP | newColor;
		}
		else
		{
			friendlyPieces[Piece::KNIGHT] ^= toBit;
			pieceArray[currMove.getTo()] = Piece::KNIGHT | newColor;
		}

		if (toType)
		{
			opponentPieces[toType] ^= toBit;
			opponentPieces[0] ^= toBit;
			allCombined ^= toBit;
		}

		allCombined = bitboards[0][0] | bitboards[1][0];
	}
	else
	{

		if ((pieceArray[currMove.getFrom()] & Piece::COLOR_MASK) == Piece::WHITE &&
		    ((currMove.getTo() / 8) - (currMove.getFrom() / 8)) == 2)
		{
			passantSquare = currMove.getTo() - 8;
		}
		else if ((pieceArray[currMove.getFrom()] & Piece::COLOR_MASK) == Piece::BLACK &&
			 (currMove.getTo() / 8) - (currMove.getFrom() / 8) == -2)
		{
			passantSquare = currMove.getTo() + 8;
		}

		normalMove(fromType, toType, fromBit, toBit, currMove);
	}
}

void Board::unmakeMove()
{
	memcpy(bitboards, bitboardsInitial, sizeof(bitboards));

	allCombined = allCombinedInitial;

	memcpy(pieceArray, pieceArrayInitial, sizeof(pieceArray));

	whiteShortCastle = whiteShortCastleInitial;
	whiteLongCastle = whiteLongCastleInitial;
	blackShortCastle = blackShortCastleInitial;
	blackLongCastle = blackLongCastleInitial;

	passantSquare = passantSquareInitial;
	halfmoveClock = halfmoveClockInitial;

	isWhiteTurn = isWhiteTurnInitial;
	fullmoveCounter = fullmoveCounterInitial;

	std::vector<Move> unmakeMoveHistory = moveHistory;
	moveHistory = {};

	for (int i = 0; i < unmakeMoveHistory.size() - 1; i++)
	{
		makeMove(unmakeMoveHistory[i]);
	}
}

void Board::printChessBoard()
{
	std::cout << "  +-----------------+" << '\n';

	for (int rank = 7; rank >= 0; rank--)
	{ // Start from rank 8 down to 1
		std::cout << rank + 1 << " | ";

		for (int file = 0; file < 8; file++)
		{
			int index = rank * 8 + file;
			int piece = pieceArray[index];

			if (piece == 0)
			{
				std::cout << ". "; // Empty square
			}
			else
			{

				char pieceChar;
				switch (piece & Piece::TYPE_MASK)
				{
				case Piece::PAWN:
					pieceChar = 'p';
					break;
				case Piece::KNIGHT:
					pieceChar = 'n';
					break;
				case Piece::BISHOP:
					pieceChar = 'b';
					break;
				case Piece::ROOK:
					pieceChar = 'r';
					break;
				case Piece::QUEEN:
					pieceChar = 'q';
					break;
				case Piece::KING:
					pieceChar = 'k';
					break;
				default:
					pieceChar = '?';
					break;
				}

				// Use Uppercase for White, Lowercase for Black
				if ((piece & Piece::COLOR_MASK) == Piece::WHITE)
				{
					pieceChar = toupper(pieceChar);
				}

				std::cout << pieceChar << " ";
			}
		}
		std::cout << "|" << '\n';
	}

	std::cout << "  +-----------------+" << '\n';
	std::cout << "    a b c d e f g h" << '\n';
}

Board::Board(std::string fenString)
{
	moveHistory.reserve(10);

	for (int color = 0; color < 2; color++)
	{
		for (int type = 0; type < 8; type++)
		{
			bitboards[color][type] = 0;
		}
	}

	allCombined = 0;

	whiteShortCastle = whiteLongCastle = blackShortCastle = blackLongCastle = false;
	passantSquare = 0;
	halfmoveClock = 0;
	fullmoveCounter = 1;
	isWhiteTurn = true;

	for (int i = 0; i < 64; i++)
		pieceArray[i] = 0;

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
		pieceArray[currSquare] = (color == 0 ? Piece::WHITE : Piece::BLACK) + type;

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
				whiteShortCastle = true;
				break;
			case 'Q':
				whiteLongCastle = true;
				break;
			case 'k':
				blackShortCastle = true;
				break;
			case 'q':
				blackLongCastle = true;
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

	memcpy(bitboardsInitial, bitboards, sizeof(bitboardsInitial));

	allCombinedInitial = allCombined;

	memcpy(pieceArrayInitial, pieceArray, sizeof(pieceArrayInitial));

	whiteShortCastleInitial = whiteShortCastle;
	whiteLongCastleInitial = whiteLongCastle;
	blackShortCastleInitial = blackShortCastle;
	blackLongCastleInitial = blackLongCastle;

	passantSquareInitial = passantSquare;
	halfmoveClockInitial = halfmoveClock;

	isWhiteTurnInitial = isWhiteTurn;
	fullmoveCounterInitial = fullmoveCounter;
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

	int piece = pieceArray[fromSq]; // Assume you have a way to get the piece type
	int targetPiece = pieceArray[toSq];

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

#include <iostream>
#include <iomanip>
#include <cstring> // For memcpy if you prefer, or use loops as below

void Board::printMoveHistory()
{
	if (moveHistory.empty())
	{
		std::cout << "Move history is empty." << '\n';
		return;
	}

	std::cout << "Move History (Replay " << moveHistory.size() << " moves):" << '\n';
	std::cout << std::string(50, '-') << '\n';

	// 1. Create a temporary board for replaying
	// We initialize it with the standard start position to ensure the object is valid,
	// then we will immediately overwrite it with the stored Initial state.
	Board replayBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	// 2. Restore the specific Initial conditions of THIS board into the replay board
	// Copy Bitboards
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			replayBoard.bitboards[i][j] = this->bitboardsInitial[i][j];
		}
	}
	replayBoard.allCombined = this->allCombinedInitial;

	// Copy Piece Array
	for (int i = 0; i < 64; i++)
	{
		replayBoard.pieceArray[i] = this->pieceArrayInitial[i];
	}

	// Copy Game State Flags
	replayBoard.whiteShortCastle = this->whiteShortCastleInitial;
	replayBoard.whiteLongCastle = this->whiteLongCastleInitial;
	replayBoard.blackShortCastle = this->blackShortCastleInitial;
	replayBoard.blackLongCastle = this->blackLongCastleInitial;
	replayBoard.passantSquare = this->passantSquareInitial;
	replayBoard.halfmoveClock = this->halfmoveClockInitial;
	replayBoard.isWhiteTurn = this->isWhiteTurnInitial;
	replayBoard.fullmoveCounter = this->fullmoveCounterInitial;

	// Ensure the replay board's history is clear before we start adding to it
	replayBoard.moveHistory.clear();

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

		// --- REPLAY LOGIC ---
		// Apply the move to our temporary board
		replayBoard.makeMove(move);

		std::cout << '\n';
	}

	std::cout << std::string(50, '-') << '\n';
}