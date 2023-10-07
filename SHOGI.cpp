#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
using namespace std;
int upper_boundary = 5;
int left_boundary = 20;
int box_size = 8;
int dimension = 9;
void getRowColbyLeftClick(int& rpos, int& cpos)
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD Events;
    INPUT_RECORD InputRecord;
    SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
    do
    {
        ReadConsoleInput(hInput, &InputRecord, 1, &Events);
        if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
        {
            cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
            rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
            break;
        }
    } while (true);
}
void gotoRowCol(int rpos, int cpos)
{
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = cpos;
    scrn.Y = rpos;
    SetConsoleCursorPosition(hOuput, scrn);
}
void SetClr(int clr)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
}

void print_horline(int row, int start, int end, char sym, int color)
{
    SetClr(color);
    for (int i = start; i <= end; i++)
    {

        gotoRowCol(row, i);
        cout << sym;
    }
}
void print_verline(int col, int start, int end, char sym, int color)
{
    SetClr(color);
    for (int i = start; i <= end; i++)
    {

        gotoRowCol(i, col);
        cout << sym;
    }
}
//...................................
struct piece
{
    char symbol = '-';
    int player = -1;
    int power = 0;
};
void initialize_board(piece**& board, int dim)
{
    ifstream initial_pattern_file("SHOGI_PATTERN.txt");
    board = new piece * [dim];
    for (int i = 0; i < dim; i++)
    {
        board[i] = new piece[dim];
    }
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            initial_pattern_file >> board[i][j].symbol;

            if (board[i][j].symbol != '-' && board[i][j].symbol >= 97 && board[i][j].symbol <= 122)
            {
                board[i][j].player = 0;
            }
            else if (board[i][j].symbol != '-')
            {
                board[i][j].player = 1;
            }
        }
    }
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            initial_pattern_file >> board[i][j].symbol;

            if (board[i][j].symbol != '-' && board[i][j].symbol >= 97 && board[i][j].symbol <= 122)
            {
                board[i][j].symbol = board[i][j].symbol - 32;
            }
        }
    }

}
//....................................................
void get_board_coordinates_by_click(int& x, int& y, int upper_boundary, int left_boundary)
{
    do
    {
        getRowColbyLeftClick(x, y);
    } while (x<upper_boundary || x>upper_boundary + 8 * 9 || y<left_boundary || y>left_boundary + 8 * 9);
    x -= 5;
    y -= 20;
    x /= 8;
    y /= 8;

}
//........................................................
void input_players_name(string names[])
{
    cout << "Player 1 name :";
    cin >> names[0];
    cout << "Player 2 name :";
    cin >> names[1];
}
//.......................................................
bool king_valid_move(int src[], int des[])
{
    if (abs(des[0] - src[0]) > 1 || abs(des[1] - src[1]) > 1)
        return false;
    return true;
}
bool gold_valid_move(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].player == 0)
    {
        if ((abs(des[0] - src[0]) > 1 || abs(des[1] - src[1]) > 1) || (des[0] - src[0] == 1 && abs(des[1] - src[1]) == 1))
            return false;
        return true;
    }
    else
    {
        if ((abs(des[0] - src[0]) > 1 || abs(des[1] - src[1]) > 1) || (des[0] - src[0] == -1 && abs(des[1] - src[1]) == 1))
            return false;
        return true;
    }
}
bool silver_valid_move(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].power == 0)
    {
        if (board[src[0]][src[1]].player == 0)
        {
            if (abs(des[0] - src[0]) > 1 || abs(des[1] - src[1]) > 1 || (src[0] == des[0] && abs(des[1] - src[1]) == 1) || (src[1] == des[1] && des[0] - src[0] == 1))
                return false;
            return true;
        }
        if (board[src[0]][src[1]].player == 1)
        {
            if (abs(des[0] - src[0]) > 1 || abs(des[1] - src[1]) > 1 || (src[0] == des[0] && abs(des[1] - src[1]) == 1) || (src[1] == des[1] && des[0] - src[0] == -1))
                return false;
            return true;
        }

    }
    else
    {
        return gold_valid_move(board, src, des);
    }
    return 0;
}
bool knight_valid_move(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].power == 0)
    {
        if (board[src[0]][src[1]].player == 0)
        {
            if (abs(des[1] - src[1]) == 1 && src[0] - des[0] == 2)
            {
                return true;
            }
            return false;
        }
        if (board[src[0]][src[1]].player == 1)
        {
            if (abs(des[1] - src[1]) == 1 && des[0] - src[0] == 2)
                return true;
            return false;
        }

    }
    else
    {
        return gold_valid_move(board, src, des);
    }
    return 0;
}
bool lance_valid_move(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].power == 0)
    {
        if (board[src[0]][src[1]].player == 0)
        {
            if (abs(des[1] - src[1]) == 0 && src[0] - des[0] >= 0)
                return true;
            return false;
        }
        if (board[src[0]][src[1]].player == 1)
        {
            if (abs(des[1] - src[1]) == 0 && des[0] - src[0] >= 0)
                return true;
            return false;
        }

    }
    else
    {
        return gold_valid_move(board, src, des);
    }
    return 0;
}
bool bishope_valid_move(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].power == 0)
    {
        if (abs(src[0] - des[0]) == abs(src[1] - des[1]))
            return true;
        return false;
    }
    else
    {
        if (abs(des[0] - src[0]) > 1 || abs(des[1] - src[1]) > 1)
        {
            if (abs(src[0] - des[0]) == abs(src[1] - des[1]))
                return true;
            return false;

        }
        return true;
    }
}
bool rook_valid_move(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].power == 0)
    {
        if (abs(src[0] - des[0]) == 0 || abs(src[1] - des[1]) == 0)
            return true;
        return false;
    }
    else
    {
        if (abs(des[0] - src[0]) > 1 || abs(des[1] - src[1]) > 1)
        {
            if (abs(src[0] - des[0]) == 0 || abs(src[1] - des[1]) == 0)
                return true;
            return false;

        }
        return true;
    }
}
bool pawn_valid_move(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].power == 0)
    {
        if (board[src[0]][src[1]].player == 0)
        {
            if (des[0] - src[0] == -1 && abs(des[1] - src[1] == 0))
                return true;
            return false;
        }
        if (board[src[0]][src[1]].player == 1)
        {
            if (des[0] - src[0] == 1 && abs(des[1] - src[1] == 0))
                return true;
            return false;
        }
    }
    else
        return gold_valid_move(board, src, des);
    return 0;
}
bool move_according_to_rule(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].symbol == 'K')
    {
        return king_valid_move(src, des);
    }
    else if (board[src[0]][src[1]].symbol == 'G')
    {
        return gold_valid_move(board, src, des);
    }
    else if (board[src[0]][src[1]].symbol == 'S')
    {
        return silver_valid_move(board, src, des);
    }
    else if (board[src[0]][src[1]].symbol == 'P')
    {
        return pawn_valid_move(board, src, des);
    }
    else if (board[src[0]][src[1]].symbol == 'N')
    {
        return knight_valid_move(board, src, des);
    }
    else if (board[src[0]][src[1]].symbol == 'L')
    {
        return lance_valid_move(board, src, des);
    }
    else if (board[src[0]][src[1]].symbol == 'B')
    {
        return bishope_valid_move(board, src, des);
    }
    else if (board[src[0]][src[1]].symbol == 'R')
    {
        return rook_valid_move(board, src, des);
    }

    return true;
}
//........................................................
bool lance_over_other(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].power == 0)
    {
        if (board[src[0]][src[1]].player == 0)
        {
            for (int i = 1; i < src[0] - des[0]; i++)
            {
                if (board[des[0] + i][des[1]].symbol != '-')
                    return true;
            }
            return false;
        }
        if (board[src[0]][src[1]].player == 1)
        {
            for (int i = 1; i < des[0] - src[0]; i++)
            {
                if (board[des[0] - i][des[1]].symbol != '-')
                    return true;
            }
            return false;
        }

    }
    return false;

}
bool bishope_over_other(piece** board, int src[], int des[])
{
    if (abs(src[0] - des[0]) != abs(src[1] - des[1]))
    {
        return false;
    }
    else
    {
        if (des[0]<src[0] && des[1]>src[1])
        {
            for (int i = 1; i < abs(des[0] - src[0]); i++)
            {
                if (board[src[0] - i][src[1] + i].symbol != '-')
                    return true;
            }
            return false;
        }
        else if (des[0] < src[0] && des[1] < src[1])
        {
            for (int i = 1; i < abs(des[0] - src[0]); i++)
            {
                if (board[src[0] - i][src[1] - i].symbol != '-')
                    return true;
            }
            return false;
        }
        else if (des[0] > src[0] && des[1] < src[1])
        {
            for (int i = 1; i < abs(des[0] - src[0]); i++)
            {
                if (board[src[0] + i][src[1] - i].symbol != '-')
                    return true;
            }
            return false;
        }
        else
        {
            for (int i = 1; i < abs(des[0] - src[0]); i++)
            {
                if (board[src[0] + i][src[1] + i].symbol != '-')
                    return true;
            }
            return false;
        }
    }
}
bool rook_over_other(piece** board, int src[], int des[])
{
    if (abs(src[0] - des[0]) == abs(src[1] - des[1]))
    {
        return false;
    }
    else
    {
        if (des[0] < src[0])
        {
            for (int i = 1; i < abs(des[0] - src[0]); i++)
            {
                if (board[src[0] - i][src[1]].symbol != '-')
                {
                    return true;
                }
            }
            return false;
        }
        else if (des[0] > src[0])
        {
            for (int i = 1; i < abs(des[0] - src[0]); i++)
            {
                if (board[src[0] + i][src[1]].symbol != '-')
                {
                    return true;
                }

            }
            return false;
        }
        else if (des[1] < src[1])
        {
            for (int i = 1; i < abs(des[1] - src[1]); i++)
            {
                if (board[src[0]][src[1] - i].symbol != '-')
                {
                    return true;
                }

            }
            return false;
        }
        else
        {
            for (int i = 1; i < abs(des[1] - src[1]); i++)
            {
                if (board[src[0]][src[1] + i].symbol != '-')
                {
                    return true;
                }

            }
            return false;
        }
    }
    return 0;
}
bool is_piece_over_other(piece** board, int src[], int des[])
{


    if (board[src[0]][src[1]].symbol == 'L')
    {
        return lance_over_other(board, src, des);
    }
    else if (board[src[0]][src[1]].symbol == 'B')
    {
        return bishope_over_other(board, src, des);
    }
    else if (board[src[0]][src[1]].symbol == 'R')
    {
        return rook_over_other(board, src, des);
    }
    return false;
}
//........................................................
bool selected_yourself_move(piece** board, int src[], int turn)
{
    if (turn == board[src[0]][src[1]].player)
        return true;
    return false;
}
bool is_valid_source_move(piece** board, int src[], int turn)
{
    if (selected_yourself_move(board, src, turn))
        return true;
    return false;
}
//..............................................................
bool is_killing_its_own_man(piece** board, int src[], int des[])
{
    if (board[src[0]][src[1]].player == board[des[0]][des[1]].player)
        return true;
    return false;
}
bool is_valid_destination_move(piece** board, int src[], int des[])
{
    if (!is_killing_its_own_man(board, src, des) && move_according_to_rule(board, src, des) && !is_piece_over_other(board, src, des))
        return true;
    return false;
}
//...................................................................

void printP(int row, int col, int S, char sym, int color)
{
    gotoRowCol(row, col);
    print_horline(row - S, col - S / 2, col + S / 2, sym, color);
    print_horline(row, col - S / 2, col + S / 2, sym, color);
    print_verline(col - S / 2, row - S, row + S, sym, color);
    print_verline(col + S / 2, row - S, row, sym, color);
}
void printR(int row, int col, int S, char sym, int color)
{
    gotoRowCol(row, col);
    print_horline(row - S, col - S / 2, col + S / 2, sym, color);
    print_horline(row, col - S / 2, col + S / 2, sym, color);
    print_verline(col - S / 2, row - S, row + S, sym, color);
    print_verline(col + S / 2, row - S, row, sym, color);
    for (int i = 1; i <= S; i++)
    {
        gotoRowCol(row + i, col + i - 1);
        cout << sym;
    }
}
void printB(int row, int col, int S, char sym, int color)
{
    gotoRowCol(row, col);
    print_horline(row - S, col - S / 2, col + S / 2, sym, color);
    print_horline(row, col - S / 2, col + S / 2, sym, color);
    print_horline(row + S, col - S / 2, col + S / 2, sym, color);
    print_verline(col - S / 2, row - S, row + S, sym, color);
    print_verline(col + S / 2, row - S, row + S, sym, color);

}
void printK(int row, int col, int S, char sym, int color)
{
    gotoRowCol(row, col);
    print_verline(col - S / 2, row - S, row + S, sym, color);
    for (int i = 1; i <= S; i++)
    {
        gotoRowCol(row + i, col + i - 1);
        cout << sym;
    }
    for (int i = 1; i <= S; i++)
    {
        gotoRowCol(row - i, col + i - 1);
        cout << sym;
    }
    gotoRowCol(row, col);
    cout << sym;
}
void printG(int row, int col, int S, char sym, int color)
{

    print_horline(row - S, col - S / 2, col + S / 2, sym, color);
    print_horline(row + S, col - S / 2, col + S / 2, sym, color);
    print_verline(col - S / 2, row - S, row + S, sym, color);
    print_verline(col + S / 2, row, row + S, sym, color);
    gotoRowCol(row, col + 2);
    cout << sym;
}
void printS(int row, int col, int S, char sym, int color)
{
    gotoRowCol(row, col);
    print_horline(row - S, col - S / 2, col + S / 2, sym, color);
    print_horline(row, col - S / 2, col + S / 2, sym, color);
    print_horline(row + S, col - S / 2, col + S / 2, sym, color);
    print_verline(col - S / 2, row - S, row, sym, color);
    print_verline(col + S / 2, row, row + S, sym, color);

}
void printN(int row, int col, int S, char sym, int color)
{
    gotoRowCol(row, col);
    print_verline(col - S / 2 - 1, row - S, row + S, sym, color);
    print_verline(col + S / 2 + 1, row - S, row + S, sym, color);
    for (int i = 1; i <= S; i++)
    {
        gotoRowCol(row + i - 1, col + i - 1);
        cout << sym;
    }
    for (int i = 1; i <= S; i++)
    {
        gotoRowCol(row - i, col - i);
        cout << sym;
    }
}
void printL(int row, int col, int S, char sym, int color)
{
    gotoRowCol(row, col);
    print_horline(row + S, col - S / 2, col + S / 2, sym, color);
    print_verline(col - S / 2, row - S, row + S, sym, color);

}
//............................................................................
void box_print(int r, int c, int size, int color)
{
    SetClr(color);
    char a = -37;
    for (int i = 0; i < size; i++)
    {
        gotoRowCol(r + i, c);
        for (int j = 1; j <= size; j++)
        {
            cout << a;
        }
    }
}
void print_on_piece(piece** board, int x1, int y1, int x, int y)
{
    int c = 0;
    int set = 1;
    if (board[x1][y1].player == 0)
    {
        set = -1;
    }
    if (board[x1][y1].power == 1)
        c = 4;
    if (board[x1][y1].symbol == 'K')
        printK((x + x + box_size) / 2 - set, (y + y + box_size) / 2, 2, -37, c);
    else if (board[x1][y1].symbol == 'G')
        printG((x + x + box_size) / 2 - set, (y + y + box_size) / 2, 2, -37, c);
    else if (board[x1][y1].symbol == 'S')
        printS((x + x + box_size) / 2 - set, (y + y + box_size) / 2, 2, -37, c);
    else if (board[x1][y1].symbol == 'N')
        printN((x + x + box_size) / 2 - set, (y + y + box_size) / 2, 2, -37, c);
    else if (board[x1][y1].symbol == 'L')
        printL((x + x + box_size) / 2 - set, (y + y + box_size) / 2, 2, -37, c);
    else if (board[x1][y1].symbol == 'R')
        printR((x + x + box_size) / 2 - set, (y + y + box_size) / 2, 2, -37, c);
    else if (board[x1][y1].symbol == 'B')
        printB((x + x + box_size) / 2 - set, (y + y + box_size) / 2, 2, -37, c);
    else if (board[x1][y1].symbol == 'P')
        printP((x + x + box_size) / 2 - set, (y + y + box_size) / 2, 2, -37, c);
}
void print_in_box(int x1, int y1, int x, int y, piece** board)
{
    SetClr(6);

    char a = -37;
    if (board[x1][y1].player == 0)
    {
        for (int i = 0; i < box_size / 2; i++)
        {
            gotoRowCol(x + 1 + i, (y + y + box_size) / 2 - i);
            for (int j = 0; j <= i * 2; j++)
            {

                cout << a;
            }
        }
        for (int i = box_size / 2; i < box_size - 1; i++)
        {
            gotoRowCol(x + 1 + i, y + 1);
            for (int j = 0; j < 7; j++)
            {
                cout << a;
            }
        }

        print_on_piece(board, x1, y1, x, y);
    }
    else if (board[x1][y1].player == 1)
    {
        for (int i = 1; i < box_size / 2; i++)
        {
            gotoRowCol(x + i, y + 1);
            for (int j = 0; j < 7; j++)
            {
                cout << a;
            }
        }
        for (int i = 0; i < box_size / 2; i++)
        {
            gotoRowCol(x + box_size / 2 + i, y + 1 + i);
            for (int j = box_size - 1; j > i * 2; j--)
            {

                cout << a;
            }
        }
        print_on_piece(board, x1, y1, x, y);
    }

}
void printing_pieces_on_board(piece** board)
{
    int box_size = 8;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            int x, y;
            x = upper_boundary + (box_size * i);
            y = left_boundary + (box_size * j);
            print_in_box(i, j, x, y, board);
        }
    }
}

void print_grid()
{
    int bound_col = 8;
    for (int i = 0; i <= 9; i++)
    {
        print_verline(i * box_size + left_boundary, upper_boundary, upper_boundary + (box_size * 9) - 1, -37, 0);

    }
    for (int i = 0; i <= 9; i++)
    {
        print_horline(i * box_size + upper_boundary, left_boundary, left_boundary + (box_size * 9) - 1, -37, 0);
    }
}
void print_undo()
{
    box_print(upper_boundary, left_boundary + box_size * 11, box_size, 6);
    SetClr(0);
    for (int i = 7; i >= 0; i--)
    {
        gotoRowCol(upper_boundary + 4, left_boundary + box_size * 11 + i);
        cout << char(-37);
    }
    for (int i = 0; i < 4; i++)
    {
        gotoRowCol(upper_boundary + 4 - i, left_boundary + box_size * 11 + i);
        cout << char(-37);
    }
    for (int i = 0; i < 4; i++)
    {
        gotoRowCol(upper_boundary + 4 + i, left_boundary + box_size * 11 + i);
        cout << char(-37);
    }

}


void board_print(piece** board)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            box_print(i * box_size + upper_boundary, j * box_size + left_boundary, box_size, 3);
        }
    }
    print_grid();
    print_undo();
}


//..................................................................................
void display_board(piece** board, int kills[], piece** killed)
{
    int upper_boundary = 5;
    int left_boundary = 20;
    board_print(board);
    printing_pieces_on_board(board);
}
void display_killed(piece** killed, int kills[])
{
    int x, y;
    SetClr(7);
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < kills[j]; i++)
        {

            x = upper_boundary + (i % 9) * box_size;
            if (i < 9)
                y = left_boundary - box_size;
            else
                y = left_boundary - box_size * 2;
            if (j == 0)
            {

                if (i < 9)
                {
                    x = upper_boundary + box_size * 8 - i * box_size;
                    y += box_size * 10;
                }
                else
                {
                    x = upper_boundary + box_size * 8 - (i - 9) * box_size;
                    y += box_size * 12;
                }

            }
            print_in_box(j, i, x, y, killed);
        }
    }

}
//........................................................
void display_whole_game(piece** board, string names[], int kills[], piece** killed)
{
    display_board(board, kills, killed);
    display_killed(killed, kills);
}
//...................................................................
bool is_input_inside_board(int move[])
{
    if (move[0] > upper_boundary && move[0]<upper_boundary + box_size * 9 && move[1]>left_boundary && move[1] < left_boundary + box_size * 9)
        return true;
    return false;
}
bool is_input_for_dropping_on_turn1(int move[], piece** killed, int kills[])
{
    if (move[1] > left_boundary || move[0] < upper_boundary)
        return false;
    if (kills[1] < 9)
    {
        if (move[1] < left_boundary - box_size)
            return false;
        if (move[0] > upper_boundary + kills[1] * box_size)
            return false;
        return true;
    }
    else
    {
        if (move[1] < left_boundary - box_size * 2)
            return false;
        if (move[0] > upper_boundary + box_size * (kills[1] / 9) && move[1] < left_boundary - box_size)
            return false;
        return true;
    }

}
bool is_input_for_dropping_on_turn0(int move[], piece** killed, int kills[])
{
    if (move[1] < left_boundary + box_size * 9 || move[0]>upper_boundary + box_size * 9)
        return false;
    if (kills[1] < 9)
    {
        if (move[1] > left_boundary + box_size * 10)
            return false;
        if (move[0] < (upper_boundary + box_size * 9) - (kills[0] * box_size))
            return false;
        return true;
    }
    else
    {
        if (move[1] > left_boundary + box_size * 11)
            return false;
        if (move[0] < (upper_boundary + box_size * 9) - (box_size * (kills[0] / 9)) && move[1] > left_boundary + (box_size * 9) + box_size)
            return false;
        return true;
    }

}
bool is_input_for_undo(int move[])
{
    if (move[0] > upper_boundary && move[0]<upper_boundary + 10 && move[1]>left_boundary + box_size * 11 && move[1] < left_boundary + box_size * 12)
        return true;
    return false;
}
bool is_right_input(int move[], piece** killed, int kills[], int upper_boundary, int left_boundary, int box_size)
{

    return is_input_inside_board(move) || is_input_for_dropping_on_turn1(move, killed, kills) || is_input_for_dropping_on_turn0(move, killed, kills) || is_input_for_undo(move);

}
void convert_coordinates(int move[], int move_matrix[])
{
    move_matrix[0] = move[0] - 5;
    move_matrix[1] = move[1] - 20;
    move_matrix[0] /= 8;
    move_matrix[1] /= 8;
}
void move_input(int move[], piece** killed, int kills[], int move_matrix[])
{
    do
    {
        getRowColbyLeftClick(move[0], move[1]);
    } while (!is_right_input(move, killed, kills, upper_boundary, left_boundary, box_size));
    convert_coordinates(move, move_matrix);
}
bool can_promote(piece** board, int loc[])
{
    if (board[loc[0]][loc[1]].power == 0)
    {
        if (board[loc[0]][loc[1]].player == 0)
        {
            if (loc[0] <= 2)
                return true;
            return false;
        }
        else
        {
            if (loc[0] >= 6)
                return true;
            return false;
        }
    }
    return false;

}
//......................................................................

void check_kill(piece** board, int src[], int des[], int kills[], piece** killed, int turn)
{
    if (board[des[0]][des[1]].symbol != '-')
    {
        killed[turn][kills[turn]] = board[des[0]][des[1]];
        killed[turn][kills[turn]].power = 0;
        kills[turn]++;
        if (killed[turn][kills[turn] - 1].player == 0)
        {
            killed[turn][kills[turn] - 1].player = 1;
        }
        else
        {
            killed[turn][kills[turn] - 1].player = 0;
        }
    }
}
void place_move(piece** board, int src[], int des[])
{
    piece blank;

    board[des[0]][des[1]] = board[src[0]][src[1]];
    board[src[0]][src[1]] = blank;

}

//.....................................................
void find_king(piece** board, int dimension, int turn, int place[])
{
    int i, k;
    for (i = 0; i < dimension; i++)
    {
        for (k = 0; k < dimension; k++)
        {
            place[0] = i;
            place[1] = k;
            if (board[i][k].symbol == 'K' && board[i][k].player == turn)
            {
                return;
            }
        }
    }
}
bool is_check(piece** board, int dimension, int turn)
{
    int place_of_king[2];
    find_king(board, dimension, (turn + 1) % 2, place_of_king);
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            int src[2];
            src[0] = i;
            src[1] = j;
            if (is_valid_source_move(board, src, turn))
            {
                if (is_valid_destination_move(board, src, place_of_king))
                {
                    return true;
                }
            }
        }
    }
    return false;
}
bool self_check(piece** board, int dimension, int turn)
{
    return is_check(board, dimension, (turn + 1) % 2);
}
//....................................................
piece** copy_board(piece** board, int dimension)
{
    piece** copied_board;
    copied_board = new piece * [dimension];
    for (int i = 0; i < dimension; i++)
    {
        copied_board[i] = new piece[dimension];
    }
    for (int i = 0; i < dimension; i++)
    {
        for (int k = 0; k < dimension; k++)
        {
            copied_board[i][k] = board[i][k];
        }
    }
    return copied_board;
}
//...................................................
bool will_be_in_check(piece** board, int dimension, int src[], int des[], int turn)
{
    piece** copied_board = copy_board(board, dimension);
    place_move(copied_board, src, des);
    if (self_check(copied_board, dimension, turn))
        return true;
    return false;
}
//....................................................

bool is_valid_source_move2(piece** board, int dimension, int src[], int turn)
{
    if (!is_valid_source_move(board, src, turn))
    {
        return false;
    }
    for (int i = 0; i < dimension; i++)
    {
        for (int k = 0; k < dimension; k++)
        {
            int des[2];
            des[0] = i;
            des[1] = k;
            if (is_valid_destination_move(board, src, des) && (!will_be_in_check(board, dimension, src, des, turn)))
            {
                return true;
            }
        }
    }
    return false;
}
void colorify_possible_destinations(piece** board, int turn, int src[])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            int m[2];
            m[0] = i;
            m[1] = j;
            if ((!will_be_in_check(board, dimension, src, m, turn)) && is_valid_destination_move(board, src, m))
            {
                box_print(i * box_size + upper_boundary, j * box_size + left_boundary, box_size, 2);
                int x, y;
                x = upper_boundary + (box_size * i);
                y = left_boundary + (box_size * j);


                print_on_piece(board, i, j, x, y);
            }
        }
    }

    print_grid();
}
void uncolorify_possible_destinations(piece** board, int turn, int src[])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            int m[2];
            m[0] = i;
            m[1] = j;
            if ((!will_be_in_check(board, dimension, src, m, turn)) && is_valid_destination_move(board, src, m))
            {
                box_print(i * box_size + upper_boundary, j * box_size + left_boundary, box_size, 3);
                int x, y;
                x = upper_boundary + (box_size * i);
                y = left_boundary + (box_size * j);


                print_on_piece(board, i, j, x, y);
            }
        }
    }

    print_grid();
}
//.....................................................
bool any_valid_move(piece** board, int dimension, int turn)
{
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            int src[2];
            src[0] = i;
            src[1] = j;
            if (is_valid_source_move2(board, dimension, src, (turn + 1) % 2))
            {
                return true;
            }
        }
    }
    return false;
}
bool check_mate(piece** board, int dimension, int turn)
{
    if (is_check(board, dimension, turn))
    {
        if (any_valid_move(board, dimension, turn))
            return false;
        return true;
    }
    return false;

}
bool Draw(piece** board, int dimension, int turn)
{
    if (!is_check(board, dimension, turn))
    {
        if (any_valid_move(board, dimension, turn))
            return false;
        return true;
    }
    return false;

}
void checking_game_ending(piece** board, int dimension, int turn)
{
    if (check_mate(board, dimension, turn))
    {
        system("cls");
        cout << "CHECKMATE";
        exit(1);
    }
    else if (Draw(board, dimension, turn))
    {
        system("cls");
        cout << "DRAW";
        exit(1);
    }
}
//............................................................
int which_drop_piece_of_player1(int move[])
{
    if (move[1] > left_boundary - box_size)
    {
        return (move[0] - upper_boundary) / box_size;
    }
    else
    {
        return ((move[0] - upper_boundary) / box_size) + 9;
    }
}
int which_drop_piece_of_player0(int move[])
{
    if (move[1] < left_boundary + box_size * (9 + 1))
    {
        return (8 - ((move[0] - upper_boundary) / box_size));
    }
    else
    {
        return (8 - ((move[0] - upper_boundary) / box_size)) + 9;
    }
}
void remove_from_killed(int kills[], piece** killed, int piece_index, int turn)
{
    for (int i = piece_index; killed[turn][i].symbol != '-'; i++)
    {
        killed[turn][i] = killed[turn][i + 1];
    }
    kills[turn]--;
}
bool is_any_unpromoted_pawn(piece** board, int dimension, int des[], int turn)
{
    for (int i = 0; i < dimension; i++)
    {
        if (board[i][des[1]].power == 0 && board[i][des[1]].symbol == 'P' && board[i][des[1]].player == turn)
            return true;
    }
    return false;
}
bool valid_future_move_after_dropping(piece** board, int dimension, piece** killed, int index_of_killed, int des[], int turn)
{
    if (killed[turn][index_of_killed].symbol == 'P' || killed[turn][index_of_killed].symbol == 'L')
    {
        if (turn == 0)
        {
            if (des[0] == 0)
                return false;
            return true;
        }
        else
        {
            if (des[0] == 8)
                return false;
            return true;
        }

    }
    else if (killed[turn][index_of_killed].symbol == 'N')
    {
        if (turn == 0)
        {
            if (des[0] < 2)
                return false;
            return true;
        }
        else
        {
            if (des[0] > 6)
                return false;
            return true;
        }
    }
    return true;
}
bool is_giving_check_mate(piece** board, int dimension, piece** killed, int index_of_killed, int des[], int turn)
{
    piece** copied_board = copy_board(board, dimension);
    copied_board[des[0]][des[1]] = killed[turn][index_of_killed];
    return check_mate(copied_board, dimension, turn);
}
bool valid_drop_location(piece** board, int dimension, piece** killed, int index_of_killed, int des[], int turn)
{
    if (board[des[0]][des[1]].symbol != '-')
        return false;
    if (!valid_future_move_after_dropping(board, dimension, killed, index_of_killed, des, turn))
        return false;
    if (killed[turn][index_of_killed].symbol == 'P')
    {
        if (is_any_unpromoted_pawn(board, dimension, des, turn) || is_giving_check_mate(board, dimension, killed, index_of_killed, des, turn))
            return false;
        return true;
    }
    return true;
}
void colorify_possible_destinations_for_drop(piece** board, piece** killed, int index_of_killed, int turn)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            int m[2];
            m[0] = i;
            m[1] = j;
            if (valid_drop_location(board, dimension, killed, index_of_killed, m, turn))
            {
                box_print(i * box_size + upper_boundary, j * box_size + left_boundary, box_size, 2);
            }
        }
    }
    print_grid();
}
void uncolorify_possible_destinations_for_drop(piece** board, piece** killed, int index_of_killed, int turn)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            int m[2];
            m[0] = i;
            m[1] = j;
            if (valid_drop_location(board, dimension, killed, index_of_killed, m, turn))
            {
                box_print(i * box_size + upper_boundary, j * box_size + left_boundary, box_size, 3);
            }
        }
    }
    print_grid();
}
void drop(piece** board, piece** killed, int kills[], int index_of_killed, int turn, int des[])
{
    board[des[0]][des[1]] = killed[turn][index_of_killed];
    remove_from_killed(kills, killed, index_of_killed, turn);
}
//......................................................................
void save(piece** B, int turn, int kills[], piece** killed, fstream& File)
{

    File << turn << " ";
    File << endl;
    //.................
    File << kills[0] << " ";
    File << kills[1] << " ";
    File << endl;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < kills[i]; j++)
        {
            File << killed[i][j].symbol << " ";
            File << endl;
        }
    }
    //.................
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            File << B[i][j].symbol << " ";
            File << B[i][j].player << " ";
            File << B[i][j].power << " ";
        }
        File << endl;
    }
}
void resume(piece** board, int& turn, int kills[], piece** killed)
{
    ifstream In("BSCS22048-currstate.txt");
    In >> turn;
    //.................
    In >> kills[0];
    In >> kills[1];
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < kills[i]; j++)
        {
            In >> killed[i][j].symbol;
            killed[i][j].player = i;
        }
    }
    //.................
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            In >> board[i][j].symbol;
            In >> board[i][j].player;
            In >> board[i][j].power;
        }
    }

}
void undo(piece** board, int& turn, int kills[], piece** killed)
{
    ifstream In("BSCS22048-laststate.txt");
    In >> turn;
    //.................
    In >> kills[0];
    In >> kills[1];
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < kills[i]; j++)
        {
            In >> killed[i][j].symbol;
            killed[i][j].player = i;
        }
    }
    //.................
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            In >> board[i][j].symbol;
            In >> board[i][j].player;
            In >> board[i][j].power;
        }
    }

}
//......................................................................
void promote(piece** board, piece** killed, int kills[], int des_matrix[], int src_matrix[], int turn, int des[])
{
    if (des_matrix[0] == 0 || des_matrix[0] == 8)
    {
        board[des_matrix[0]][des_matrix[1]].power = 1;
        int x, y;
        x = upper_boundary + (box_size * src_matrix[0]);
        y = left_boundary + (box_size * src_matrix[1]);
        print_on_piece(board, src_matrix[0], src_matrix[1], x, y);
        checking_game_ending(board, dimension, turn);
    }
    else
    {
        move_input(src_matrix, killed, kills, src_matrix);

        if (src_matrix[0] == des_matrix[0] && src_matrix[1] == des_matrix[1])
        {
            move_input(des, killed, kills, des_matrix);

            if (src_matrix[0] == des_matrix[0] && src_matrix[1] == des_matrix[1])
            {
                board[src_matrix[0]][src_matrix[1]].power = 1;
                int x, y;
                x = upper_boundary + (box_size * src_matrix[0]);
                y = left_boundary + (box_size * src_matrix[1]);
                print_on_piece(board, src_matrix[0], src_matrix[1], x, y);
                checking_game_ending(board, dimension, turn);
            }
        }
    }
}
int main()
{
    int turn = 0;
    string names[2];
    int kills[2]{};
    piece** killed;
    killed = new piece * [2];
    killed[0] = new piece[19]{};
    killed[1] = new piece[19]{};
    int index_of_killed;

    //.........................
    piece** board;
    initialize_board(board, dimension);

    int a;
    cout << "Do you want to resume? Enter 1 for it.";
    cin >> a;
    if (a == 1)
    {
        resume(board, turn, kills, killed);
    }
    else
    {

        input_players_name(names);
    }
    //###############################################################################  
    int src[2]{};
    int des[2]{};
    int src_matrix[2]{};
    int des_matrix[2]{};

    display_whole_game(board, names, kills, killed);
    fstream File("curr_state.txt");
    save(board, turn, kills, killed, File);
    while (true)
    {

        //..................................................
        do
        {

            /////////////////////////////////////////////////////////            
            move_input(src, killed, kills, src_matrix);
            /////////////////////////////////////////////////////////
            if (is_input_for_undo(src))
            {
                undo(board, turn, kills, killed);
                system("cls");
                display_whole_game(board, names, kills, killed);
                break;
            }
            else if (is_input_for_dropping_on_turn1(src, killed, kills) && turn == 1)
            {
                index_of_killed = which_drop_piece_of_player1(src);
                colorify_possible_destinations_for_drop(board, killed, index_of_killed, turn);
                move_input(des, killed, kills, des_matrix);
                if (valid_drop_location(board, dimension, killed, index_of_killed, des_matrix, turn))
                {
                    fstream Filelast("laststate.txt");
                    save(board, (turn + 1) % 2, kills, killed, Filelast);

                    drop(board, killed, kills, index_of_killed, turn, des_matrix);
                    system("cls");
                    display_whole_game(board, names, kills, killed);
                    break;
                }
                else
                {
                    uncolorify_possible_destinations_for_drop(board, killed, index_of_killed, turn);
                }
            }
            else if (is_input_for_dropping_on_turn0(src, killed, kills) && turn == 0)
            {

                index_of_killed = which_drop_piece_of_player0(src);
                colorify_possible_destinations_for_drop(board, killed, index_of_killed, turn);
                move_input(des, killed, kills, des_matrix);
                if (valid_drop_location(board, dimension, killed, index_of_killed, des_matrix, turn))
                {
                    fstream Filelast("laststate.txt");
                    save(board, (turn + 1) % 2, kills, killed, Filelast);

                    drop(board, killed, kills, index_of_killed, turn, des_matrix);
                    system("cls");
                    display_whole_game(board, names, kills, killed);
                    break;
                }
                else
                {
                    uncolorify_possible_destinations_for_drop(board, killed, index_of_killed, turn);
                }
            }
            else if (is_input_inside_board(src))
            {
                if (is_valid_source_move2(board, dimension, src_matrix, turn))
                {
                    colorify_possible_destinations(board, turn, src_matrix);
                    move_input(des, killed, kills, des_matrix);
                    if (is_valid_destination_move(board, src_matrix, des_matrix) && !will_be_in_check(board, dimension, src_matrix, des_matrix, turn))
                    {
                        fstream Filelast("laststate.txt");
                        save(board, (turn + 1) % 2, kills, killed, Filelast);
                        check_kill(board, src_matrix, des_matrix, kills, killed, turn);
                        place_move(board, src_matrix, des_matrix);
                        display_whole_game(board, names, kills, killed);
                        checking_game_ending(board, dimension, turn);
                        if (can_promote(board, des_matrix))
                        {
                            promote(board, killed, kills, des_matrix, src_matrix, turn, des);
                        }
                        break;
                    }
                    else
                    {
                        
                            uncolorify_possible_destinations(board, turn, src_matrix);
                    }

                }

            }
        } while (1);
        turn++;
        turn = turn % 2;
        fstream File("currstate.txt");
        save(board, turn, kills, killed, File);
    }

    return 0;
}