#include "client_interface.h"
#include <Windows.h>
#include <string>



//FUNCTIONS
//--------------------------------------------------
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
		256
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

bool get_idea(HANDLE& hPipeWrite, PCHAR& pSharedMem) {
	while (1) {
		char* str = new char[2];
		CopyMemory(str, pSharedMem, 1);
		if (str[0] != 'W') {
			*pSharedMem += 1;
			return TRUE;
		}
		else if (hPipeWrite) {
			std::string temp_str;
			getline(std::cin, temp_str);
			temp_str.push_back('\n');
			DWORD dwBytes;
			if (!WriteFile(hPipeWrite, temp_str.c_str(), (DWORD)temp_str.size(), &dwBytes, nullptr)) {
				std::cout << "\n\nERROR WRITING INTO PIPE\n\n";
			}
		}
	}
}

void into_vector(PCHAR& pSharedMem, std::vector<std::string>& _ideas) {
	int i{ 1 };
	int j{ 1 };
	char* temp_str = pSharedMem;
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

void vote_for_ideas(HANDLE& hPipeWrite, const unsigned int size) {
	std::cout << "\nWrite 3 indexs for vote: ";
	std::string str{ "" };
	DWORD dwBytes;
	for (int i{ 0 }; i < 3; ++i) {
		unsigned int n{ 0 };
		std::cin >> n;
		if (n >= size) {
			--i;
			continue;
		}
		str.push_back(n);
		str.push_back('\n');
	}
	if (!WriteFile(hPipeWrite, str.c_str(), (DWORD)str.size(), &dwBytes, nullptr)) {
		std::cout << "\n\nERROR WRITING INTO PIPE\n\n";
	}
}

void into_vector(PCHAR& pSharedMem, std::vector<int>& ranks) {
	int i{ 1 };
	int j{ 1 };
	while (pSharedMem[j] != 0) {
		if (i != j && pSharedMem[j] == '\n') {
			++ranks[*(pSharedMem + i)];
			i = j + 1;
			j = i + 1;
		}
		else {
			++j;
		}
	}
}

int min_in_top(int max[3], std::vector<int>& ranks) {
	int num{ 0 };

	if (ranks[max[num]] > ranks[max[1]] && max[num] != max[1]) num = 1;
	if (ranks[max[num]] > ranks[max[2]] && max[num] != max[2]) num = 2;

	return num;
}

void show_top(std::vector<std::string>& _ideas, std::vector<int>& ranks) {
	int max[3]{ 0, 1, 2 };

	for (int i{ 0 }; i < ranks.size(); ++i) {
		if (i == max[0] || i == max[1] || i == max[2])
			continue;

		else if (ranks[i] > ranks[max[min_in_top(max, ranks)]]) {
			max[min_in_top(max, ranks)] = i;
		}
	}

	for (const int& idx : max) {
		std::cout << idx << ". " << _ideas[idx] << "\n";
	}
}
//--------------------------------------------------