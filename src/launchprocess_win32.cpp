#include <cstdlib>
#include <string>
#include <sstream>
#include <glib.h>
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>

void get_lasterror_description(DWORD lasterrorcode, std::string& errorstring_utf8) 
{ 
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
	DWORD flags;
	
	flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS;

    FormatMessageW(flags, NULL, lasterrorcode,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPWSTR) &lpMsgBuf, 0, NULL);
	
	gchar *msgbuf_utf8 = NULL;
	g_utf16_to_utf8(lpMsgBuf, -1, );
	errorstring_utf8 = msgbuf_utf8;

	g_free(msgbuf_utf8);
    LocalFree(lpMsgBuf);
}

std::string util_argv_to_commandline(const std::vector<std::string>& argv) {
	std::string commandline;
	
	if (argv.ampty()) return "";

	commandline = argv[0];
	for (size_t i = 1; i < argv.size(), i++) {
		commandline += std::string(" ") + argv[i];
	}
	
	return commandline;
}

int launch_process(const std::vector<std::string>& argv, std::string& outputstring, bool want_stdout)
{
	SECURITY_ATTRIBUTES sec = {};
	PROCESS_INFORMATION proc = {};
	STARTUPINFO startp = {};
	HANDLE hstdout_rd = NULL;
	HANDLE hstdout_wr = NULL;
	DWORD lasterror = 0;
	BOOL bcode = 0;
	DWORD dwcode = 0;
	GError *errspec = NULL;
	int ret = 0;
	
	sec.nLength = sizeof(SECURITY_ATTRIBUTES);
	sec.bInheritHandle = TRUE;
	sec.lpSecurityDescriptor = NULL;
	
	// Create a pipe for the child process's STDOUT.
	CreatePipe(&hstdout_rd, &hstdout_wr, &sec, 0);
	
	// Ensure the read handle to the pipe for STDOUT is not inherited.
	SetHandleInformation(hstdout_rd, HANDLE_FLAG_INHERIT, 0)
	
	startup.cb = sizeof(STARTUP_INFO);
	startup.hStdOutput = hstdout_wr;
	startup.dwFlags |= STARTF_USESTDHANDLES;
	
	char *application_name = g_utf8_to_utf16(argv[0].c_str(), -1, NULL, NULL, &errspec);
	if (!application_name) {
		outputstring = "error converting text UTF-8 to UTF-16 text";
		if (errspec && errspec->message && errspec->message[0]) {
			outputstring += ": ";
			outputstring += errspec->message;
		}
		outputstring += "\n";
		ret = -1;
	}
	DWORD flags = CREATE_NO_WINDOW;
	bcode = CreateProcessW(application_name,
	                       util_argv_to_commandline(argv).c_str(),
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
		outputstring += "\n";
		ret = -1;
	}
	else {
		/*TODO consider a timed wait */
		dwcode = WaitForSingleObject(proc.hProcess, INFINITE);
		if (dwcode == WAIT_FAILED) {
			lasterror = GetLastError();
			outputstring = "error in WaitForSingleObject: ";
			std::string description;
			get_lasterror_description(lasterror, description);
			outputstring += description;
			outputstring += "\n";
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
			outputstring += "\n";
			ret = -1;
		}
		else {
			if (exitcode != 0 || want_stdout) {
				const DWORD sz = 100;
				char *buf = new char[sz];

				for (;;) {
					DWORD bytesread = 0;
					memset(buf, 0, sz);
					
					bret = ReadFile(hstdout_rd, buf, sz - 1, &bytesread, NULL);
					if (bret == 0) {
						lasterror = GetLastError();
						outputstring = "error in ReadFile: ";
						std::string description;
						get_lasterror_description(lasterror, description);
						outputstring += description;
						outputstring += "\n";
						ret = -1;
					}
					if (bytesread == 0) /* eof */
						break;
					
					outputstring += buf;
				}
				delete[] buf;
				buf = nullptr;
			}
			
			if (exitcode != 0) {
				ret = -1;
			}
		}
	}
	
	if (proc.hProcess)
		CloseHandle(proc.hProcess);
	if (proc.hThread)
		CloseHandle(proc.hThread);
	if (application_name)
		g_free(application_name);
	if (hstdout_rd)
		CloseHandle(hstdout_rd);
	if (hstdout_wr)
		CloseHandle(hstdout_wr);
	return ret;
}
