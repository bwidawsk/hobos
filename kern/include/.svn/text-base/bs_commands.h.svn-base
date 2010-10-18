#ifndef _BS_COMMANDS_H_
#define _BS_COMMANDS_H_

/* BuiltinShell Commands */

#define BS_COMMAND_KEY sect_cmds
#define BS_COMANDS_CREATE_LIST CTLIST_CREATE(BS_COMMAND_KEY, struct bs_cmd *); 

#define BS_COMMAND_DECLARE(cmd_name, cmd_exec, cmd_help) \
	static struct bs_cmd cmd_name = { \
		.name = #cmd_name, \
		.execute = cmd_exec, \
		.help = cmd_help, \
		}; \
	CTLIST_ELEM_ADD(BS_COMMAND_KEY, cmd_name##_list_ptr, struct bs_cmd *, (struct bs_cmd *)&cmd_name);

#define COMMAND_FOREACH(elem, garbage) \
	CTLIST_FOREACH(elem, BS_COMMAND_KEY, garbage)
	
struct bs_cmd {
	char *name;
	void *(*execute)(char *argv[]);
	void (*help)();
};
void do_shell_cmd(int argc, char *argv[]);

#endif