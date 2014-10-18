#include <bs_commands.h>
#include <console.h> // console_puts
BS_COMANDS_CREATE_LIST;

static void *
help_cmd_execute(struct console_info *info, int argc, char *argv[]) {
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
void
do_shell_cmd(struct console_info *info, int argc, char *argv[]) {
	int throwaway = 0;
	struct bs_cmd *cmd = &help_cmd;
	struct bs_cmd *best = &help_cmd;
	char *cmd_str = argv[0];

	COMMAND_FOREACH(cmd, throwaway) {
		int i = 0;
		int match = 1;
		while(cmd_str[i] != 0 && cmd->name[i] != 0) {
			if (cmd_str[i] == cmd->name[i]) {
			} else {
				match = 0;
				break;
			}
			i++;
		}

		// if the matching string ended, but we still have more in our input
		// command it wasn't a match
		if (match && (cmd_str[i] == 0)) {
			best = cmd;
			match = 0;
		}

		if (match && (cmd->name[i] == 0))
			match = 0;

		if (match) {
			best = cmd;
			break;
		}
	}

	best->execute(info, argc, argv);
}
