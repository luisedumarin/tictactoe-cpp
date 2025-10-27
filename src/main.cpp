#include <iostream>
#include <limits>
#include <random>
#include <fstream>

namespace
{

    constexpr int kBoardSize = 3;

    // Limpia el estado de cin ante entradas invalidas.
    void clearInput()
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void resetBoard(char board[kBoardSize][kBoardSize])
    {
        for (int row = 0; row < kBoardSize; ++row)
        {
            for (int col = 0; col < kBoardSize; ++col)
            {
                board[row][col] = ' ';
            }
        }
    }

    // Dibuja el tablero mostrando indices disponibles.
    void printBoard(const char board[kBoardSize][kBoardSize])
    {
        std::cout << "\n";
        for (int row = 0; row < kBoardSize; ++row)
        {
            for (int col = 0; col < kBoardSize; ++col)
            {
                const int cellIndex = row * kBoardSize + col + 1;
                const char cellValue = board[row][col] == ' ' ? static_cast<char>('0' + cellIndex) : board[row][col];
                std::cout << " " << cellValue << " ";
                if (col < kBoardSize - 1)
                {
                    std::cout << "|";
                }
            }
            std::cout << "\n";
            if (row < kBoardSize - 1)
            {
                std::cout << "-----------\n";
            }
        }
        std::cout << "\n";
    }

    bool isMoveValid(const char board[kBoardSize][kBoardSize], int row, int col)
    {
        if (row < 0 || row >= kBoardSize || col < 0 || col >= kBoardSize)
        {
            return false;
        }
        return board[row][col] == ' ';
    }

    bool hasWinner(const char board[kBoardSize][kBoardSize], char player)
    {
        for (int row = 0; row < kBoardSize; ++row)
        {
            if (board[row][0] == player && board[row][1] == player && board[row][2] == player)
            {
                return true;
            }
        }
        for (int col = 0; col < kBoardSize; ++col)
        {
            if (board[0][col] == player && board[1][col] == player && board[2][col] == player)
            {
                return true;
            }
        }
        if (board[0][0] == player && board[1][1] == player && board[2][2] == player)
        {
            return true;
        }
        if (board[0][2] == player && board[1][1] == player && board[2][0] == player)
        {
            return true;
        }
        return false;
    }

    bool isDraw(const char board[kBoardSize][kBoardSize])
    {
        for (int row = 0; row < kBoardSize; ++row)
        {
            for (int col = 0; col < kBoardSize; ++col)
            {
                if (board[row][col] == ' ')
                {
                    return false;
                }
            }
        }
        return true;
    }

    // Solicita una casilla valida al jugador actual.
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

            const int index = selection - 1;
            const int row = index / kBoardSize;
            const int col = index % kBoardSize;

            if (!isMoveValid(board, row, col))
            {
                std::cout << "Casilla ocupada. Elija otra.\n";
                continue;
            }

            return index;
        }
    }

    // Ejecuta una partida completa y retorna el resultado.
    // vsCPU: si es true, la CPU participa. humanIsX: si es true el humano juega con 'X', si no juega con 'O'.
    char playMatch(bool vsCPU = false, bool humanIsX = true)
    {
        char board[kBoardSize][kBoardSize];
        resetBoard(board);
        char currentPlayer = 'X';

        // Setup para CPU
        std::random_device rd;
        std::mt19937 rng(rd());

        auto cpuMove = [&](const char b[kBoardSize][kBoardSize]) -> int
        {
            // Recolecta casillas libres
            int freeCells[9];
            int freeCount = 0;
            for (int i = 0; i < kBoardSize * kBoardSize; ++i)
            {
                int r = i / kBoardSize;
                int c = i % kBoardSize;
                if (b[r][c] == ' ')
                {
                    freeCells[freeCount++] = i;
                }
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
                // Determinar si es el turno de la CPU
                char cpuPlayer = humanIsX ? 'O' : 'X';
                if (currentPlayer == cpuPlayer)
                {
                    std::cout << "Turno de la CPU (" << cpuPlayer << ")..." << std::endl;
                    index = cpuMove(board);
                    if (index == -1)
                    {
                        // no hay movimientos
                        index = 0;
                    }
                }
                else
                {
                    index = readCellSelection(currentPlayer, board);
                }
            }
            else
            {
                index = readCellSelection(currentPlayer, board);
            }
            const int row = index / kBoardSize;
            const int col = index % kBoardSize;
            board[row][col] = currentPlayer;

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

    // Mejora opcional: tablero de marcadores persistente entre partidas.
    void printScoreboard(int xWins, int oWins, int draws)
    {
        std::cout << "\nMarcador global\n";
        std::cout << "----------------\n";
        std::cout << "Jugador X: " << xWins << "\n";
        std::cout << "Jugador O: " << oWins << "\n";
        std::cout << "Empates:   " << draws << "\n\n";
    }

    // Guarda el marcador en un archivo simple: tres enteros separados por espacios.
    void saveScoreboardToFile(const std::string &path, int xWins, int oWins, int draws)
    {
        std::ofstream out(path, std::ios::trunc);
        if (!out)
            return;
        out << xWins << ' ' << oWins << ' ' << draws << '\n';
    }

    // Intenta cargar el marcador; si falla deja los valores en 0.
    void loadScoreboardFromFile(const std::string &path, int &xWins, int &oWins, int &draws)
    {
        std::ifstream in(path);
        if (!in)
            return;
        in >> xWins >> oWins >> draws;
        if (!in)
        {
            // si la lectura falla, resetear a 0
            xWins = oWins = draws = 0;
        }
    }

    int readMenuOption()
    {
        while (true)
        {
            std::cout << "Menu principal\n";
            std::cout << "1. Jugar una partida\n";
            std::cout << "2. Ver marcador\n";
            std::cout << "3. Salir\n";
            std::cout << "Opcion: ";

            int option = 0;
            if (!(std::cin >> option))
            {
                std::cout << "Entrada invalida. Intente nuevamente.\n\n";
                clearInput();
                continue;
            }

            if (option < 1 || option > 3)
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

    int xWins = 0;
    int oWins = 0;
    int draws = 0;

    // Cargar marcador previo si existe
    const std::string scoreboardPath = "scoreboard.txt";
    loadScoreboardFromFile(scoreboardPath, xWins, oWins, draws);

    bool shouldExit = false;
    while (!shouldExit)
    {
        const int option = readMenuOption();

        switch (option)
        {
        case 1:
        {
            // Preguntar si quiere jugar contra la CPU
            bool vsCPU = false;
            bool humanIsX = true;
            std::cout << "Jugar contra CPU? (s/n): ";
            char yn;
            if (std::cin >> yn)
            {
                if (yn == 's' || yn == 'S')
                {
                    vsCPU = true;
                    // Preguntar si el humano desea empezar
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
            }

            const char result = playMatch(vsCPU, humanIsX);
            if (result == 'X')
            {
                ++xWins;
            }
            else if (result == 'O')
            {
                ++oWins;
            }
            else
            {
                ++draws;
            }
            break;
        }
        case 2:
            printScoreboard(xWins, oWins, draws);
            break;
        case 3:
            shouldExit = true;
            break;
        default:
            break;
        }
    }

    // Guardar marcador antes de salir
    saveScoreboardToFile(scoreboardPath, xWins, oWins, draws);

    std::cout << "Gracias por jugar. Hasta luego.\n";
    return 0;
}
