/*
When run: 
- assume this exe is in the tf\custom folder
- find tf2 .vpk files
- get a list of all files in them
- identify all folders that contain .mdl files
- build a directory structure containing those folders
*/

//#include "windows_functions.h"
#include "kr.h"
#include "parse_vpk.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    using boost::filesystem::path;
    using boost::filesystem::exists;
    using boost::filesystem::is_directory;
    using boost::filesystem::is_regular_file;
    using boost::filesystem::create_directories;

    //using boost::filesystem::directory_iterator;
    //using boost::filesystem::recursive_directory_iterator;

    KeepRunning kr{};
    //std::wcout << argv[0] << std::endl;
    //std::wcout << boost::filesystem::current_path() << std::endl; // can't rely on this, gives where the exe was excuted *from* e.g. via a .bat

    // First output block - announcement
    std::wcout
        << L"tf2_mdl_dirs.exe" << std::endl
        << L"==============================" << std::endl
        // output formatting check, 80 chars including the \r\n
        //<< L"12345678901234567890123456789012345678901234567890123456789012345678901234567890" << std::endl
        << L"Scans your TF2 installation and makes a directory named 'mdl_directories' in " << std::endl
        << L"your 'Team Fortress 2\\tf\\custom' directory which contains all the directories " << std::endl
        << L"in 'tf2_misc_dir.vpk' which contain at least one '.mdl' file. This allows " << std::endl
        << L"hlmv.exe to browse these directories." << std::endl
        << std::endl
        << L"Run this program any time you come across a directory you can see but not" << std::endl
        << L"browse in hlmv.exe and the appropriate directories will be added. " << std::endl
        //<< L"Directories no longer containing .mdl file(s) will be removed." << std::endl
        << std::endl;
    

    // Second output block - setup results header
    std::wcout
        << L"Configuration:" << std::endl
        << L"------------------------------" << std::endl;

    path self_path{argv[0]};
    /*
    Work down the directories in the path to determine if the program is in the correct directory.
    Is there a more efficent way of doing this? Probably.
    Does it matter? Probably not.

    SteamApps\\common\\Team Fortress 2\\tf\\custom
    */
    if (!(
        self_path.parent_path().leaf().wstring() == L"custom"
        && self_path.parent_path().parent_path().leaf().wstring() == L"tf"
        && self_path.parent_path().parent_path().parent_path().leaf().wstring() == L"Team Fortress 2"
        && self_path.parent_path().parent_path().parent_path().parent_path().leaf().wstring() == L"common"
        && self_path.parent_path().parent_path().parent_path().parent_path().parent_path().leaf().wstring() == L"SteamApps"
        ))
    {
        // Third output block A - setup failure
        std::wcout
            << L"Not in the correct directory. To use this program place it in your" << std::endl 
            << L"'SteamApps\\common\\Team Fortress 2\\tf\\custom' directory and run it there." << std::endl;
        return 1;
    }
    
    // Build the base path to where the directories will be created
    path target_dir{ self_path.parent_path() / L"mdl_directories" };
    
    // Build the path to tf2_misc_dir.vpk and check it exists
    path tf2_misc_dir_vpk{ self_path.parent_path().parent_path() / L"tf2_misc_dir.vpk" };
    if ( !(exists(tf2_misc_dir_vpk) && is_regular_file(tf2_misc_dir_vpk)) )
    {
        std::wcout << L"tf2_misc_dir.vpk not found at " << tf2_misc_dir_vpk << std::endl;
        return 1;
    }
    
    // Build the path to vpk.exe and check it exists
    path vpk_exe{ self_path.parent_path().parent_path().parent_path() / L"bin" / L"vpk.exe" };
    if ( !(exists(vpk_exe) && is_regular_file(vpk_exe)) )
    {
        std::wcout << L"vpk.exe not found at " << vpk_exe << std::endl;
        return 1;
    }

    // Third output block B - setup success
    std::wcout
        << L"Target Root: " << std::endl
        << target_dir << std::endl
        << L"tf2_misc_dir.vpk: " << std::endl
        << tf2_misc_dir_vpk << std::endl
        << L"vpk.exe: " << std::endl
        << vpk_exe << std::endl
        << std::endl;
    
    std::list<std::wstring> all_files = parse_vpk(tf2_misc_dir_vpk, vpk_exe);
    if (all_files.empty())
    {
        std::wcerr << L"vpk parse failed." << std::endl;
        return 1;
    }

    std::list<std::wstring> mdl_files{};
    for (auto& file : all_files)
    {
        auto str_size = file.size();
        if (str_size > 11
            && file.substr(0, 7) == L"models\\"
            && file.substr(str_size - 4) == L".mdl")
        {
            mdl_files.push_back(file);
        }
    }

    /*
    We could convert all of these to path objects now and even make them the full path, but 
    sorting path objects is extremely slow compared to just doing strings - the list<path>.sort() 
    call becomes the slowest part of the entire program.

    So we keep them as strings, and forget about the front of the path since they will all have that
    and it will be the same in the end.
    */
    std::list<std::wstring> mdl_directories{};
    for (auto& file : mdl_files)
    {
        std::wstring dir = path(file).parent_path().wstring();
        mdl_directories.push_back(dir);
    }
    mdl_directories.sort();
    mdl_directories.unique();

    // Third output block - sorting results
    std::wcout
        << L"tf2_misc_dir.vpk statistics:" << std::endl
        << L"------------------------------" << std::endl
        << all_files.size() << L" files in total." << std::endl
        << mdl_files.size() << L" .mdl files." << std::endl
        << mdl_directories.size() << L" unique directories cotaining .mdl file(s)." << std::endl
        << std::endl;

    size_t count_existing = 0;
    size_t count_created = 0;
    size_t count_failed = 0;
        
    for (auto& mdl_dir : mdl_directories)
    {
        /*
        Turn the string into a path object we can use and add the front of the path, making it 
        fully qualified.
        */
        path directory = target_dir / path(mdl_dir);

        if (exists(directory))
        {
            ++count_existing;
        }
        else
        {
            try
            {
                // If an exception is thrown, it's going to come from this line,
                // so incrementing the account should only happen if creation succeeds
                create_directories(directory);
                ++count_created;
            }
            catch (std::runtime_error& e)
            {
                std::wcout 
                    << e.what() << std::endl
                    << "Could not create directory: " << directory << std::endl;
                ++count_failed;
            }
        }
    }

    /*
    // ok this is arse to do and has dubious usefullness.

    // Remove directories already present which have no corresponding mdl_dir
    size_t count_removed = 0;
    std::list<std::wstring> existing_dirs{};

    recursive_directory_iterator i{target_dir};
    recursive_directory_iterator end{};
    while (i != end)
    {
        // if i is at a top directory, add it to existing_dirs
        if ()
        {

        }
        ++i;
    }
    */

    // Forth output block - results
    std::wcout 
        << L"Directory creation statistics:" << std::endl
        << L"------------------------------" << std::endl
        << count_existing << L" already exist." << std::endl
        << count_created << L" created." << std::endl
        << count_failed << L" could not be created." << std::endl
        //<< count_removed << L" removed." << std::endl
        << std::endl;
    return 0;
}