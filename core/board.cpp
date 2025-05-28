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
        int n = rank.length();
        int bitsToMove = 0;
        Bitboard mask;
        char bitPos;
        int squareNum;
        for (int i = 0; i < n; i++)
        {
            bitPos = rank[i];
            mask = fenMask(rankCount, bitsToMove);
            squareNum = 8*rankCount+bitsToMove;
            switch(bitPos)
            {
                case ('K'): pieceBB[WHITE][KING] = mask; kingSq[WHITE] = squareNum; pieceMailbox[squareNum] = W_KING; break; // kingSq[WHITE] = __builtin_ctzll(mask); break;

                case ('k'): pieceBB[BLACK][KING] = mask; kingSq[BLACK] = squareNum; pieceMailbox[squareNum] = B_KING; break; // = __builtin_ctzll(mask); break;

                case ('B'): pieceBB[WHITE][BISHOP] |= mask; pieceMailbox[squareNum] = W_BISHOP; break;

                case ('b'): pieceBB[BLACK][BISHOP] |= mask; pieceMailbox[squareNum] = B_BISHOP; break;

                case ('N'): pieceBB[WHITE][KNIGHT] |= mask; pieceMailbox[squareNum] = W_KNIGHT; break;

                case ('n'): pieceBB[BLACK][KNIGHT] |= mask; pieceMailbox[squareNum] = B_KNIGHT; break;

                case ('R'): pieceBB[WHITE][ROOK] |= mask; pieceMailbox[squareNum] = W_ROOK; break;

                case ('r'): pieceBB[BLACK][ROOK] |= mask; pieceMailbox[squareNum] = B_ROOK; break;

                case ('Q'): pieceBB[WHITE][QUEEN] |= mask; pieceMailbox[squareNum] = W_QUEEN; break;

                case ('q'): pieceBB[BLACK][QUEEN] |= mask; pieceMailbox[squareNum] = B_QUEEN; break;

                case ('P'): pieceBB[WHITE][PAWN] |= mask; pieceMailbox[squareNum] = W_PAWN; break;

                case ('p'): pieceBB[BLACK][PAWN] |= mask; pieceMailbox[squareNum] = B_PAWN; break;

                default: bitsToMove += int(bitPos % 49); for (int i = squareNum; i <= (squareNum+(bitPos%49)); i++) {pieceMailbox[i] = EMPTY;} break;
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
    //Can replace nearly this entire section by iterating over the pieceMailbox array
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
    for (int i = 8; i < 16; i++) {pieceMailbox[i] = W_PAWN;}
    pieceBB[WHITE][KNIGHT] = 0x0000000000000042ULL;
    pieceMailbox[1] = W_KNIGHT;
    pieceMailbox[6] = W_KNIGHT;
    pieceBB[WHITE][BISHOP] = 0x0000000000000024ULL;
    pieceMailbox[2] = W_BISHOP;
    pieceMailbox[5] = W_BISHOP;
    pieceBB[WHITE][ROOK] = 0x0000000000000081ULL;
    pieceMailbox[0] = W_ROOK;
    pieceMailbox[7] = W_ROOK;
    pieceBB[WHITE][QUEEN] = 0x0000000000000008ULL;
    pieceMailbox[3] = W_QUEEN;
    pieceBB[WHITE][KING] = 0x0000000000000010ULL;
    pieceMailbox[4] = W_KING;

    pieceBB[BLACK][PAWN] = 0x00FF000000000000ULL;
    for (int i = 48; i < 56; i++) {pieceMailbox[i] = B_PAWN;}
    pieceBB[BLACK][KNIGHT] = 0x4200000000000000ULL;
    pieceMailbox[57] = B_KNIGHT;
    pieceMailbox[62] = B_KNIGHT;
    pieceBB[BLACK][BISHOP] = 0x2400000000000000ULL;
    pieceMailbox[58] = B_BISHOP;
    pieceMailbox[61] = B_BISHOP;
    pieceBB[BLACK][ROOK] = 0x8100000000000000ULL;
    pieceMailbox[56] = B_ROOK;
    pieceMailbox[63] = B_ROOK;
    pieceBB[BLACK][QUEEN] = 0x0800000000000000ULL;
    pieceMailbox[59] = B_QUEEN;
    pieceBB[BLACK][KING] = 0x1000000000000000ULL;
    pieceMailbox[60] = B_KING;

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
    //Timing("makeMove",)
    //incremental update to board state
    Square destSquare = move & 0x0000003F;
    Bitboard destSquareMask = 1ULL << (destSquare);
    Square sourceSquare = (move & 0x00000FC0) >> 6;
    Bitboard sourceSquareMask = 1ULL << (sourceSquare);
    int special = (move & 0x0000F000) >> 12;

    Side color = stm;
    Side oppColor = (stm ? WHITE : BLACK);

    //Find the source and destination square pieces
    //Can replace later with O(1) lookup if I implement 
    //a mailbox or piecelist hybrid appraoch
    // pieceType sourcePiece = NO_PIECE;
    // pieceType destPiece = NO_PIECE;
    // for (int i = 0; i < 6; i++)
    // {
    //     if (pieceBB[color][i] & sourceSquareMask)
    //     {
    //         sourcePiece = (pieceType)i;
    //     }
    //     if (pieceBB[oppColor][i] & destSquareMask)
    //     {
    //         destPiece = (pieceType)i;
    //     }
    // }

    //Constant O(1) lookup to find source and destination piece types
    //Approach using pieceMailbox array
    pieceCode srcCode = pieceMailbox[sourceSquare];
    pieceType sourcePiece = getPieceType(srcCode);
    pieceCode destCode = pieceMailbox[destSquare];
    pieceType destPiece = getPieceType(destCode); //only need if there's a capture move, so this may appear redundant. destPiece == NO_PIECE is needed for halfmovecounter increment

    //Save irreversible board state data for undoMove
    Undo madeMove;
    madeMove.move = move;
    madeMove.hash = 0; // replace 0 with zobrist hash later on
    madeMove.castling = castling;
    madeMove.enPassant = enPassant;
    madeMove.capturedPiece = destPiece;
    madeMove.halfmove = halfmoveCounter;

    //Place Undo struct into the moveHistory
    history.push(madeMove);

    //Toggle the bitboards for the moving piece and color
    //We know that the source piece will leave the source square and end on dest
    //square no matter what king of move that it is
    pieceBB[color][sourcePiece] ^= (sourceSquareMask | destSquareMask);
    pieceMailbox[sourceSquare] = EMPTY; pieceMailbox[destSquare] = srcCode;
    occupancy[color] ^= (sourceSquareMask | destSquareMask);

    //regular promotion
    if (special >= KNIGHT_PROMOTION && special <= QUEEN_PROMOTION)
    {
        //8 knight promo -> pieceType = Special - 7
        //9 bishop promo
        //10 rook promo
        //11 queen promo
        pieceBB[color][special - 7] ^= destSquareMask;
        pieceBB[color][PAWN] ^= destSquareMask;
        pieceMailbox[destSquare] = static_cast<pieceCode>((special-7)+6*color);
    }

    //promo capture (same relative ordering as regular promotion)
    else if (special >= KNIGHT_PROMO_CAPTURE && special <= QUEEN_PROMO_CAPTURE)
    {
        pieceBB[color][special - 11] ^= destSquareMask;
        pieceBB[color][PAWN] ^= destSquareMask;

        pieceBB[oppColor][destPiece] ^= destSquareMask;
        occupancy[oppColor] ^= destSquareMask;

        pieceMailbox[destSquare] = static_cast<pieceCode>((special-11)+color*6);
    }

    //castles (2 king side, 3 queen side)
    else if (special == KING_CASTLE || special == QUEEN_CASTLE)
    {
        //castling is encoded as a king move
        //So the destSquare mask must be modified by one or two squares
        //adjust the rook position
        Bitboard rookNewPos = destSquareMask << 1;
        Bitboard rookOldPos = destSquareMask >> (special - 1);

        pieceBB[color][ROOK] ^= rookNewPos | rookOldPos;
        occupancy[color] ^= rookNewPos | rookOldPos;

        pieceMailbox[destSquare+1] == static_cast<pieceCode>(color*6+ROOK) ? pieceMailbox[destSquare+1] = EMPTY : pieceMailbox[destSquare+1] = static_cast<pieceCode>(ROOK+color*6); //Need to toggle the pieceMailbox for the rook moving
        pieceMailbox[destSquare-(special-1)] == static_cast<pieceCode>(color*6+ROOK) ? pieceMailbox[destSquare-(special-1)] = EMPTY : pieceMailbox[destSquare-(special-1)] = static_cast<pieceCode>(ROOK+color*6);
    }

    //captures
    else if (special == CAPTURE)
    {
        pieceBB[oppColor][destPiece] ^= destSquareMask;
        occupancy[oppColor] ^= destSquareMask;
    }

    //en passant capture
    else if (special == EN_PASSANT)
    {
        Bitboard oppColorPawnPos;
        if (oppColor) {oppColorPawnPos = destSquareMask >> 8;}
        else          {oppColorPawnPos = destSquareMask << 8;}

        pieceBB[oppColor][PAWN] ^= oppColorPawnPos;
        occupancy[oppColor] ^= oppColorPawnPos;

        pieceMailbox[__builtin_ctzll(oppColorPawnPos)] = EMPTY;
    }

    //double pawn push
    else if (special == DOUBLE_PAWN_PUSH)
    {
        if (color) {enPassant = __builtin_ctzll(destSquareMask << 8);}
        else       {enPassant = __builtin_ctzll(destSquareMask >> 8);}
    }

    //set castling rights
    if (sourcePiece == KING) {castling &= ~((0b0011) << ((int)color*2)); kingSq[color] = destSquare;}
    
    if (sourcePiece == ROOK || destPiece == ROOK)
    {
        if (sourceSquareMask == 0x0000000000000001 || destSquareMask == 0x0000000000000001)
        {castling &= ~whiteQueenSide;}
        else if (sourceSquareMask == 0x0000000000000080 || destSquareMask == 0x0000000000000080)
        {castling &= ~whiteKingSide;}
        else if (sourceSquareMask == 0x0100000000000000 || destSquareMask == 0x0100000000000000)
        {castling &= ~blackQueenSide;}
        else if (sourceSquareMask == 0x8000000000000000 || destSquareMask == 0x8000000000000000)
        {castling &= ~blackKingSide;}
    }

    //set side to move, halfmove counter, full move counter
    stm = oppColor;
    halfmoveCounter =  (sourcePiece == PAWN || special == EN_PASSANT || destPiece != NO_PIECE) ? 0 : ++halfmoveCounter;
    if (stm == WHITE) {fullmoveCounter++;}
    if (special != DOUBLE_PAWN_PUSH) {enPassant = -1;}

    allOccupiedSquares = occupancy[WHITE] | occupancy[BLACK];
}

void cBoard::undoMove()
{
    //Timing("undoMove",)
    //Pop off irreversible info from back of history vector
    //inverse bitboard operations
    /*
     * Struct Undo
     * {
     *    uint64_t hash;
     *    Move move;
     *    int castling;
     *    Square enPassant;
     *    int halfmove;
     */

    Undo lastMove = history.top();
    history.pop();
    
    Square destSquare = lastMove.move & 0x0000003F;
    Bitboard destSquareMask = 1ULL << (destSquare);
    Square sourceSquare = (lastMove.move & 0x00000FC0) >> 6;
    Bitboard sourceSquareMask = 1ULL << (sourceSquare);
    int special = (lastMove.move & 0x0000F000) >> 12;
    Side color;
    Side oppColor;
    //go ahead and set the side to move and fullmove counter
    if (stm == WHITE) {color = BLACK; oppColor = WHITE; --fullmoveCounter; stm = BLACK;} else {color = WHITE; oppColor = BLACK; stm = WHITE;}

    
    pieceCode srcCode = pieceMailbox[destSquare];
    pieceType sourcePiece = getPieceType(srcCode);
    pieceType destPiece = lastMove.capturedPiece; //putting this error intentionally to remind myself to fix the problem with restoring the proper color destpiece

    //When reversing the move, we know that the source square will contain the source piece
    //and the destination square will contain the captured piece or NO_PIECE
    //This information is sufficient to reverse any non-castle or enPassant move by toggling bitboards and piece mailbox
    pieceBB[color][sourcePiece] ^= (sourceSquareMask | destSquareMask);
    pieceMailbox[sourceSquare] = srcCode; 
    if (destPiece == NO_PIECE)  {pieceMailbox[destSquare] = EMPTY;}  
    else {pieceMailbox[destSquare] = static_cast<pieceCode>(destPiece+6*oppColor); pieceBB[oppColor][destPiece] ^= destSquareMask; occupancy[oppColor] ^= destSquareMask;}
    occupancy[color] ^= (sourceSquareMask | destSquareMask);

    //Reset rook position for castling moves
    if (special == KING_CASTLE || special == QUEEN_CASTLE)
    {
        //castling is encoded as a king move
        //So the destSquare mask must be modified by one or two squares
        //adjust the rook position
        Bitboard rookNewPos = destSquareMask << 1;
        Bitboard rookOldPos = destSquareMask >> (special - 1);

        pieceBB[color][ROOK] ^= rookNewPos | rookOldPos;
        occupancy[color] ^= rookNewPos | rookOldPos;

        pieceMailbox[destSquare+1] == static_cast<pieceCode>(color*6+ROOK) ? pieceMailbox[destSquare+1] = EMPTY : pieceMailbox[destSquare+1] = static_cast<pieceCode>(ROOK+color*6); //Need to toggle the pieceMailbox for the rook moving
        pieceMailbox[destSquare-(special-1)] == static_cast<pieceCode>(color*6+ROOK) ? pieceMailbox[destSquare-(special-1)] = EMPTY : pieceMailbox[destSquare-(special-1)] = static_cast<pieceCode>(ROOK+color*6);
    }

    //Reset opponent pawn position for enPassant captures
    if (special == EN_PASSANT)
    {
        Bitboard oppColorPawnPos;
        int epCapture;
        if (oppColor) {oppColorPawnPos = destSquareMask >> 8; epCapture = destSquare - 8;}
        else          {oppColorPawnPos = destSquareMask << 8; epCapture = destSquare + 8;}

        pieceBB[oppColor][PAWN] ^= oppColorPawnPos;
        occupancy[oppColor] ^= oppColorPawnPos;

        pieceMailbox[epCapture] = static_cast<pieceCode>(PAWN+oppColor*6);
    }

    //Reset in case of pawn promotion
    if (special >= KNIGHT_PROMOTION && special <= QUEEN_PROMO_CAPTURE)
    {
        pieceBB[color][sourcePiece] ^= sourceSquareMask;
        pieceBB[color][PAWN] ^= sourceSquareMask;
        pieceMailbox[sourceSquare] = static_cast<pieceCode>(color*6+PAWN);
    }

    if (sourcePiece == KING) {kingSq[color] = sourceSquare;}

    allOccupiedSquares = occupancy[WHITE] | occupancy[BLACK];

    //Reset irreversible info (castling, enpassant target, halfmove counter)
    castling = lastMove.castling;
    enPassant = lastMove.enPassant;
    halfmoveCounter = lastMove.halfmove;
}

bool cBoard::isSquareAttacked(Square sq, Side attackingSide) const
{
    //Can be implemented by seeing if sq aligns with any of the attacking bitboards/lookuptables 
    //Obviously if there's alignment then the square is attacked
    //attack tables & (1ULL << sq);
}

bool cBoard::inCheck(Side side) const
{
    //could simply return isSquareAttacked(kingSq[side], ~side);
    //maybe there's a more optimal approach 
}

uint64_t cBoard::hash() const {}
