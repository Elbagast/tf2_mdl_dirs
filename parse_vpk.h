#ifndef PARSE_VPK_H
#define PARSE_VPK_H
/*
PURPOSE
Run "dir\vpk.exe l [file.vpk]" and capture the result for analysis.
Alternatively parse a given dir structure in the same way.
*/
#include <boost/filesystem.hpp>    // boost v 1.55.0

std::list<std::wstring> parse_dir(
    boost::filesystem::path const& dir
    );

std::list<std::wstring> parse_vpk(
    boost::filesystem::path const& file,
    boost::filesystem::path const& exe
    );

#endif