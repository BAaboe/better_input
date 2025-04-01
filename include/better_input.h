#ifndef ALOFC_H
#define ALOFC_H

#define MOVE_LEFT "\033[D"
#define MOVE_RIGHT "\033[C"

#define NEW_LINE "\r\n"

#define CTRL_A 1
#define CTRL_C 3
#define ENTER 13
#define ESCAPE 27
#define CTRL_SEQ 91
#define BACKSPACE 127

#define LEFT 68
#define RIGHT 67
#define UP 66
#define DOWN 64

#define BELL "\x07"

#define pr_raw_str(f) write(STDOUT_FILENO, f, strlen(f))
#define pr_raw_char(f) write(STDOUT_FILENO, f, 1)

/**
 * @brief Gives you an input from the terminal.
 * @param buffer: this is the string where the input will be stored.
 * @param size: the size of the buffer
 * @returns the buffer param
 *
 * @note Currently it does not really handle errors, so use at own risk.
 * */

char* get_input(char* buffer, int size);

#endif // !ALOFC_H
