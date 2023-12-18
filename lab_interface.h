#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

// OBJECTS
//--------------------------------------------------
struct Process {
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
};

class Clients {
public:
  Clients(const int size, std::string str, HANDLE& pipe_write);



private:
  int m_Size;
  std::vector<Process> m_Processes;
  std::wstring commandLine{ L"Client.exe" };
};
//--------------------------------------------------


// FUNCTIONS
//--------------------------------------------------
void msg_Greetings(int& _n, std::string& _name, int& _dur);

LPCWSTR AnsiToWide(const LPSTR ansiString);

LPCWSTR StringToLPCWSTR(const std::string& str);

HANDLE create(const std::string& str);

HANDLE open(PCHAR* lpBuffer, const LPSTR& name);
HANDLE open(HANDLE** pSharedMem, const LPSTR& name);

void get_ideas(HANDLE& hPipeRead, std::vector<std::string>& _ideas, PCHAR& pSharedMem);

bool create_pipe(PHANDLE write_pipe, PHANDLE read_pipe);

void into_vector(char temp_str[1000], std::vector<std::string>& _ideas);

void show_ideas(std::vector<std::string>& _ideas);

void write_top(HANDLE& hPipeRead, std::vector<std::string>& _ideas, PCHAR& pSharedMem);

std::string show_top(std::vector<std::string>& _ideas, std::vector<int>& ranks);


//--------------------------------------------------
