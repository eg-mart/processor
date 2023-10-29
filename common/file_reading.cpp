#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

#include "file_reading.h"

const char *TXT_ERROR_MSG[] = {
	"No errors were encountered.",
	"Reading text failed: out of memory.",
	"Error accessing input file.",
	"Error reading from the input file."
};

size_t count_sep_in_str(const char *str, char c);
void split_text(Text *text, char sep);

enum TextError read_text(struct Text *text, const char *filename)
{
	assert(text);
	assert(filename);

	FILE *file = fopen(filename, "r");
	if (file == NULL) return ERR_FILE_ACCESS;

	size_t filesize = 0;
	enum TextError err = get_file_size(file, &filesize);
	if (err < 0) return err;
	text->buffer_size = filesize + 1;

	text->buffer = (char *) calloc(text->buffer_size, sizeof(char));
	if (text->buffer == NULL) return ERR_MEM;

	size_t read_chars = fread(text->buffer, sizeof(char), text->buffer_size - 1, file);
	if (read_chars != text->buffer_size - 1 || ferror(file)) return ERR_FILE_READING;
	text->buffer[text->buffer_size - 1] = '\0';

	text->num_lines = count_sep_in_str(text->buffer, '\n');
	
	text->lines = (struct Line *) calloc(text->num_lines + 1, sizeof(struct Line));
	if (text->lines == NULL) return ERR_MEM;

	*text->lines = { 0, text->buffer };
	split_text(text, '\n');

	fclose(file);

	return NO_TXT_ERR;
}

enum TextError get_file_size(FILE *file, size_t *size)
{
	assert(file);
	assert(size);

	int fd = fileno(file);
	struct stat stbuf;
	if (fstat(fd, &stbuf) == -1) return ERR_FILE_ACCESS;
	*size = (size_t) stbuf.st_size;
	return NO_TXT_ERR;
}

size_t count_sep_in_str(const char *str, char c)
{
	assert(str);

	size_t num_chars = 0;
	while (*str != '\0')
		if (*str++ == c)
			num_chars++; 
	return num_chars;
}

void split_text(Text *text, char sep)
{
	assert(text);
	assert(text->buffer);
	assert(text->lines);

	char *iter_buf = text->buffer;
	struct Line *iter_lines = text->lines;
	size_t cur_line_len = 0;

	while (*iter_buf != '\0') {
		cur_line_len++;
		if (*iter_buf == sep) {
			*iter_buf = '\0';
			iter_lines->len = cur_line_len;
			*++iter_lines = { 0, iter_buf + 1 };
			cur_line_len = 0;
		}
		iter_buf++;
	}
	iter_lines->len = cur_line_len;
}

void destroy_text(struct Text *text)
{
	assert(text);

	free(text->lines);
	free(text->buffer);

	text->lines = NULL;
	text->buffer = NULL;

	text->num_lines = 0;
	text->buffer_size = 0;
}

const char *txt_error_to_str(enum TextError err)
{
	return TXT_ERROR_MSG[-((int) err)];
}
