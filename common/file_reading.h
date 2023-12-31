#ifndef FILE_READING
#define FILE_READING

struct Text {
	size_t buffer_size;
	char *buffer;
	size_t num_lines;
	struct Line *lines;
};

struct Line {
	size_t len;
	char *txt;
};

enum TextError {
	NO_TXT_ERR = 0,
	ERR_MEM = -1,
	ERR_FILE_ACCESS = -2,
	ERR_FILE_READING = -3,
};

enum TextError read_text(struct Text *text, const char *filename);
enum TextError get_file_size(FILE *file, size_t *size);
void destroy_text(struct Text *text);
const char *txt_error_to_str(enum TextError err);

#endif
