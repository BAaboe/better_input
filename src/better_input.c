#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "better_input.h"


// Struct containing the need info for the input
typedef struct input_state{
	char* buffer;
	int pos;
	int num_of_char;
	ssize_t size;
} input_state_t;

struct termios original_state;

// Turns the terminal into "Raw" mode. Will tell the terminal to let us handle almost everything
int enable_raw_mode(){
	if(tcgetattr(STDIN_FILENO, &original_state) == -1){
		return -1;
	}

	struct termios raw = original_state;

	raw.c_iflag &= ~(ICRNL | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);

	fflush(stdout);
	tcsetattr(STDIN_FILENO, TCSADRAIN, &raw);

	return 0;
}

// Turns the terminal back to the normal mode
void disbale_raw_mode(){
	fflush(stdout);
	tcsetattr(STDIN_FILENO, TCSANOW, &original_state);
}

// Shits the given buffer one byte to the right
// @note: There must be space enough on the right side to shift.
static void right_shift_string(char* buffer, int size){
	char tmp[size];
	strcpy(tmp, buffer);
	buffer++;
	memmove(buffer, tmp, strlen(tmp)+1);
}

// Shits the given buffer one byte to the left
// @note: There must be space enough on the left side to shift.
static void left_shift_string(char* buffer, int size){
	char tmp[size];
	strcpy(tmp, buffer);
	buffer--;
	memmove(buffer, tmp, strlen(tmp)+1);
	buffer[strlen(tmp)] = '\0';
}


// Will print the current buffer to the screen.
static void render_buffer(input_state_t* state){
	// Moves the cursor to the start of buffer
	for(int i = 0; i<state->pos; i++){
		pr_raw_str(MOVE_LEFT);
	}

	// Clear the line from the cursor and out
	pr_raw_str("\033[K");

	// Print the buffer
	pr_raw_str(state->buffer);

	// Return the cursor to where it was placed
	for(int i = 0; i < state->num_of_char-state->pos; i++){
		pr_raw_str(MOVE_LEFT);
	}
}

static void handle_escape(input_state_t* state){
	char rc[4] = "\0\0\0";
	read(STDIN_FILENO, &rc, 4); // Read the rest of the escape key
	if(rc[0] == CTRL_SEQ){
		switch(rc[1]){
			case LEFT:
				if(state->pos > 0){
					state->pos--;
					pr_raw_str(MOVE_LEFT);
				}else{
					pr_raw_char(BELL);
				}
				break;

			case RIGHT:
				if(state->pos < state->num_of_char){
					state->pos++;
					pr_raw_str(MOVE_RIGHT);
				}else{
					pr_raw_char(BELL);
				}
				break;
			case DOWN:
				break;
			case UP:
				break;
			default:
				break;
		}
	}

}

static void handle_backspace(input_state_t* state){
	if(state->pos > 0){
		if(state->pos == state->num_of_char){
			state->buffer[--state->pos] = '\0';
			pr_raw_str(MOVE_LEFT);
			pr_raw_char(" ");
			pr_raw_str(MOVE_LEFT);
		}else if(state->pos < state->num_of_char){
			left_shift_string(state->buffer+state->pos, state->size);
			pr_raw_str(MOVE_LEFT);
			state->pos--;
		}
		state->num_of_char--;

	}else{
		pr_raw_str(BELL);
	}

}

char* get_input(char* buffer, int size){
	enable_raw_mode();
	input_state_t state;
	state.buffer = buffer;
	state.pos = 0;
	state.num_of_char = 0;
	state.size = size;

	char eol = 0; // Is end of line
	while (!eol) {
		char ch = '\0';
		read(STDIN_FILENO, &ch, 1);
		if(iscntrl(ch)){ // Is not printable character
			switch(ch){
				case CTRL_A:
					while(state.pos > 0){
						pr_raw_str(MOVE_LEFT);
						state.pos--;
					}
					state.pos = 0;
					break;
				case CTRL_C:
					pr_raw_str(NEW_LINE);
					disbale_raw_mode();
					exit(0);
					break;
				case ENTER:
					pr_raw_str(NEW_LINE);
					eol = 1;
					continue;
				case ESCAPE:
					handle_escape(&state);
					break;
				case BACKSPACE:
					handle_backspace(&state);
					break;
				default:
					continue;

			}
		}else{ // Is printable character
			if(state.num_of_char < size){
				if(state.pos == state.num_of_char){
					buffer[state.pos++] = ch;
					buffer[state.pos] = '\0';
					state.num_of_char++;
					pr_raw_str(MOVE_RIGHT);
				}else if(state.pos < state.num_of_char){
					right_shift_string(buffer+state.pos, size);
					buffer[state.pos++] = ch;
					state.num_of_char++;
					pr_raw_str(MOVE_RIGHT);
				}
			}
		}

		render_buffer(&state);
	}
	
	disbale_raw_mode(); // if this is not done, the terminal will remain fucked after exit.
	return state.buffer;
}
