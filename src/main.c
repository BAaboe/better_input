#include <stdio.h>
#include <limits.h>
#include "better_input.h"



int main(){
	char buffer[LINE_MAX];

	get_input(buffer, LINE_MAX);

	printf("%s\n", buffer);

	return 0;
}
