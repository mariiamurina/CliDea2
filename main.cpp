`//----------SERVER----------

#include <iostream>
#include <vector>
#include <Windows.h>
#include <chrono>
#include "lab_interface.h"

#define DURATION(a, b)  (std::chrono::duration_cast<std::chrono::seconds>(a - b).count())
#define CUR_TIME    (std::chrono::high_resolution_clock::now())

int main(void) {

  std::cout << "- SERVER\n\n\n";

  HANDLE hFileMap;
  HANDLE hPipeWrite;
  HANDLE hPipeRead;
  PCHAR pSharedMem;
  BOOL bResult;
  
  std::string fileMapName{ "file" };
  int iCount;
  int iTimerDuration;

  if (!create_pipe(&hPipeWrite, &hPipeRead)) {
    return 0;
  }

  msg_Greetings(iCount, fileMapName, iTimerDuration);

  if (!(hFileMap = create(fileMapName))) {
    return 0;
  }
    
  if (!(hFileMap = open(&pSharedMem, (LPSTR)fileMapName.c_str()))) {
    return 0;
  }



  // Записуємо слово "W" у File Map Obj
  //*pSharedMem = hPipeWrite;

  CopyMemory(pSharedMem, "W", 1);

  // Створення клієнтів к-ті iCount
  Clients clients(iCount, fileMapName, hPipeWrite);

  // Вектор для зберігання отриманих ідей
  std::vector<std::string> Ideas;


  //auto GENERATING_START_TIME = CUR_TIME;
  //while (1) {
  //  if (DURATION(CUR_TIME, GENERATING_START_TIME) >= iTimerDuration) {
  //    *pSharedMem = NULL;
  //    break; 
  //  }
  //}
  Sleep(iTimerDuration * 1000);

  CopyMemory(pSharedMem, "0", 1);

  while (*pSharedMem != '0' + iCount) {
    //waiting
  }

  CopyMemory(pSharedMem, "0", 1);

  get_ideas(hPipeRead, Ideas, pSharedMem);

  show_ideas(Ideas);

  while (*pSharedMem != '0' + iCount) {
    //waiting
  }

  write_top(hPipeRead, Ideas, pSharedMem);
  CopyMemory(pSharedMem, "A", 1);

  while (*pSharedMem != 'A' + iCount) {
    //waiting
  }

  std::cout << "\n\t- SUCCESS END!\n\n";

  bResult = UnmapViewOfFile(pSharedMem);

  if(!bResult) {
    std::cout << "SERVER CLOSE FILE MAP VIEW: Error occured: " << GetLastError() << "\n" << bResult << "\n";
    return 1;
  }

  /*std::cout << "\n\n";
  for (int i{ 0 }; i < Ideas.size(); ++i) {
    std::cout << Ideas[i] << "\n";
  }*/


  CloseHandle(hFileMap);

  return 0;
}
