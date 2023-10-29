/** An enum of possible IO errors (including command line arguments parsing) */
enum IO_error {
	/** No IO errors happened */
	IO_NO_ERR			=	0,
	/** No arguments were found */
	IO_NO_ARGS_ERR		=	-1,
	/** An unknown error occured */
	IO_UNKNOWN_ERR		=	-2,
	/** A wrong argument (or flag) was encountered */
	IO_WRONG_ARG_ERR	=	-3,
	/** No filename was found where it should have been in command arguments */
	IO_NO_FILENAME_ERR	=	-4,
};

/** A struct containing flags and input/output filenames from the command line. */
struct Args {
	/** A flag that controls whether unit testing will run instead of the program */
	bool test_mode;
	/** Input filename */
	const char *input_filename;
	/** Output filename */
	const char *output_filename;
};

const char *cmd_err_to_str(enum IO_error err);

/**
* Parses command line arguments for flags and filenames.
*
* @param [in]  argc the number of arguments
* @param [in]  argv array of pointers to arguments
* @param [out] args pointer to a struct containing flags and input/output filenames
*/
enum IO_error handle_arguments(int argc, const char *argv[], struct Args *args);