// Tic Tac Toe mejorado: CPU con Minimax opcional + reset marcador
#include <iostream>
#include <limits>
#include <random>
#include <fstream>
#include <algorithm>
#include <cstdlib>

namespace
{

    constexpr int kBoardSize = 3;

    void clearInput()
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void resetBoard(char board[kBoardSize][kBoardSize])
    {
        for (int r = 0; r < kBoardSize; ++r)
            for (int c = 0; c < kBoardSize; ++c)
                board[r][c] = ' ';
    }

    void printBoard(const char board[kBoardSize][kBoardSize])
    {
        std::cout << "\033[2J\033[H"; // clear
        std::cout << "\n";
        for (int r = 0; r < kBoardSize; ++r)
        {
            for (int c = 0; c < kBoardSize; ++c)
            {
                int idx = r * kBoardSize + c + 1;
                char v = board[r][c] == ' ' ? static_cast<char>('0' + idx) : board[r][c];
                std::cout << ' ' << v << ' ';
                if (c < kBoardSize - 1)
                    std::cout << '|';
            }
            std::cout << '\n';
            if (r < kBoardSize - 1)
                std::cout << "-----------\n";
        }
        std::cout << '\n';
    }

    bool isMoveValid(const char board[kBoardSize][kBoardSize], int row, int col)
    {
        return row >= 0 && row < kBoardSize && col >= 0 && col < kBoardSize && board[row][col] == ' ';
    }

    bool hasWinner(const char board[kBoardSize][kBoardSize], char player)
    {
        for (int i = 0; i < kBoardSize; ++i)
            if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
                return true;
        for (int i = 0; i < kBoardSize; ++i)
            if (board[0][i] == player && board[1][i] == player && board[2][i] == player)
                return true;
        if (board[0][0] == player && board[1][1] == player && board[2][2] == player)
            return true;
        if (board[0][2] == player && board[1][1] == player && board[2][0] == player)
            return true;
        return false;
    }

    bool isDraw(const char board[kBoardSize][kBoardSize])
    {
        for (int r = 0; r < kBoardSize; ++r)
            for (int c = 0; c < kBoardSize; ++c)
                if (board[r][c] == ' ')
                    return false;
        return true;
    }

    // Minimax
    int minimax(char board[kBoardSize][kBoardSize], bool isMax, char cpuPlayer, char humanPlayer, int depth)
    {
        if (hasWinner(board, cpuPlayer))
            return 10 - depth;
        if (hasWinner(board, humanPlayer))
            return depth - 10;
        if (isDraw(board))
            return 0;

        if (isMax)
        {
            int best = std::numeric_limits<int>::min();
            for (int i = 0; i < kBoardSize * kBoardSize; ++i)
            {
                int r = i / kBoardSize, c = i % kBoardSize;
                if (board[r][c] == ' ')
                {
                    board[r][c] = cpuPlayer;
                    best = std::max(best, minimax(board, false, cpuPlayer, humanPlayer, depth + 1));
                    board[r][c] = ' ';
                }
            }
            return best;
        }
        else
        {
            int best = std::numeric_limits<int>::max();
            for (int i = 0; i < kBoardSize * kBoardSize; ++i)
            {
                int r = i / kBoardSize, c = i % kBoardSize;
                if (board[r][c] == ' ')
                {
                    board[r][c] = humanPlayer;
                    best = std::min(best, minimax(board, true, cpuPlayer, humanPlayer, depth + 1));
                    board[r][c] = ' ';
                }
            }
            return best;
        }
    }

    int getBestMoveMinimax(char board[kBoardSize][kBoardSize], char cpuPlayer, char humanPlayer)
    {
        int bestVal = std::numeric_limits<int>::min();
        int bestMove = -1;
        for (int i = 0; i < kBoardSize * kBoardSize; ++i)
        {
            int r = i / kBoardSize, c = i % kBoardSize;
            if (board[r][c] == ' ')
            {
                board[r][c] = cpuPlayer;
                int val = minimax(board, false, cpuPlayer, humanPlayer, 0);
                board[r][c] = ' ';
                if (val > bestVal)
                {
                    bestVal = val;
                    bestMove = i;
                }
            }
        }
        return bestMove;
    }

    int readCellSelection(char currentPlayer, const char board[kBoardSize][kBoardSize])
    {
        while (true)
        {
            std::cout << "Jugador " << currentPlayer << ", elija una casilla (1-9): ";
            int selection = 0;
            if (!(std::cin >> selection))
            {
                std::cout << "Entrada invalida. Intente nuevamente.\n";
                clearInput();
                continue;
            }
            if (selection < 1 || selection > kBoardSize * kBoardSize)
            {
                std::cout << "Casilla fuera de rango. Intente nuevamente.\n";
                continue;
            }
            int idx = selection - 1;
            int r = idx / kBoardSize, c = idx % kBoardSize;
            if (!isMoveValid(board, r, c))
            {
                std::cout << "Casilla ocupada. Elija otra.\n";
                continue;
            }
            return idx;
        }
    }

    char playMatch(bool vsCPU = false, bool humanIsX = true, bool cpuSmart = false)
    {
        char board[kBoardSize][kBoardSize];
        resetBoard(board);
        char currentPlayer = 'X';

        std::random_device rd;
        std::mt19937 rng(rd());

        auto cpuMove = [&](const char b[kBoardSize][kBoardSize]) -> int
        {
            char cpuPlayer = humanIsX ? 'O' : 'X';
            char humanPlayer = humanIsX ? 'X' : 'O';
            if (cpuSmart)
            {
                int m = getBestMoveMinimax(const_cast<char (*)[kBoardSize]>(b), cpuPlayer, humanPlayer);
                if (m >= 0)
                    return m;
            }
            int freeCells[9], freeCount = 0;
            for (int i = 0; i < kBoardSize * kBoardSize; ++i)
            {
                int r = i / kBoardSize, c = i % kBoardSize;
                if (b[r][c] == ' ')
                    freeCells[freeCount++] = i;
            }
            if (freeCount == 0)
                return -1;
            std::uniform_int_distribution<int> dist(0, freeCount - 1);
            return freeCells[dist(rng)];
        };

        while (true)
        {
            printBoard(board);
            int index = -1;
            if (vsCPU)
            {
                char cpuPlayer = humanIsX ? 'O' : 'X';
                if (currentPlayer == cpuPlayer)
                {
                    std::cout << "Turno de la CPU (" << cpuPlayer << ")...\n";
                    index = cpuMove(board);
                    if (index == -1)
                        index = 0;
                }
                else
                    index = readCellSelection(currentPlayer, board);
            }
            else
                index = readCellSelection(currentPlayer, board);

            int r = index / kBoardSize, c = index % kBoardSize;
            board[r][c] = currentPlayer;

            if (hasWinner(board, currentPlayer))
            {
                printBoard(board);
                std::cout << "Jugador " << currentPlayer << " gana la partida.\n";
                return currentPlayer;
            }
            if (isDraw(board))
            {
                printBoard(board);
                std::cout << "La partida termina en empate.\n";
                return 'D';
            }
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
    }

    void printScoreboard(int xWins, int oWins, int draws)
    {
        std::cout << "\nMarcador global\n";
        std::cout << "----------------\n";
        std::cout << "Jugador X: " << xWins << "\n";
        std::cout << "Jugador O: " << oWins << "\n";
        std::cout << "Empates:   " << draws << "\n\n";
    }

    void saveScoreboardToFile(const std::string &path, int xWins, int oWins, int draws)
    {
        std::ofstream out(path, std::ios::trunc);
        if (!out)
            return;
        out << xWins << ' ' << oWins << ' ' << draws << '\n';
    }

    void loadScoreboardFromFile(const std::string &path, int &xWins, int &oWins, int &draws)
    {
        std::ifstream in(path);
        if (!in)
            return;
        in >> xWins >> oWins >> draws;
        if (!in)
            xWins = oWins = draws = 0;
    }

    int readMenuOption()
    {
        while (true)
        {
            std::cout << "Menu principal\n";
            std::cout << "1. Jugar una partida\n";
            std::cout << "2. Ver marcador\n";
            std::cout << "3. Reiniciar marcador\n";
            std::cout << "4. Salir\n";
            std::cout << "Opcion: ";
            int option = 0;
            if (!(std::cin >> option))
            {
                std::cout << "Entrada invalida. Intente nuevamente.\n\n";
                clearInput();
                continue;
            }
            if (option < 1 || option > 4)
            {
                std::cout << "Seleccione una opcion valida.\n\n";
                continue;
            }
            return option;
        }
    }

} // namespace

int main()
{
    std::cout << "=== Tic Tac Toe ===\n";
    int xWins = 0, oWins = 0, draws = 0;
    const std::string scoreboardPath = "scoreboard.txt";
    loadScoreboardFromFile(scoreboardPath, xWins, oWins, draws);

    bool shouldExit = false;
    while (!shouldExit)
    {
        int option = readMenuOption();
        switch (option)
        {
        case 1:
        {
            bool vsCPU = false;
            bool humanIsX = true;
            std::cout << "Jugar contra CPU? (s/n): ";
            char yn;
            if (std::cin >> yn && (yn == 's' || yn == 'S'))
            {
                vsCPU = true;
                std::cout << "Deseas empezar como X? (s/n): ";
                char start;
                while (true)
                {
                    if (!(std::cin >> start))
                    {
                        clearInput();
                        continue;
                    }
                    if (start == 's' || start == 'S')
                    {
                        humanIsX = true;
                        break;
                    }
                    if (start == 'n' || start == 'N')
                    {
                        humanIsX = false;
                        break;
                    }
                    std::cout << "Respuesta invalida. Escribe 's' o 'n': ";
                }
            }

            bool cpuSmart = false;
            if (vsCPU)
            {
                std::cout << "Dificultad CPU - aleatoria (r) o inteligente (m): ";
                char diff;
                while (!(std::cin >> diff))
                {
                    clearInput();
                }
                if (diff == 'm' || diff == 'M')
                    cpuSmart = true;
            }

            char result = playMatch(vsCPU, humanIsX, cpuSmart);
            if (result == 'X')
                ++xWins;
            else if (result == 'O')
                ++oWins;
            else
                ++draws;
            break;
        }
        case 2:
            printScoreboard(xWins, oWins, draws);
            break;
        case 3:
            xWins = oWins = draws = 0;
            std::remove(scoreboardPath.c_str());
            std::cout << "Marcador reiniciado.\n";
            break;
        case 4:
            shouldExit = true;
            break;
        default:
            break;
        }
    }

    saveScoreboardToFile(scoreboardPath, xWins, oWins, draws);
    std::cout << "Gracias por jugar. Hasta luego.\n";
    return 0;
}
