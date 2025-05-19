#include "board.h"

//constructors

//Could potentially parse using string_view in the future for optimization
//Will stick with string stream and getline for now
//Sets the board state according to the FEN string
void cBoard::setFromFEN(const std::string& FEN)
{
    pieceBB[WHITE][PAWN] = 0;
    pieceBB[WHITE][KNIGHT] = 0;
    pieceBB[WHITE][BISHOP] = 0;
    pieceBB[WHITE][ROOK] = 0;
    pieceBB[WHITE][QUEEN] = 0;
    pieceBB[WHITE][KING] = 0;

    pieceBB[BLACK][PAWN] = 0;
    pieceBB[BLACK][KNIGHT] = 0;
    pieceBB[BLACK][BISHOP] = 0;
    pieceBB[BLACK][ROOK] = 0;
    pieceBB[BLACK][QUEEN] = 0;
    pieceBB[BLACK][KING] = 0;

    //Set bitboards
    std::stringstream ss(FEN);
    std::string piecePlacement;
    getline(ss, piecePlacement, ' ');
    std::stringstream Pieces(piecePlacement);
    std::string rank;
    int rankCount = 7;
    while (getline(Pieces, rank, '/'))
    {
        int n = rank.size();
        int bitsToMove = 0;
        Bitboard mask;
        for (int i = 0; i < n; i++)
        {
            char bitPos = rank[i];
            mask = fenMask(rankCount, bitsToMove);
            switch(bitPos)
            {
                case ('K'): pieceBB[WHITE][KING] = mask; kingSq[WHITE] = __builtin_ctzll(mask); break;

                case ('k'): pieceBB[BLACK][KING] = mask; kingSq[BLACK] = __builtin_ctzll(mask); break;

                case ('B'): pieceBB[WHITE][BISHOP] |= mask; break;

                case ('b'): pieceBB[BLACK][BISHOP] |= mask; break;

                case ('N'): pieceBB[WHITE][KNIGHT] |= mask; break;

                case ('n'): pieceBB[BLACK][KNIGHT] |= mask; break;

                case ('R'): pieceBB[WHITE][ROOK] |= mask; break;

                case ('r'): pieceBB[BLACK][ROOK] |= mask; break;

                case ('Q'): pieceBB[WHITE][QUEEN] |= mask; break;

                case ('q'): pieceBB[BLACK][QUEEN] |= mask; break;

                case ('P'): pieceBB[WHITE][PAWN] |= mask; break;

                case ('p'): pieceBB[BLACK][PAWN] |= mask; break;

                default: bitsToMove += int(bitPos % 49); break;
            }
            bitsToMove++;
        }
        rankCount--;
    }
    //Set occupied squares
    occupancy[WHITE] = pieceBB[WHITE][PAWN] | pieceBB[WHITE][KNIGHT] | pieceBB[WHITE][BISHOP] | pieceBB[WHITE][ROOK] | pieceBB[WHITE][QUEEN] | pieceBB[WHITE][KING];
    occupancy[BLACK] = pieceBB[BLACK][PAWN] | pieceBB[BLACK][KNIGHT] | pieceBB[BLACK][BISHOP] | pieceBB[BLACK][ROOK] | pieceBB[BLACK][QUEEN] | pieceBB[BLACK][KING];
    allOccupiedSquares = occupancy[WHITE] | occupancy[BLACK];

    //Set side to move
    std::string stmSection;
    getline(ss, stmSection, ' ');

    if (stmSection[0] == 'w') {stm = WHITE;}
    else {stm = BLACK;}

    //Set castling rights
    getline(ss, stmSection, ' ');
    int n = stmSection.length();
    castling = 0;
    for (int i = 0; i < n; i++)
    {
        char castRight = stmSection[i];
        switch(castRight)
        {
            case('K'): castling |= whiteKingSide; break;
            case('k'): castling |= blackKingSide; break;
            case('Q'): castling |= whiteQueenSide; break;
            case('q'): castling |= blackQueenSide; break;
        }
    }

    //en passant target square
    getline(ss, stmSection, ' ');
    if (stmSection == "-") {enPassant = -1;}
    else {enPassant = (Square)((stmSection[1]-'1')*8+(stmSection[0]-'a'));}

    //halfmove clack
    getline(ss,stmSection,' ');
    halfmoveCounter = std::stoi(stmSection);

    //fullmove counter
    getline(ss, stmSection, ' ');
    fullmoveCounter = std::stoi(stmSection);

}

std::string cBoard::getFEN() const
{
    std::stringstream outputFEN;
    for (int rank = 7; rank >= 0; rank--)
    {
        int emptySquareCount = 0;
        for (int file = 0; file < 8; file++)
        {
            Square squareToCheck = rank*8 + file;
            Bitboard boolFound = allOccupiedSquares & (1ULL << squareToCheck);
            if (boolFound)
            {
                if (emptySquareCount) {outputFEN << std::to_string(emptySquareCount); emptySquareCount = 0;}
                //know that we found a piece
                //need to determine piece type and color
                Side color = (boolFound & occupancy[WHITE]) ? WHITE : BLACK;
                pieceType pt = NO_PIECE;
                for (int i = 0; i < 6; i++)
                {
                    if (pieceBB[color][i] & boolFound) {pt = (pieceType)i;}
                }
                char c;
                switch (pt)
                {
                    case PAWN:   c = 'p'; break;
                    case KNIGHT: c = 'n'; break;
                    case BISHOP: c = 'b'; break;
                    case ROOK:   c = 'r'; break;
                    case QUEEN:  c = 'q'; break;
                    case KING:   c = 'k'; break;
                    default:     c = '?'; break; 
                }
                outputFEN << (char)(color ? c : toupper(c));
            }
            else {emptySquareCount++;}
        }
        if (emptySquareCount) {outputFEN << std::to_string(emptySquareCount);}
        if (rank != 0) {outputFEN << '/';}
    }

    //side to move
    outputFEN << ' ' << (stm ? 'b' : 'w') << ' ';
    
    //castling rights
    std::string castRightFEN = "";

    if (castling & whiteKingSide) {castRightFEN += "K";}
    if (castling & whiteQueenSide) {castRightFEN += "Q";}
    if (castling & blackKingSide) {castRightFEN += "k";}
    if (castling & blackQueenSide) {castRightFEN += "q";}
    if (castRightFEN == "") {castRightFEN = "-";}
    outputFEN << castRightFEN << ' ';

    //en passant square
    if (enPassant == -1)
    {
        outputFEN << '-' << ' ';
    }
    else
    {
        int rank = enPassant / 8;
        int file = enPassant % 8;

        char fileCharacter = 97 + file;
        char rankCharacter = 49 + rank;

        outputFEN << fileCharacter << rankCharacter;
    }

    //halfmove counter
    outputFEN << ' ' << halfmoveCounter;

    //fullmove counter
    outputFEN << ' ' << fullmoveCounter;

    return outputFEN.str();
}

//initialize the board state to the starting position
void cBoard::resetBoard()
{
    pieceBB[WHITE][PAWN] = 0x000000000000FF00ULL;
    pieceBB[WHITE][KNIGHT] = 0x0000000000000042ULL;
    pieceBB[WHITE][BISHOP] = 0x0000000000000024ULL;
    pieceBB[WHITE][ROOK] = 0x0000000000000081ULL;
    pieceBB[WHITE][QUEEN] = 0x0000000000000008ULL;
    pieceBB[WHITE][KING] = 0x0000000000000010ULL;

    pieceBB[BLACK][PAWN] = 0x00FF000000000000ULL;
    pieceBB[BLACK][KNIGHT] = 0x4200000000000000ULL;
    pieceBB[BLACK][BISHOP] = 0x2400000000000000ULL;
    pieceBB[BLACK][ROOK] = 0x8100000000000000ULL;
    pieceBB[BLACK][QUEEN] = 0x0800000000000000ULL;
    pieceBB[BLACK][KING] = 0x1000000000000000ULL;

    occupancy[WHITE] = 0x000000000000FFFFULL;
    occupancy[BLACK] = 0xFFFF000000000000ULL;
    allOccupiedSquares = 0xFFFF00000000FFFFULL;

    kingSq[WHITE] = 4;
    kingSq[BLACK] = 60;

    stm = WHITE;
    enPassant = -1;
    castling = 15;
    halfmoveCounter = 0;
    fullmoveCounter = 0;
}

void cBoard::makeMove(Move move)
{

}

void cBoard::undoMove()
{

}

bool cBoard::isSquareAttacked(Square sq, Side attackingSide) const
{

}

bool cBoard::inCheck(Side side) const
{

}

Bitboard cBoard::pieces(Side side, pieceType pt) const {return pieceBB[side][pt];}
Bitboard cBoard::occupied(Side side) const {return occupancy[side];}
Bitboard cBoard::allOccupied() const {return allOccupiedSquares;}
Square cBoard::kingSquare(Side side) const {return kingSq[side];}
Side cBoard::sideToMove() const {return stm;}
Square cBoard::enPassantSquare() const {return enPassant;}
int cBoard::castlingRights() const {return castling;}
int cBoard::halfmoveClock() const {return halfmoveCounter;}
int cBoard::fullmoveNumber() const {return fullmoveCounter;}

uint64_t cBoard::hash() const {}
