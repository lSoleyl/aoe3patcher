#include <sys/stat.h>

#define XML_LIBRARY 
#define XERCES_STATIC_LIBRARY

#include <iostream>
#include <fstream>
#include <string>
	
#include <Windows.h>

#include <xercesc/util/PlatformUtils.hpp>

#include "PatchData.hpp"
#include "ProtoPatcher.hpp"

using namespace std;

bool fileExists(const std::string& name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

extern std::string convert(const std::wstring& source);


void handleWinError() {
  DWORD ec = GetLastError();

  char* messageBuffer;
  FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, NULL, ec, NULL, (LPSTR)&messageBuffer, 0, NULL);
  std::string message = messageBuffer;
  LocalFree(messageBuffer);

  throw std::exception(message.c_str(), ec);
}



void dropReadonlyFlag(const std::string& fileName) {
  DWORD flags = GetFileAttributesA(fileName.c_str());

  if (flags == INVALID_FILE_ATTRIBUTES)
    handleWinError();

  if ((flags & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY) { //we have to clear the flag
    cout << "[INFO] Clearing readonly flag on '" << fileName << "'..." << endl;
    if (!SetFileAttributesA(fileName.c_str(), flags & ~FILE_ATTRIBUTE_READONLY))
      handleWinError();
  }
}

void ensureFilePatchable(const std::string& fileName) {
  if (!fileExists(fileName))
    throw std::exception(("Missing '" + fileName + "'. Please launch this program in the AOE III directory!").c_str());

  dropReadonlyFlag(fileName);
}

std::pair<string, string> patchFiles("age3x.exe", "data\\protox.xml");

int main() {
  try {
    std::string& fileName = patchFiles.first;

    try {
      xercesc::XMLPlatformUtils::Initialize();
    } catch (const xercesc::XMLException& exp) {
      throw std::exception(("Failed to initialize XML libary. Error: " + convert(exp.getMessage())).c_str());
    }

    //Test, whether files exist and we could write into it
    ensureFilePatchable(patchFiles.first);
    ensureFilePatchable(patchFiles.second);
    cout << endl;

    {
      ifstream source(fileName, ios::binary);
      if (!source)
        throw std::exception(("Cannot open " + fileName + ". File exists, but is not readable?").c_str());

      PatchData data(source);
      source.close();

      ProtoPatcher proto;

      cout << "Found ";
      data.PrintReport();
      cout << "Build-Limit for houses is " << (proto.HasLimits() ? "enabled" : "disabled") << endl << endl;

      int newCap;
      cout << "Enter new population cap, or 0 to restore the original population cap and unpatch the file again. " << endl;
      cin >> newCap;


      if (newCap > 0) {
        data.SetPopulation(newCap);
        proto.RemoveLimits();
      } else {
        data.Restore();
        proto.RestoreLimits();
      }

      ofstream out(fileName, ios::in | ios::binary); //Additional ios::in to prevent truncation

      if (!out)
        throw std::exception(("Couldn't open " + fileName + " for writing. Check file permissions for 'age3x.exe'").c_str());

      data.PatchFile(out);

      cout << "Successfully changed population cap and build-limit." << endl;
    }

    xercesc::XMLPlatformUtils::Terminate();

    system("PAUSE");
    return 0;
  } catch(std::exception& e) {
    cerr << endl << "[ERR] " << e.what() << endl;
    cerr << "Patching failed!" << endl;
    system("PAUSE");
  } catch(...) {
    cerr << endl << "[ERR] caught unknown exception " << endl;
  }
}