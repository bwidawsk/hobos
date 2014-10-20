#include <mutex.h>
#include <console.h>
#include <bs_commands.h>

//#define USE_LOCAL_ECHO

extern int early_printf(const char *format, ...);

CONSOLE_CREATE_LIST;

MUTEX_DECLARE(console_puts_lock);

struct console_driver *best_cons;

int use_allcons = 0;
int inited = 0;

#define init_console(cons) if (cons->init) { cons->init(cons); }
#define putc_console(cons, c) if (cons->putc) { cons->putc(cons, c); }
#define get_console(cons) cons->getc ? cons->getc(cons) : 0

void
console_init() {
	int throwaway = 0;
	int best_awe = 0;
	struct console_driver *cons;

	CONSOLE_FOREACH(cons, throwaway) {
		init_console(cons);

		if(cons->awesomeness > best_awe)
			best_cons = cons;
	}
	inited = 1;
}

void
console_putc(char c) {
	int throwaway = 0;
	struct console_driver *cons;
	if (use_allcons) {
		CONSOLE_FOREACH(cons, throwaway) {
			putc_console(cons, c)
		}
	} else {
		putc_console(best_cons, c);
	}
}

char
console_getc() {
	int throwaway = 0;
	struct console_driver *cons;
	/* FIXME: this won't work, must getc from only 1 console at a time */
	if (use_allcons) {
		CONSOLE_FOREACH(cons, throwaway) {
			return get_console(cons);
		}
	} else {
		return get_console(best_cons);
	}
	return -1;
}

void 
console_puts(char *s) {
	if(!inited) {
		early_printf(s);
		return;
	}

	int str_max = 4096;
	mutex_acquire(&console_puts_lock);
	while(*s != 0 && str_max--) {
		console_putc(*s);
		s++;
	}
	mutex_release(&console_puts_lock);

}

/* TODO: replace this with libc like stuff */
static void
parse_and_send_cmd(char *in_cmd, struct console_info *info) {
	#define MAX_ARGS 4
	int idx;
	int argv_idx = 0;
	int last_was_space = 1; // assume last char was a space to satisfy logic below
	char *argv[MAX_ARGS];

	/* 
	 * basic state machine that only records the next pointer if the last char
	 * was a space.
	 */
	for (idx = 0; in_cmd[idx] != 0; idx++) {
		if (in_cmd[idx] == ' ') {
			in_cmd[idx] = 0;
			last_was_space = 1;
		} else if (last_was_space) {
			argv[argv_idx] = &in_cmd[idx];
			argv_idx++;
			last_was_space = 0;
		}
	}

	do_shell_cmd(info, argv_idx, argv);
}

void
start_interactive_console(struct console_info *info) {
	#define MAX_CMD 256
	while(1) {
		int cmd_len = 0;
		int idx = 0;
		char in_cmd[MAX_CMD + 1];

		console_puts("\nhobo_cons> ");
		in_cmd[MAX_CMD] = 0;

		/* TODO: bzero or memset */
		for(idx = 0; idx < MAX_CMD; idx++) {
			in_cmd[idx] = 0;
		}

		idx = 0;
		char c = 0;
		do {
			c = console_getc();
			in_cmd[idx] = c;
			/* Handle backspace support */
			if (c == 0x7F) {
				if (idx > 0) {
					in_cmd[idx] = 0;
					idx--;
				}
				continue;
			}
			idx++;
			#ifdef USE_LOCAL_ECHO
			console_putc(c);
			#endif
		} while(c != '\r' && c != '\n');

		/* Shortcut empty commands */
		if (idx == 0)
			continue;

		/* remove the \r which was the last character */
		cmd_len = idx - 1;

		/* make sure we null pad it */
		in_cmd[cmd_len] = 0;

		/* go work on it */
		parse_and_send_cmd(in_cmd, info);
	}
}
