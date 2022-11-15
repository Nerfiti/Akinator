#ifndef LINESLIB_HPP
#define LINESLIB_HPP

#include <cstdio>

struct Line{
    char *start = nullptr;
    char *finish = nullptr;
};

struct Text{
    int   nLines = 0;
    char *content = nullptr;
    Line* lines = nullptr;
};

//-----------------------------------------------------------
//! Sort lines from the file and print it to the "Output.txt"
//!
//! \param [in] filename the name of the file
//! \param [in] reverse whether to sort the strings in reverse lexicographic order
//! \param [in] backsort whether to sort lines from the end
//-----------------------------------------------------------
void SortFile(const char *filename, bool reverse, bool backsort);

//-----------------------------------------------------------
//! Get size of the file
//!
//! \param [in] file pointer to the file
//!
//! \return size of the file
//-----------------------------------------------------------
int get_file_size(FILE *file);

//-----------------------------------------------------------
//! Copy text of the file to the memory. The user
//! guarantees that the entire text will fit into memory
//! according to this pointer
//!
//! \param [in] file pointer to the file
//! \param [out] text pointer to the structure "Text"
//-----------------------------------------------------------
void file_to_memory(FILE *file, Text *text);

//-----------------------------------------------------------
//! Split text into lines. The user guarantees that
//! the size of the array "lines" in the structure allows you to put
//! all the lines there
//!
//! \param [in] text the text to be split
//-----------------------------------------------------------
void text_to_lines(Text *text);

//-----------------------------------------------------------
//! Copy text of the file to the memory. The user
//! guarantees that the entire text will fit into memory
//! according to this pointer
//!
//! \param [in] file pointer to the file
//! \param [out] text pointer to the structure "Text"
//-----------------------------------------------------------
void file_to_memory_with_fread(FILE *file, Text *text);

//-----------------------------------------------------------
//! Split text into lines. The user guarantees that
//! the size of the array "lines" in the structure allows you to put
//! all the lines there
//!
//! \param [in] text the text to be split
//-----------------------------------------------------------
void text_to_lines_without_empty_lines(Text *text);

//-----------------------------------------------------------
//! Sort array of the lines.
//!
//! \param [in, out] lines array of the lines
//! \param [in] left the left border of the sorted part of the array
//! \param [in] right the right border of the sorted part of the array
//! \param [in] reverse whether to sort the strings in reverse lexicographic order
//! \param [in] backsort whether to sort lines from the end
//-----------------------------------------------------------
void lines_qsort(Line *lines, int left, int right, bool reverse, bool backsort);

//-----------------------------------------------------------
//! Compare two lines
//!
//! \param [in] string1 first line
//! \param [in] string2 second line
//! \param [in] reverse whether to sort the strings in reverse lexicographic order
//! \param [in] backsort whether to sort lines from the end
//!
//! \return 0 (if lines are equal), positive (if first line > second line)
//! or negative (if second line < first line) number
//-----------------------------------------------------------
int lines_compare(Line string1, Line string2, bool reverse, bool backsort);

//-----------------------------------------------------------
//! Swap 2 array elements
//!
//! \param [in, out] lines array
//! \param [in] fIndex index of the first element
//! \param [in] sIndex index of the second element
//-----------------------------------------------------------
void swap(Line *lines, int fIndex, int sIndex);

//-----------------------------------------------------------
//! print lines from array to the file
//!
//! \param [in] file pointer to the file
//! \param [in] lines array of the lines
//! \param [in] nLines number of lines
//-----------------------------------------------------------
void f_print_lines(FILE *file, Line *lines, int nLines);

//-----------------------------------------------------------
//! Сonvert type (char *) to type (Line)
//! The string must end with '\0'
//!
//! \param [in] string line of type (char *)
//!
//! \return line of type (Line)
//-----------------------------------------------------------
Line string_to_Line(char *string);

//-----------------------------------------------------------
//! Get length of the string.
//!
//! \param [in] string line
//!
//! \return length of the line
//-----------------------------------------------------------
unsigned long int line_length(const Line string);

//-----------------------------------------------------------
//! Find the first occurrence of a character in a string
//!
//! \param [in] string line
//! \param [in] target target symbol
//!
//! \return pointer to the symbol
//-----------------------------------------------------------
char *char_in_line(Line string, char target);

//-----------------------------------------------------------
//! Copy line prototype to the line target. The length of the target must be at least the length of the copy line
//!
//! \param [in] target first line
//! \param [in] prototype second line
//-----------------------------------------------------------
void lines_copy(Line *target, Line prototype);

//-----------------------------------------------------------
//! Append line add to the end of line target. The length of the target must not be less than the total length of the
//! lines target and add
//!
//! \param [in] target first line
//! \param [in] add second line
//-----------------------------------------------------------
void lines_cat(Line *target, Line add);

//-----------------------------------------------------------
//! Free buffer of the text
//!
//! \param text variable structure "Text"
//-----------------------------------------------------------
void FreeBuff(Text *text);

//-----------------------------------------------------------
//! Compare two lines from the beginning of the line in direct lexicographic order
//!
//! \param [in] string1 first line
//! \param [in] string2 second line
//! \param [in] reverse whether to sort the strings in reverse lexicographic order
//! \param [in] backsort whether to sort lines from the end
//!
//! \return 0 (if lines are equal), 1 (if first line > second line), -1 (if second line < first line)
//-----------------------------------------------------------
int lines_compare_for_qsort_fromBEGINNING_DIRECT(const void *line1, const void *line2);

//-----------------------------------------------------------
//! Compare two lines from the end of the line in direct lexicographic order
//!
//! \param [in] line1 first line
//! \param [in] line2 second line
//!
//! \return 0 (if lines are equal), positive (if first line > second line)
//! or negative (if second line < first line) number
//-----------------------------------------------------------
int lines_compare_for_qsort_fromEND_DIRECT(const void *line1, const void *line2);

//-----------------------------------------------------------
//! Compare two lines from the beginning of the line in reverse lexicographic order
//!
//! \param [in] line1 first line
//! \param [in] line2 second line
//!
//! \return 0 (if lines are equal), positive (if first line > second line)
//! or negative (if second line < first line) number
//-----------------------------------------------------------
int lines_compare_for_qsort_fromBEGINNING_REVERSE(const void *line1, const void *line2);

//-----------------------------------------------------------
//! Compare two lines from the end of the line in reverse lexicographic order
//!
//! \param [in] line1 first line
//! \param [in] line2 second line
//!
//! \return 0 (if lines are equal), positive (if first line > second line)
//! or negative (if second line < first line) number
//-----------------------------------------------------------
int lines_compare_for_qsort_fromEND_REVERSE(const void *line1, const void *line2);

#endif //LINESLIB_HPP