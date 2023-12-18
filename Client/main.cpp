#include <iostream>
#include <vector>
#include <sstream>
#include <Windows.h>
#include "client_interface.h"

//----------CLIENT----------

int main(int argc, char** argv) {

	const LPSTR fileMapName{ argv[1] };
	std::string hPipeWriteStr{ argv[2] };

	uintptr_t handleValue;
	std::istringstream iss(hPipeWriteStr);
	iss >> handleValue;
	HANDLE hPipeWrite = reinterpret_cast<HANDLE>(handleValue);

	HANDLE hFileMap;
	PCHAR pSharedMem;

	std::vector<std::string> Ideas;
		
	if (!(hFileMap = open(&pSharedMem, fileMapName))) {
		return 0;
	}

	while (1) {
		switch (*pSharedMem) {
		case 'W':
			std::cout
				<< "- CLIENT\n\n\n"
				<< "Hello! Write your ideas below (1 line - 1 idea):\n";
			get_idea(hPipeWrite, pSharedMem);
			break;
		case '0':
			std::cout << "\n\t- End of generating\n\n";
			Sleep(1000);

			system("cls");
			std::cin.sync();

			into_vector(pSharedMem, Ideas);

			show_ideas(Ideas);

			vote_for_ideas(hPipeWrite, (unsigned int)Ideas.size());

			*pSharedMem += 1;
			break;
		case 'A':
			std::cout << "\n\t- End of voting\n\n";
			Sleep(1000);

			std::cin.sync();

			std::cout << pSharedMem + 1;

			*pSharedMem += 1;

			Sleep(1000);
			break;
		default:
			break;

		}
	}


	Sleep(5000);

	return 0;
}
