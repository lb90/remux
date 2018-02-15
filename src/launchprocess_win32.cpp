#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <glib.h>
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#include "launchprocess.h"
#include "app.h" /*TODO temp */

void get_lasterror_description(DWORD lasterrorcode, std::string& errorstring_utf8) 
{ 
    // Retrieve the system error message for the last-error code
    uint16_t *lpMsgBuf = NULL;
	DWORD flags;
	
	flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS;

    FormatMessageW(flags, NULL, lasterrorcode,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPWSTR) &lpMsgBuf, 0, NULL);
	
	gchar *msgbuf_utf8 = NULL;
	msgbuf_utf8 = g_utf16_to_utf8(lpMsgBuf, -1, NULL, NULL, NULL);
	if (msgbuf_utf8)
		errorstring_utf8 = msgbuf_utf8;

	if (msgbuf_utf8)
		g_free(msgbuf_utf8);
	if (lpMsgBuf)
		LocalFree(lpMsgBuf);
}

std::string util_argv_to_commandline(const std::vector<std::string>& argv) {
	std::string commandline;
	
	if (argv.empty()) return "";

	std::string quote = "\"";
	commandline = quote + argv[0] + quote;
	for (size_t i = 1; i < argv.size(); i++) {
		commandline += std::string(" ");
		if (argv[i].find(' ') != std::string::npos)
			commandline += quote + argv[i] + quote;
		else
			commandline += argv[i];
	}

	return commandline;
}

int read_pipe(HANDLE hstdout_rd, std::string& outputstring) {
	DWORD lasterror = 0;
	BOOL bcode = 0;
	int ret = 0;
	const DWORD sz = 100;
	char *buf = new char[sz];
	bool done = false;

	while (!done) {
		DWORD bytesread = 0;
		memset(buf, 0, sz);
					
		bcode = ReadFile(hstdout_rd, buf, sz - 1, &bytesread, NULL);
		if (bcode == 0) {
			lasterror = GetLastError();
			if (lasterror == ERROR_BROKEN_PIPE) {
				/* it's ok, child process has done and is closing stdout */
				done = true;
			}
			else {
				/* truly an error */
				outputstring = "error in ReadFile: "; /*TODO += */
				std::string description;
				get_lasterror_description(lasterror, description);
				outputstring += description;
				ret = -1;
				break;
			}
		}

		outputstring += buf;
	}
	delete[] buf;
	buf = nullptr;
	
	return ret;
}

int launch_process(const std::vector<std::string>& argv,
                   std::string& outputstring,
                   std::string& errstring,
                   int *status)
{
	SECURITY_ATTRIBUTES sec = {};
	PROCESS_INFORMATION proc = {};
	STARTUPINFO startup = {};
	HANDLE hstdout_rd = NULL;
	HANDLE hstdout_wr = NULL;
	HANDLE hstdin_rd = NULL;
	HANDLE hstdin_wr = NULL;
	uint16_t *application_name = NULL;
	uint16_t *command_line = NULL;
	DWORD lasterror = 0;
	BOOL bcode = 0;
	DWORD dwcode = 0;
	int code = 0;
	GError *errspec = NULL;
	int ret = 0;
	
	sec.nLength = sizeof(SECURITY_ATTRIBUTES);
	sec.bInheritHandle = TRUE;
	sec.lpSecurityDescriptor = NULL;
	
	// Create a pipe for the child process's STDOUT.
	bcode = CreatePipe(&hstdout_rd, &hstdout_wr, &sec, 0);
	if (bcode == 0) return -1;
	// Ensure the read handle to the pipe for STDOUT is not inherited.
	bcode = SetHandleInformation(hstdout_rd, HANDLE_FLAG_INHERIT, 0);
	if (bcode == 0) return -1;
	// Create a pipe for the child process's STDIN.
	bcode = CreatePipe(&hstdin_rd, &hstdin_wr, &sec, 0);
	if (bcode == 0) return -1;
	// Ensure the write handle to the pipe for STDIN is not inherited.
	bcode = SetHandleInformation(hstdin_wr, HANDLE_FLAG_INHERIT, 0);
	if (bcode == 0) return -1;

	startup.cb = sizeof(STARTUPINFO);
	startup.hStdOutput = hstdout_wr;
	startup.hStdInput  = hstdin_rd;
	startup.dwFlags |= STARTF_USESTDHANDLES;
	
	application_name = g_utf8_to_utf16(argv[0].c_str(), -1, NULL, NULL, &errspec);
	if (!application_name) {
		outputstring = "error converting text UTF-8 to UTF-16 text";
		if (errspec && errspec->message && errspec->message[0]) {
			outputstring += ": ";
			outputstring += errspec->message;
		}
		ret = -1;
	}
	else {
		command_line = g_utf8_to_utf16(util_argv_to_commandline(argv).c_str(), -1, NULL, NULL, &errspec);
		if (!command_line) {
			outputstring = "error converting text UTF-8 to UTF-16 text";
			if (errspec && errspec->message && errspec->message[0]) {
				outputstring += ": ";
				outputstring += errspec->message;
			}
			ret = -1;
		}
		else {
			DWORD flags = 0;
			if (!app::showwindow)
				flags |= CREATE_NO_WINDOW;
			bcode = CreateProcessW((LPWSTR) application_name,
								   (LPWSTR) command_line,
								   NULL,      // process security attributes
								   NULL,      // primary thread security attributes
								   TRUE,      // handles are inherited
								   flags,     // creation flags
								   NULL,      // use parent's environment
								   NULL,      // use parent's current directory
								   &startup,  // STARTUPINFO pointer
								   &proc);    // receives PROCESS_INFORMATION
			if (bcode == 0) {
				lasterror = GetLastError();
				outputstring = "error in CreateProcess: ";
				std::string description;
				get_lasterror_description(lasterror, description);
				outputstring += description;
				ret = -1;
			}
			else {
				CloseHandle(hstdin_rd);
				hstdin_rd = NULL;
				CloseHandle(hstdout_wr);
				hstdout_wr = NULL;
				
				code = read_pipe(hstdout_rd, outputstring);
				if (code == 0) {
					/*TODO consider a timed wait */
					dwcode = WaitForSingleObject(proc.hProcess, 10*1000);
					if (dwcode == WAIT_FAILED) {
						lasterror = GetLastError();
						outputstring = "error in WaitForSingleObject: ";
						std::string description;
						get_lasterror_description(lasterror, description);
						outputstring += description;
						ret = -1;
					}

					DWORD exitcode = 0;
					bcode = GetExitCodeProcess(proc.hProcess, &exitcode);
					if (bcode == 0) {
						lasterror = GetLastError();
						outputstring = "error in GetExitCodeProcess: ";
						std::string description;
						get_lasterror_description(lasterror, description);
						outputstring += description;
						ret = -1;
					}
					else {
						if (status)
							*status = exitcode;
						ret = 0;
					}
				}
			}
		}
	}
	
	if (proc.hProcess)
		CloseHandle(proc.hProcess);
	if (proc.hThread)
		CloseHandle(proc.hThread);
	if (command_line)
		g_free(command_line);
	if (application_name)
		g_free(application_name);
	if (hstdin_rd)
		CloseHandle(hstdin_rd);
	if (hstdout_wr)
		CloseHandle(hstdout_wr);
	if (hstdin_wr)
		CloseHandle(hstdin_wr);
	if (hstdout_rd)
		CloseHandle(hstdout_rd);
	return ret;
}
