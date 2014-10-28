#include "parse_vpk.h"

#include "capture_output.h"
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <locale>

std::list<std::wstring> parse_dir(
	boost::filesystem::path const& dir
	)
{
	namespace bfs = boost::filesystem;

	std::list<std::wstring> results;
	for (bfs::recursive_directory_iterator it(dir), end; it != end; ++it)
	{
		if (bfs::is_regular_file(*it))								// found a file
		{
			bfs::path p(*it);										// path p is BLAH/tf/custom/dir/stuff/file.ext
			size_t dir_length(dir.wstring().size());				// path dir is BLAH/tf/custom/dir		
			bfs::path short_p(p.wstring().substr(dir_length + 1));	// cut p down to stuff/file.ext
			short_p.make_preferred();								// stuff\file.ext
			results.push_back(short_p.wstring());
		}
	}
	return results;
}

std::list<std::wstring> parse_vpk(
	boost::filesystem::path const& file,
	boost::filesystem::path const& exe
	)
{
	std::wstring cmdline = exe.wstring() + L" l \"" + file.wstring() + L"\"";
	std::list<std::wstring> results{};

	try
	{
		std::wstringstream wos{ capture_output(cmdline) };	// this is the part that may throw exceptions, it won't retaurn if it does
		std::wstring s{};									// capture strings to here

		while (std::getline(wos, s))						// reading into s up to '\n'
		{
			if (!s.empty())	// just in case
			{
				if (s.back() == L'\r')	// getline ends at '\n' or eof, leaving the '\r'
					s.pop_back();
				boost::filesystem::path temp(s);
				temp.make_preferred();
				results.push_back(temp.wstring());
			}
		}
	}
	catch (std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return results;
}