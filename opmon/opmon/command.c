#include "command.h"
#include <stdio.h>
#include <string.h>
#define PROMPT		 "opmon"
int num_commands = 0;
command_t commands[MAX_COMMANDS];
void register_command_func(const char* name, const char* params, \
const char* help, int (*func)(int argc, char* argv[])) {
	if(num_commands<MAX_COMMANDS) {
		commands[num_commands].name = name;
		commands[num_commands].params = params;
		commands[num_commands].help = help;
		commands[num_commands].func = func;
		num_commands++;
	}else {
		printf("Command '%s' ignored; MAX_COMMANDS limit reached.\n", name);
    printf("MAX_COMMANDS = %d, num_commands = %d\n", MAX_COMMANDS, num_commands);
	}
}
/* Command monitor */
int mon_commands(void)
{
  char str[1000];
  char *pstr = str;
  char *command_str;
  char *argv[20];
  int argc = 0, chrCnt=0;
  printf("\n" PROMPT "> ");/* Prompt */
	/* Read command line */
	fgets(str, 1000, stdin);
	str[strlen(str)-1] = '\0';	/* truncate the tail newline */
	while(*pstr==' ' && *pstr!='\0') pstr++; // Skip leading blanks 
	command_str = pstr;
  while(1) {
		/* Next argument */
		while(*pstr!=' ' && *pstr!='\0') pstr++;
		if(*pstr) {
			*pstr++ = '\0';
			while(*pstr==' ') pstr++;
			argv[argc++] = pstr;
		} else {
			break;
		}
	}
	/* Parse command */
	int i, found = 0;
	for( i=0; i<num_commands; i++) {
		if(strcmp(command_str, commands[i].name)==0) {
		  if(commands[i].func(argc, argv)==-1) {
				printf("Missing/wrong parameters, usage: %s %s\n", commands[i].name, commands[i].params);
			}
			found++;
			break;
		}else {
			//printf("%s", command_str);
		}
	}
	if(strcmp(command_str, "q")==0)
		return -1;
	if(!found) {
		printf("Unknown command. Type 'help' for help.\n");
	}
	return 0;
}

