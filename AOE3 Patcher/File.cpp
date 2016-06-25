#include "pch.hpp"
#include "File.hpp"
#include "Utils.hpp"

#include <sys/stat.h>	

using namespace std;

bool File::exists(const std::string& name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

bool File::clearReadOnlyFlag(const std::string& fileName) {
  DWORD flags = GetFileAttributesA(fileName.c_str());

  if (flags == INVALID_FILE_ATTRIBUTES)
    Utils::handleWinError();

  if ((flags & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY) { //we have to clear the flag
    cout << "[INFO] Clearing readonly flag on '" << fileName << "'..." << endl;
    if (!SetFileAttributesA(fileName.c_str(), flags & ~FILE_ATTRIBUTE_READONLY))
      Utils::handleWinError();
    return true;
  }

  return false;
}