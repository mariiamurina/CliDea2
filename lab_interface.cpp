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
void get_ideas(HANDLE& hPipeRead, std::vector<std::string>& _ideas, PCHAR& pSharedMem) {
  char temp_str[1000];
  DWORD dwBytesReaden;
  if (ReadFile(hPipeRead, temp_str, 1000, &dwBytesReaden, NULL)) {
    //std::string str(temp_str);
    temp_str[dwBytesReaden] = 0;
    CopyMemory(pSharedMem + 1, temp_str, strlen(temp_str));  
    //std::cout << temp_str << "\n\n";
    into_vector(temp_str, _ideas);
  }
  else {
    std::cout << "\n\nERROR READING PIPE!!!\n\n";
  }
}

void into_vector(char temp_str[1000], std::vector<std::string>& _ideas) {
  int i{ 0 };
  int j{ 0 };
  while (temp_str[j] != 0) {
    if (i != j && temp_str[j] == '\n') {
      _ideas.push_back(std::string(temp_str + i, j - i));
      i = j + 1;
      j = i + 1;
    }
    else {
      ++j;
    }
  }
}

void show_ideas(std::vector<std::string>& _ideas) {
  for (int i{ 0 }; i < _ideas.size(); ++i) {
    std::cout << i << ". " << _ideas[i] << "\n";
  }
}

bool create_pipe(PHANDLE write_pipe, PHANDLE read_pipe) {
  SECURITY_ATTRIBUTES saAttr;

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  return CreatePipe(
    read_pipe,
    write_pipe,
    &saAttr,
    0
  );
}

void into_vector(char temp_str[1000], std::vector<int>& ranks) {
  int i{ 0 };
  int j{ 0 };
  while (temp_str[j] != 0) {
    if (i != j && temp_str[j] == '\n') {
      ++ranks[*(temp_str + i)];
      i = j + 1;
      j = i + 1;
    }
    else {
      ++j;
    }
  }

  //std::cout << "\n\n";
  //for (int i{ 0 }; i < ranks.size(); ++i) {
  //  std::cout << i << " " << ranks[i] << "\n";
  //}
  //std::cout << "\n\n";
}

int min_in_top(int max[3], std::vector<int> ranks) {
  int num{ 0 };

  if (ranks[max[num]] > ranks[max[1]] && max[num] != max[1]) num = 1;
  if (ranks[max[num]] > ranks[max[2]] && max[num] != max[2]) num = 2;

  return num;
}

std::string show_top(std::vector<std::string>& _ideas, std::vector<int>& ranks) {
  int max[3]{ 0, 1, 2 };

  for (int i{ 0 }; i < ranks.size(); ++i) {
    if (i == max[0]  i == max[1]  i == max[2])
      continue;

    else if (ranks[i] > ranks[max[min_in_top(max, ranks)]]) {
      max[min_in_top(max, ranks)] = i;
    }
  }

  std::string temp{ "" };
  for (const int& idx : max) {
    temp += _ideas[idx] + "\n";
  }

  return temp;
}

void write_top(HANDLE& hPipeRead, std::vector<std::string>& _ideas, PCHAR& pSharedMem) {
	std::vector<int> ranks(_ideas.size());

	char temp_str[1000];
	DWORD dwBytesReaden;
	if (ReadFile(hPipeRead, temp_str, 1000, &dwBytesReaden, NULL)) {
		temp_str[dwBytesReaden] = 0;
		// std::cout << temp_str << "\n";
		into_vector(temp_str, ranks);

		std::string temp(show_top(_ideas, ranks));
		temp.push_back(0);

		// Запис результатів у файл result.txt
		std::ofstream resultFile("result.txt", std::ios::app);
		resultFile << temp;
		resultFile.close();

		const char* str = temp.c_str();
		CopyMemory(pSharedMem + 1, str, strlen(str) + 1);
	}
}

//--------------------------------------------------
