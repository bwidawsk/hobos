#include <bs_commands.h>
#include <console.h> // console_puts
BS_COMANDS_CREATE_LIST;

void
do_shell_cmd(int argc, char *argv[]) {
	int throwaway = 0;
	struct bs_cmd *cmd;
	char *cmd_str = argv[0];

	COMMAND_FOREACH(cmd, throwaway) {
		int i = 0;
		int match = 1;
		while(cmd_str[i] != 0 && cmd->name[i] != 0) {
			if(cmd_str[i] == cmd->name[i]) {
			
			} else {
				match = 0;
				break;
			}
			i++;
		}
		// if the matching string ended, but we still have more in our input
		// command it wasn't a match
		if(match && cmd_str[i]) {
			match = 0;
		}
		
		if(match) {
			break;
		}
	}
	cmd->execute(argv);
}


static void *
help_cmd_execute(char *argv[]) {
	int throwaway = 0;
	struct bs_cmd *cmd;
	console_puts("Displaying commands: \n");
	COMMAND_FOREACH(cmd, throwaway) {
		console_puts("    ");
		console_puts(cmd->name);
		console_puts("\n");
	}
	return 0;
}

static void
help_cmd_help() {

}
	
BS_COMMAND_DECLARE(help_cmd, help_cmd_execute, help_cmd_help);