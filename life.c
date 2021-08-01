#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#ifdef _WIN32
#    include <Windows.h>
#else
#    include <unistd.h>
#endif

// For technical reasons this has to be a power of 2
#define BOARD_SIZE 64

void fillBoard(uint8_t *, uint16_t, uint32_t);
void fillBoardTerminate(uint8_t *, uint16_t, uint32_t);

void printBoard(uint8_t *, uint16_t, char, char, uint32_t);
void printBoardTerminate(uint8_t *, uint16_t, char, char, uint32_t);

void stepBoard(uint8_t *, uint16_t, uint32_t);
void stepBoardTerminate(uint8_t *, uint16_t, uint32_t);

void (*fillBoardFunctions[2])(uint8_t *, uint16_t, uint32_t) = {&fillBoard, &fillBoardTerminate};
void (*printBoardFunctions[2])(uint8_t *, uint16_t, char, char, uint32_t) = {&printBoard, &printBoardTerminate};
void (*stepBoardFunctions[2])(uint8_t *, uint16_t, uint32_t) = {&stepBoard, &stepBoardTerminate};

int main() {
    srand(time(NULL));

    uint8_t *board = (uint8_t *)malloc(sizeof(uint8_t) * BOARD_SIZE * BOARD_SIZE);

    // Fill the board with a random initial state
    fillBoard(board, BOARD_SIZE, 0);

TICK:
    printBoard(board, BOARD_SIZE, '#', ' ', 0);
    stepBoard(board, BOARD_SIZE, 0);
    usleep(100000);
    goto TICK;

    return 0;
}

void fillBoard(uint8_t *board, uint16_t boardSize, uint32_t currentIdx) {
    uint16_t cellStateSeed = rand();
    board[currentIdx] = (cellStateSeed & (cellStateSeed >> 1)) & 0x1;

    uint32_t nextFuncIdx = ((currentIdx + 1) & (boardSize * boardSize)) - (boardSize * boardSize);
    nextFuncIdx = ~(nextFuncIdx >> 31) & 0x1;

    fillBoardFunctions[nextFuncIdx](board, boardSize, currentIdx + 1);
}

void fillBoardTerminate(uint8_t *board, uint16_t boardSize, uint32_t currentIdx) {
    return;
}

void printBoard(uint8_t *board, uint16_t boardSize, char liveCell, char deadCell, uint32_t currentIdx) {
    uint8_t cellState = board[currentIdx] & 0x1;

    printf("%c%c", (&deadCell)[cellState], (&deadCell)[cellState]);

    // If this is one less than a multiple of the board size, print a newline
    char *newLine = "\n";
    uint32_t endLineIdx = ((currentIdx % boardSize) + 1) - boardSize;
    endLineIdx = endLineIdx >> 31;

    // printf("Index is %d, printing starting at character %d\n", currentIdx, endLineIdx);

    printf("%s", newLine + endLineIdx);

    uint32_t nextFuncIdx = ((currentIdx + 1) & (boardSize * boardSize)) - (boardSize * boardSize);
    nextFuncIdx = ~(nextFuncIdx >> 31) & 0x1;

    printBoardFunctions[nextFuncIdx](board, boardSize, liveCell, deadCell, currentIdx + 1);
}

void printBoardTerminate(uint8_t *board, uint16_t boardSize, char deadCell, char liveCell, uint32_t currentIdx) {
    printf("\033[H");
    fflush(stdout);
    return;
}

void stepBoard(uint8_t *board, uint16_t boardSize, uint32_t currentIdx) {
    // Get the row and column of the current cell
    uint16_t row = currentIdx / boardSize;
    uint16_t col = currentIdx % boardSize;

    // Get the neighbors of the current cell
    uint8_t neighbors = 0;
    neighbors += board[((col - 1) & (boardSize - 1)) + ((row - 1) & (boardSize - 1)) * boardSize] & 0x1;
    neighbors += board[((col - 1) & (boardSize - 1)) + row * boardSize] & 0x1;
    neighbors += board[((col - 1) & (boardSize - 1)) + ((row + 1) & (boardSize - 1)) * boardSize] & 0x1;
    neighbors += board[col + ((row - 1) & (boardSize - 1)) * boardSize] & 0x1;
    neighbors += board[col + ((row + 1) & (boardSize - 1)) * boardSize] & 0x1;
    neighbors += board[((col + 1) & (boardSize - 1)) + ((row - 1) & (boardSize - 1)) * boardSize] & 0x1;
    neighbors += board[((col + 1) & (boardSize - 1)) + row * boardSize] & 0x1;
    neighbors += board[((col + 1) & (boardSize - 1)) + ((row + 1) & (boardSize - 1)) * boardSize] & 0x1;

    // Store the next state of the current cell
    board[currentIdx] |= (~neighbors >> 1 & neighbors & ((board[currentIdx] & 0x1) | neighbors) << 1) & 0x2;

    uint32_t nextFuncIdx = ((currentIdx + 1) & (boardSize * boardSize)) - (boardSize * boardSize);
    nextFuncIdx = ~(nextFuncIdx >> 31) & 0x1;

    // Call the appropriate function
    stepBoardFunctions[nextFuncIdx](board, boardSize, currentIdx + 1);

    // Update the current cell index
    board[currentIdx] >>= 1;
}

void stepBoardTerminate(uint8_t *board, uint16_t boardSize, uint32_t currentIdx) {
    return;
}