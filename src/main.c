#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include "better_input.h"



int main(){
	char buffer[LINE_MAX];

	printf("Input: ");

	get_input(buffer, LINE_MAX);

	printf("%s\n", buffer);

	return 0;
}
