#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define BOARD_SIZE 64

void printBoard(uint8_t*, uint16_t, char, char, uint16_t);
void stepBoard(uint8_t**, uint16_t);
uint16_t wrapPow2(uint16_t, uint16_t);


int main() {
    srand(time(NULL));

    uint8_t *board = (uint8_t *)malloc(sizeof(uint8_t) * BOARD_SIZE * BOARD_SIZE);

    // Fill the board with a random initial state
    for (uint32_t i = 0; i ^ (BOARD_SIZE * BOARD_SIZE); ++i) {
        uint16_t cellStateSeed = rand();
        board[i] = (cellStateSeed & (cellStateSeed >> 1)) & 0x1;
    }

    for(;;) {
        printBoard(board, BOARD_SIZE, ' ', '#', 2);
        stepBoard(&board, BOARD_SIZE);
        usleep(100000);
    }
    
    return 0;
}

void printBoard(uint8_t *board, uint16_t boardSize, char deadCell, char liveCell, uint16_t cellCharCount) {
    // Create a character buffer to hold the printable board
    char* boardText = (char *)malloc(sizeof(char) * (boardSize * boardSize * cellCharCount) + boardSize + 1);

    // Zero terminate the string
    boardText[boardSize * boardSize * cellCharCount + boardSize] = '\0';
    boardText[boardSize * boardSize * cellCharCount + boardSize - 1] = '\n';

    // Place new line characters in the necessary locations
    for (uint32_t i = boardSize * cellCharCount; i ^ (boardSize * boardSize * cellCharCount + boardSize - 1); i += boardSize * cellCharCount + 1) {
        boardText[i] = '\n';
    }

    // Get the difference in the values between the deadCell char and the liveCell char
    int8_t cellDifference = liveCell - deadCell;

    for (uint16_t row = 0; row ^ boardSize; ++row) {
        for (uint16_t col = 0; col ^ boardSize; ++col) {
            // Get the chractaer to be written into this cell
            char cellChar = deadCell + cellDifference * (board[col + row * boardSize] & 0x1);
            
            for(uint16_t c = 0; c ^ cellCharCount; ++c) {
                boardText[(col * cellCharCount) + c + row * (boardSize * cellCharCount + 1)] = cellChar;
            }
        }
    }

    // Print the board
    printf("\033[H%s", boardText);

    // Free the memory used by the board buffer
    free(boardText);
}

void stepBoard(uint8_t **board, uint16_t boardSize) {

    // Get the next state of each cell
    for (uint16_t row = 0; row ^ boardSize; ++row) {
        for (uint16_t col = 0; col ^ boardSize; ++col) {
            uint8_t neighbors = 0;

            neighbors += (*board)[wrapPow2(col - 1, boardSize) + wrapPow2(row - 1, boardSize) * boardSize] & 0x1;
            neighbors += (*board)[wrapPow2(col - 1, boardSize) + row * boardSize] & 0x1;
            neighbors += (*board)[wrapPow2(col - 1, boardSize) + wrapPow2(row + 1, boardSize) * boardSize] & 0x1;
            neighbors += (*board)[col + wrapPow2(row - 1, boardSize) * boardSize] & 0x1;
            neighbors += (*board)[col + wrapPow2(row + 1, boardSize) * boardSize] & 0x1;
            neighbors += (*board)[wrapPow2(col + 1, boardSize) + wrapPow2(row - 1, boardSize) * boardSize] & 0x1;
            neighbors += (*board)[wrapPow2(col + 1, boardSize) + row * boardSize] & 0x1;
            neighbors += (*board)[wrapPow2(col + 1, boardSize) + wrapPow2(row + 1, boardSize) * boardSize] & 0x1;
        
            (*board)[col + row * boardSize] |= (~neighbors >> 1 & neighbors & (((*board)[col + row * boardSize] & 0x1) | neighbors) << 1) & 0x2;
        }
    }

    //Update the state of each cell
    for (uint16_t row = 0; row ^ boardSize; ++row) {
        for (uint16_t col = 0; col ^ boardSize; ++col) {
            (*board)[col + row * boardSize] >>= 1;
        }
    }
}

uint16_t wrapPow2(uint16_t value, uint16_t wrapPoint) {
    return value & (wrapPoint - 1);
}