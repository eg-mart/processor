#include <assert.h>
#include <string.h>

#include "cmd_args.h"

const char *CMD_ERR_MSG[] = {
	"No errors were encountered\n",
	"No arguments were provided. Usage: assembler -i input -o output [--test]\n",
	"An unknown error occured while handling command arguments\n",
	"Wrong arguments. Usage: assembler -i input -o output [--test]\n",
	"Filename not found where expected. Usage: assembler -i input -o output [--test]\n",
};

enum IO_error handle_arguments(int argc, const char *argv[], struct Args *args)
{
	assert(argv != NULL);
	assert(args != NULL);

	if (argc < 2)
		return IO_NO_ARGS_ERR;

	char c = '\0';
	args->input_filename = NULL;
	args->output_filename = NULL;
	while (--argc > 0 && (*++argv)[0] == '-') {
		#ifdef TEST
		if (strcmp(*argv, "--test") == 0) {
			args->test_mode = true;
			continue;
		}
		#endif

		bool flags_ended = false;
		while ((c = *++argv[0]) && !flags_ended)
			switch (c) {
				case 'i':
					if (argc < 2 || *++argv[0] != '\0' || (*++argv)[0] == '-')
						return IO_NO_FILENAME_ERR;
					args->input_filename = argv[0];
					argc--;
					flags_ended = true;
					break;
				case 'o':
					if (argc < 2 || *++argv[0] != '\0' || (*++argv)[0] == '-')
						return IO_NO_FILENAME_ERR;
					args->output_filename = argv[0];
					argc--;
					flags_ended = true;
					break;
				default:
					return IO_WRONG_ARG_ERR;
			}
	}

	if (!args->input_filename || !args->output_filename)
		return IO_NO_ARGS_ERR;

	if (argc > 0)
		return IO_WRONG_ARG_ERR;
	return IO_NO_ERR;
}

const char *cmd_err_to_str(enum IO_error err)
{
	return CMD_ERR_MSG[-((int) err)];
}