# Tic Tac Toe (C++)
// Versión mejorada: incluye modo CPU (aleatorio o inteligente con Minimax),
// marcador persistente en disco y opciones para reiniciar marcador.
//
// Estructura principal:
// - funciones utilitarias para manejo del tablero e input
// - implementación de Minimax para la CPU "inteligente"
// - menú interactivo en `main()` que guarda el marcador en `scoreboard.txt`
//
// Notas de uso:
// - Compilar con C++17: g++ -std=c++17 -Wall -Wextra -pedantic -o bin/tictactoe src/main.cpp
// - Ejecutar: ./bin/tictactoe
//
#include <iostream>
#include <limits>
#include <random>
#include <fstream>
#include <algorithm>
#include <cstdlib>

namespace
{

    constexpr int kBoardSize = 3;

    // Limpia el estado de std::cin tras una entrada inválida para evitar bucles
    // infinitos cuando el usuario escribe texto donde se espera un entero.
    void clearInput()
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Inicializa todas las casillas del tablero a espacio ' ' (vacío).
    void resetBoard(char board[kBoardSize][kBoardSize])
    {
        for (int r = 0; r < kBoardSize; ++r)
            for (int c = 0; c < kBoardSize; ++c)
                board[r][c] = ' ';
    }

    // Dibuja el tablero en consola.
    // Las casillas vacías muestran su índice (1..9) para facilitar la selección.
    // Usa códigos ANSI para limpiar la pantalla en la mayoría de terminales.
    void printBoard(const char board[kBoardSize][kBoardSize])
    {
        std::cout << "\033[2J\033[H"; // clear pantalla (ANSI)
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

    // Comprueba que la fila/columna estén dentro del tablero y que la casilla
    // esté vacía (' '). Devuelve true si la jugada es válida.
    bool isMoveValid(const char board[kBoardSize][kBoardSize], int row, int col)
    {
        return row >= 0 && row < kBoardSize && col >= 0 && col < kBoardSize && board[row][col] == ' ';
    }

    // Comprueba si `player` (X u O) tiene tres en raya en filas, columnas o diagonales.
    bool hasWinner(const char board[kBoardSize][kBoardSize], char player)
    {
        // Filas y columnas
        for (int i = 0; i < kBoardSize; ++i)
            if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
                return true;
        for (int i = 0; i < kBoardSize; ++i)
            if (board[0][i] == player && board[1][i] == player && board[2][i] == player)
                return true;
        // Diagonales principales
        if (board[0][0] == player && board[1][1] == player && board[2][2] == player)
            return true;
        if (board[0][2] == player && board[1][1] == player && board[2][0] == player)
            return true;
        return false;
    }

    // Devuelve true si no quedan casillas libres (empate).
    bool isDraw(const char board[kBoardSize][kBoardSize])
    {
        for (int r = 0; r < kBoardSize; ++r)
            for (int c = 0; c < kBoardSize; ++c)
                if (board[r][c] == ' ')
                    return false;
        return true;
    }

    // Minimax
    // Evaluador recursivo que asigna una puntuación a los estados de tablero.
    // - isMax: true cuando el nodo actual representa el turno del CPU (maximizador).
    // - cpuPlayer / humanPlayer: símbolos ('X' o 'O') usados para evaluar ganador.
    // - depth: profundidad actual para preferir victorias más rápidas.
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

    // Calcula la mejor jugada para el CPU usando Minimax.
    // Devuelve el índice (0..8) de la casilla recomendada o -1 si no hay movimientos.
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

