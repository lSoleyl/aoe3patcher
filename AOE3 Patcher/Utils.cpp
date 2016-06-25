#include "pch.hpp"
#include "Utils.hpp"

#include <xercesc\util\XMLString.hpp>

using namespace std;
using namespace xercesc;

std::string Utils::convert(const std::wstring& text) {
  char* transcoded = XMLString::transcode(text.c_str());
  string result = transcoded;
  XMLString::release(&transcoded);
  return result;
}

void Utils::handleWinError() {
  DWORD ec = GetLastError();

  char* messageBuffer;
  FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, NULL, ec, NULL, (LPSTR)&messageBuffer, 0, NULL);
  std::string message = messageBuffer;
  LocalFree(messageBuffer);

  throw std::exception(message.c_str(), ec);
}


bool Utils::confirm(const std::string& question) {
  cout << question << " [y/n]: ";
  char choice;
  cin >> choice;
  if (choice == 'y' || choice == 'Y')
    return true;
  return false;
}