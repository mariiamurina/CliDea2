#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>

LPCWSTR AnsiToWide(const LPSTR ansiString);

HANDLE open(PCHAR* lpBuffer, const LPSTR& name);
HANDLE open(HANDLE** pSharedMem, const LPSTR& name);

bool get_idea(HANDLE& hPipeWrite, PCHAR& pSharedMem);
void into_vector(PCHAR& hSharedMem, std::vector<std::string>& _ideas);
void into_vector(PCHAR& pSharedMem, std::vector<int>& ranks);
void show_ideas(std::vector<std::string>& _ideas);
void vote_for_ideas(HANDLE& hPipeWrite, const unsigned int size);
void show_top(std::vector<std::string>& _ideas, std::vector<int>& ranks);
