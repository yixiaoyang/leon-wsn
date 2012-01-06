#ifndef COMMAND_H
#define COMMAND_H
#define MAX_COMMANDS   100
typedef struct
{
	const char* name;
	const char* params;
	const char* help;
	int (*func)(int argc, char* argv[]);
}command_t;


void register_command_func(const char* name, const char* params, \
const char* help, int (*func)(int argc, char* argv[]));
#define register_command(name, params, help, func) \
	register_command_func(name, params, help, func)
int mon_commands(void);

#endif //COMMAND_H
