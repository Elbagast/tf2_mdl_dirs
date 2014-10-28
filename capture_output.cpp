#include "capture_output.h"
#include <Windows.h>
#include <sstream>
#include <list>
#include <locale>
#include <memory>

std::wstring capture_output(std::wstring const& cmdline)
{
	//First, set up a SECURITY_ATTRIBUTES structure to allow inheritance.
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = nullptr;

	//Create the stdout pipe.
	HANDLE child_stdout_read;
	HANDLE child_stdout_write;
	if (!CreatePipe(&child_stdout_read, &child_stdout_write, &sa, sizeof(sa)))
		throw std::runtime_error("Failed to create stdout pipe.");
	//Apparently I'd like the read handle to not be inherited.
	if (!SetHandleInformation(child_stdout_read, HANDLE_FLAG_INHERIT, 0))
		throw std::runtime_error("Failed to set child stdout read to noinherit");

	//Create the child process...
	//This structure is set by CreateProcess.
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	//This structure is essentially options for the process I want to create.
	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.hStdError = child_stdout_write;
	si.hStdOutput = child_stdout_write;
	//si.hStdInput = child_stdin_read;
	si.dwFlags |= STARTF_USESTDHANDLES;

	size_t len = cmdline.length();

	// make a non-const array that will clean itself up
	std::unique_ptr<wchar_t[]> editable_cmdline(new wchar_t[len + 1]);

	// fill the array
	for (size_t i = 0; i < len; ++i)
		editable_cmdline.get()[i] = cmdline[i];

	//null terminator, len is the index of the last entry
	editable_cmdline.get()[len] = '\0';

	if (!::CreateProcessW(
		nullptr,
		editable_cmdline.get(),	// full cmdline as an non-const null-terminated wchar_t array
		nullptr,
		nullptr,
		TRUE,
		CREATE_NO_WINDOW,
		nullptr,
		nullptr,
		&si,
		&pi)
		) throw std::runtime_error("Failed to start the process.");

	//This handle needs to be closed on this side so that the child can write to it.
	::CloseHandle(child_stdout_write);

	// stream that will be read into
	std::stringstream os;

	//Now I poll until the child process dies.
	while (
		[&]()//This lambda is really just a funny-looking truth condition.
		{ 
			switch (WaitForSingleObject(pi.hProcess, 0))
			{
			case WAIT_OBJECT_0:
				return false;
			case WAIT_TIMEOUT:
				return true;
			default:
				throw std::runtime_error("Failed to wait on process handle for some reason.");
			}
		}()
			)
	{
		//Dump the contents of the pipe (if any) to cout.
		char buffer[128];
		DWORD readbytes;
		do
		{
			ReadFile(child_stdout_read, buffer, sizeof(buffer), &readbytes, nullptr);
			os.write(buffer, readbytes);
		} while (readbytes != 0);
	}

	std::wstring_convert <std::codecvt<wchar_t, char, std::mbstate_t> >  convert;

	return convert.from_bytes(os.str());	// wstring from ostream from converted result
}