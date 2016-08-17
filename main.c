#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#ifndef APP_NAME
#define APP_NAME "didj"
#endif
#define DEFAULT_ROW_SIZE 10
#define DEFAULT_GROUP_SIZE 30
#define MIN_ROW_SIZE 1
#define MAX_ROW_SIZE 100
#define MIN_GROUP_SIZE 1
#define MAX_GROUP_SIZE 100
#define MAX_FILE_LINE 4096
#define OUTPUT_FIELD_SEP '\t'

/* when defined, the output will be more readable but hard to import */ 
#define USE_READABLE_OUTPUT_FORMAT

/* 0=any (determined by the prefix: 0=octal 0x=hex any other digit=decimal) 10=decimal 16=hexa 8=octal */
#define INPUT_NUMBER_FORMAT 10 

/* all error messages go to stderr */
#define EPRINT(FMT, ERR, ...) fprintf(stderr, APP_NAME ": %s " FMT, (ERR) != 0 ? strerror(ERR) : "", ##__VA_ARGS__)

/* warning messages (to stderr) */
#define WPRINT(FMT, ...) do { if (!supress_warnings) EPRINT(FMT, 0, ##__VA_ARGS__); } while (0)

/* uncomment the following to supress debug messages */
/*#define DPRINT(X, ...)*/
/* debug messages, print out to stderr */
#ifndef DPRINT
#define DPRINT(LVL, FMT, ...) if ((LVL) <= debug_level) EPRINT(FMT, 0, ##__VA_ARGS__)
#endif

#define DBG_ALL 1
#define DBG_COUNT 1
#define DBG_PARSE 2

static int debug_level = 0;
static int human_readable = 0;
static int supress_warnings = 0;

/** utility to remove blank spaces at the start and end of a C string.
 * @param s: the string to be formatted.
 * @result: pointer to the formatted string.
 * @warning: the s buffer is changed in place. The returned string must not be freed.
 */ 
static char *trim(char *s)
{
	char *result;

	/* remove leading spaces */
	while (isspace(*s))
		s++;
	result = s;

	/* and trailing */
	s = s + strlen(s) - 1;
	while (s >= result) {
		if (isspace(*s)) {
			s--;
		} else {
			break;
		}
	}
	s++;
	*s = '\0';
	return result;
}

/** utility to search for the first digit in a C string.
 * @param s: the string to be searched.
 * @result: the pointer to the first digit found. '\0' if nothing.
 */
static char *strdigit(char *s)
{
	while (*s && !isdigit(*s))
		s++;
	return s;
}

/** utility to find the greatest value in an array of ints
 * @param aoi: array of ints
 * @param len: number of elements
 * @result: index of the greatest element or -1 if all zeroes.
 */
static int find_max(int *aoi, int len)
{
	int i;
	int max = 0;
	int result = -1;

	for (i = 0; i < len; i++) {
		if (aoi[i] > max) {
			max = aoi[i];
			result = i;
		}
	}
	return result;
}

/** print a usage message to stderr.
 */
static void usage(void)
{
	EPRINT("usage: [-H] [-d] [-i INPUT] [-o OUTPUT] [-c COLUMNS] [-g GROUP]\n"
		"where:\n"
		"\t-H human readable output (TSV otherwise)\n"
		"\t-w supress warnings\n"
		"\t-d debug messages\n"
		"\tINPUT input file name\n"
		"\tOUTPUT output file name (defaults to stdout)\n"
		"\tCOLUMNS number of columns per row (default %d)\n"
		"\tGROUP number of rows to group in (default %d)\n",
		0,
		DEFAULT_ROW_SIZE,
		DEFAULT_GROUP_SIZE
		);
}

/* process data.
 * @param fin: input file. Must be valid, open for read and positioned at the first byte.
 * @param fout: output file. Must be valid, open for read and write.
 * @param cols: number of columns per row.
 * @param group_size: number of rows in a group.
 * @result: error code or 0 if success.
 */
static int process_didj(FILE *fin, FILE *fout, int cols, int group_size)
{
	/* holds a line read from the input file */
	char *raw_line = NULL;
	/* the line read from the input file, ready to be processed */
	char *fmt_line;
	/* holds an array of integers extracted from the line string */
	int *a_row = NULL;
	/* holds the pattern */
	int *pattern_matrix = NULL;
	/* the group result */
	int *group = NULL;
	/* buffer to read from the input file */
	char *buffer = NULL;
	/* the index of the current group */
	int group_count = 0;
	/* counts the lines read from the file */
	int raw_lines = 0;
	/* counts warnings */
	int warnings = 0;
	/* counts errors */
	int errors = 0;
	/* function result */
	int result = 0;
	/* the line in the group */
	int in_group = 0;
	char *next_v;
	char *curr_v;
	/* processed line */
	int row = 0;
	/* temporary for a value */
	int a_val;
	/* column counter */
	int col;
	int v;

	/* everything is dynamic */	
	buffer = (char *) malloc(MAX_FILE_LINE);
	a_row = (int *) malloc(sizeof(a_row[0]) * cols);
	group = (int *) malloc(sizeof(group[0]) * cols * group_size);
	pattern_matrix = (int *) malloc(sizeof(pattern_matrix[0]) * (cols * cols));
	if (buffer == NULL || a_row == NULL || group == NULL || pattern_matrix == NULL) {
		result = ENOMEM;
		EPRINT("allocating buffers\n", ENOMEM);
		goto out;
	}
	memset(pattern_matrix, 0, sizeof(pattern_matrix[0]) * (cols * cols));

	/* keeps running until the end of file or error */
	while (1) {
		raw_line = fgets(buffer, MAX_FILE_LINE, fin);
		if (raw_line == NULL) {
			if (feof(fin)) {
				/* EOF */
			} else {
				/* another error */
				EPRINT("reading input file\n", errno);
				errors++;
				result = EIO;
			}
			break;
		}
		raw_lines++;
		/* remove spaces */
		fmt_line = trim(raw_line);
		/* ignore comments */
		if (fmt_line[0] == '#') {
			continue;
		}
		if (fmt_line[0] == '\0') {
			/* empty line */
			continue;
		}
		/* parse the line */
		DPRINT(DBG_PARSE, "parsing row %d line contents: \"%s\"\n", row, fmt_line);
		curr_v = fmt_line;
		/* extract the columns values */
		for (col = 0; col < cols; col++) {
			/* find the first digit */
			curr_v = strdigit(curr_v);
			if (curr_v[0] != '\0') {
				v = strtol(curr_v, &next_v, INPUT_NUMBER_FORMAT);
				DPRINT(DBG_PARSE, "(%d,%d)=%d\n", row, col, v);
				a_row[col] = v;
				/* nothing there */
				if (curr_v == next_v) {
					WPRINT("Row %d: skipping an invalid line with the following contents:\n[%s]\n", row, raw_line);
					warnings++;
					break;
				}
				/* next value */
				curr_v = next_v;
			} else {
				WPRINT("Row %d is incomplete, expected %d elements, found just %d. Line contents: \"%s\"\n", row, cols, col, raw_line);
				warnings++;
				break;
			}
		}
		/* if the row is valid */
		if (col >= cols) {
			row++;
			/* check if there were more data than necessary */
			if (next_v[0] != '\0') {
				WPRINT("Row %d: extra trailing characters found. Line contents: \"%s\"\n", row, raw_line);
				warnings++;
			}
			/* copy the values read into the group */
			for (col = 0; col < cols; col++) {
				group[in_group * cols + col] = a_row[col];
			}
			in_group++;
			/* if the group is complete, process the results */
			if (in_group >= group_size) {
				int ij[2]; /* fixed, 2 elements */
				int state; /* index of the current element, [0,1] */
				int how_many = 0; /* how many pairs were found */

				/* scan the group looking for sequences */
				state = 0;
				for (in_group = 0; in_group < group_size; in_group++) {
					int imax;

					imax = find_max(&group[in_group * cols], cols);
					/* found anything? */
					if (imax >= 0) {
						DPRINT(DBG_COUNT, "ingroup=%d imax=%d\n", in_group, imax);
						ij[state] = imax;
						if (state == 1) {
							state = 0;
							how_many++;
							pattern_matrix[ij[0] + ij[1] * cols]++;
							DPRINT(DBG_COUNT, "ingroup row %d: (%d,%d)=%d\n", in_group, ij[0], ij[1], pattern_matrix[ij[0] + ij[1] * cols]);
						} else {
							state++;
						}
					}
				}
				DPRINT(DBG_COUNT, "found %d pairs\n", how_many);
				if (how_many > 0) {
					int i, j, w, z;

					/* just to control the first column */
					how_many = 0;
					/* print the results */
					for (i = 0; i < cols; i++) {
						for (j = 0; j < cols; j++) {
							if (pattern_matrix[i + j * cols] > 0) {
								if (how_many == 0) {
									if (human_readable){

									/*	fprintf(stdout, "%d\t(d%d,d%d)=%d", group_count, i, j, pattern_matrix[i + j * cols]); */
									  w = i + 1;
									  z= j + 1;

										fprintf(stdout, "%d;d%dd%d=%d", group_count, w, z, pattern_matrix[i + j * cols]); 
									 }
									else
										fprintf(stdout, "%d%c%d%c%d%c%d", group_count, OUTPUT_FIELD_SEP, i, OUTPUT_FIELD_SEP, j, OUTPUT_FIELD_SEP, pattern_matrix[i + j * cols]); 
								} else {
									if (human_readable) {

									 w = i + 1;
									  z= j + 1;
										/*fprintf(stdout, "\t(d%d,d%d)=%d", i, j, pattern_matrix[i + j * cols]); */
										fprintf(stdout, ";d%dd%d=%d", w, z, pattern_matrix[i + j * cols]); 

									}
									else
										fprintf(stdout, "%c%d%c%d%c%d", OUTPUT_FIELD_SEP, i, OUTPUT_FIELD_SEP, j, OUTPUT_FIELD_SEP, pattern_matrix[i + j * cols]); 
								}
								how_many++;
							}
						}
					}
					fprintf(stdout, "\n");
					DPRINT(DBG_COUNT, "%d pairs printed\n", how_many);
				} else {
					DPRINT(DBG_ALL, "found nothing!\n");
				}
				in_group = 0;
				memset(pattern_matrix, 0, sizeof(pattern_matrix[0]) * (cols * cols));
				group_count++;
			}
			/* move the first group out */
			//memmove();
		} else {
			/*
			WPRINT("Row %d: insufficient elements, expected %d, found %d\n", row, cols, col);
			warnings++;
			*/
		}
	}
	/* check for an incomplete group */
	if (in_group != 0) {
		warnings++;
		WPRINT("Row %d: no more data, expected %d rows to complete the group but found only %d rows\n", row, group_size, in_group);
	}
out:
	if (buffer)
		free(buffer);
	if (a_row)
		free(a_row);
	if (group)
		free(group);
	if (pattern_matrix)
		free(pattern_matrix);
	if (errors != 0) {
		EPRINT("found %d warnings and %d errors while processing the data. Valid rows: %d\n", 0, warnings, errors, row);
	} else if (warnings != 0) {
		WPRINT("found %d warnings while processing the data. Valid rows: %d\n", warnings, row);
	}
	return result;
}

int main(int argc, char *argv[])
{
	const char *input_file_name = NULL;
	const char *output_file_name = NULL;
	FILE *input_file = NULL;
	FILE *output_file = NULL;
	int row_size = DEFAULT_ROW_SIZE;
	int group_size = DEFAULT_GROUP_SIZE;
	int ch;
	int result = 0;

	while ((ch = getopt(argc, argv, "Hwd?hi:o:c:g:")) != -1) {
		switch (ch) {
			case 'H':
				human_readable = 1;
				break;
			case 'w':
				supress_warnings = 1;
				break;
			case 'd':
				debug_level++;
				break;
			case 'i':
				input_file_name = optarg;
				break;
			case 'o':
				output_file_name = optarg;
				break;
			case 'c':
				row_size = strtol(optarg, NULL, 0);
				break;
			case 'g':
				group_size = strtol(optarg, NULL, 0);
				break;
			case '?':
			case 'h':
			default:
				result = -1;
				usage();
				goto out;
		}
	}
	/* input/output */
	if (input_file_name == NULL)
		input_file = stdin;
	else {
		/* input is read only */
		input_file = fopen(input_file_name, "rb");
	}
	if (output_file_name == NULL)
		output_file = stdout;
	else {
		/* XXX if a real file, output will be truncated */
		output_file = fopen(output_file_name, "w");
	}
	/* sanity check */
	if (row_size < MIN_ROW_SIZE || row_size > MAX_ROW_SIZE) {
		EPRINT("-c argument must be between %d and %d\n", EINVAL, MIN_ROW_SIZE, MAX_ROW_SIZE);
		goto out;
	}
	if (group_size < MIN_GROUP_SIZE || group_size > MAX_GROUP_SIZE) {
		EPRINT("-g argument must be between %d and %d\n", EINVAL, MIN_GROUP_SIZE, MAX_GROUP_SIZE);
		goto out;
	}
	if (input_file == NULL) {
		result = -1;
		EPRINT("can't open input file \"%s\"\n", errno, input_file_name);
		goto out;
	}
	if (output_file == NULL) {
		result = -2;
		EPRINT("can't open output file \"%s\"\n", errno, output_file_name);
		goto out;
	}
	result = process_didj(input_file, output_file, row_size, group_size);
	if (result != 0)
		result = -2;
out:
	if (input_file != NULL)
		fclose(input_file);
	if (output_file != NULL)
		fclose(output_file);
	return result;
}
