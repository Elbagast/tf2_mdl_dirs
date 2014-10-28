#ifndef CAPTURE_OUTPUT_H
#define CAPTURE_OUTPUT_H
/*
For a given command line, capture the entire stdout stream as a std::wstring
*/
#include <string>

std::wstring capture_output(std::wstring const& cmdline);

#endif