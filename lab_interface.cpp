#include "lab_interface.h"
#include <sstream>
#include <string>

// OBJECTS
//--------------------------------------------------
Clients::Clients(const int size, std::string str, HANDLE& pipe_write) : m_Size(size) {
	for (int i{ 0 }; i < m_Size; ++i) {
		m_Processes.push_back(Process());

		ZeroMemory(&m_Processes[i].si, sizeof(m_Processes[i].si));
		m_Processes[i].si.cb = sizeof(STARTUPINFOA);
		m_Processes[i].si.dwFlags |= STARTF_USESTDHANDLES;
		ZeroMemory(&m_Processes[i].pi, sizeof(m_Processes[i].pi));

		std::string cmdLineStr{ "Client.exe " + str + " "};
		std::ostringstream oss;
		oss << reinterpret_cast<uintptr_t>(pipe_write);
		cmdLineStr += oss.str();
		if (!CreateProcessA(
			NULL,
			LPSTR(cmdLineStr.c_str()),
			NULL,
			NULL,
			TRUE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&m_Processes[i].si,
			&m_Processes[i].pi
		)) {
			std::cout << "\n\n" << GetLastError() << "\n\n";
		}
		else {
			std::cout << "\n\nAll OK! Process created!\n\n";
		}
	}
}
//--------------------------------------------------


// FUNCTIONS
//--------------------------------------------------
void msg_Greetings(int& _n, std::string& _name, int& _dur) {
	std::cout << "Hello! Choose count of clients: ";
	std::cin >> _n;
	std::cout << "Enter file map name: ";
	std::cin >> _name;
	std::cout << "Enter generating ideas duration: ";
	std::cin >> _dur;
}

LPCWSTR StringToLPCWSTR(const std::string& str) {
	// Calculate the required buffer size for the wide character string
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	// Allocate buffer for the wide character string
	wchar_t* buffer = new wchar_t[size];

	// Convert the narrow character string to wide character string
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, size);

	// Return the pointer to the wide character string
	return buffer;
}

HANDLE create(const std::string& str) {
	HANDLE hTemp{ CreateFileMapping(
				INVALID_HANDLE_VALUE,
				NULL,
				PAGE_READWRITE,
				0,
				126,
				StringToLPCWSTR(str)
			)
	};
	if (!hTemp) {
		std::cout << "SERVER CREATE FILE MAP:\n\tError occured: " << GetLastError() << "\n";
	}

	return hTemp;
}

LPCWSTR AnsiToWide(const LPSTR ansiString) {
	// Calculate the required buffer size for the wide character string
	int size = MultiByteToWideChar(CP_ACP, 0, ansiString, -1, NULL, 0);

	// Allocate buffer for the wide character string
	wchar_t* wideString = new wchar_t[size];

	// Convert the ANSI string to wide character string
	MultiByteToWideChar(CP_ACP, 0, ansiString, -1, wideString, size);

	// Return the pointer to the wide character string
	return wideString;
}

HANDLE open(PCHAR* lpBuffer, const LPSTR& name) {
	HANDLE hFileMap{ OpenFileMapping(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			AnsiToWide(name)
		)
	};

	if (!hFileMap) {
		std::cout << "Error occured: " << GetLastError() << "\n";
		return hFileMap;
	}

	*lpBuffer = (PCHAR)MapViewOfFile(
		hFileMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		128
	);
	if (!*lpBuffer) {
		std::cout << "Error occured: " << GetLastError() << "\n";
		CloseHandle(hFileMap);
		return NULL;
	}

	return hFileMap;
}

HANDLE open(HANDLE** pSharedMem, const LPSTR& name) {
	HANDLE hFileMap{ OpenFileMapping(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			AnsiToWide(name)
		)
	};

	if (!hFileMap) {
		std::cout << "Error occured: " << GetLastError() << "\n";
		return hFileMap;
	}

	*pSharedMem = static_cast<HANDLE*>(MapViewOfFile(
		hFileMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(HANDLE)
	));
	if (!*pSharedMem) {
		std::cout << "Error occured: " << GetLastError() << "\n";
		CloseHandle(hFileMap);
		return NULL;
	}

	return hFileMap;
}
