#include "pch.hpp"
#include "PatchData.hpp"
#include "ProtoPatcher.hpp"
#include "File.hpp"
#include "Utils.hpp"

#include <xercesc/util/PlatformUtils.hpp>


using namespace std;

/** Ensures, that the file exists and is writable.
 *  Returns true if the readonly-flag got cleared
 */
bool ensureFilePatchable(const std::string& fileName) {
  if (!File::exists(fileName))
    throw std::exception(("Missing '" + fileName + "'. Please launch this program in the AOE III directory!").c_str());

  return File::clearReadOnlyFlag(fileName);
}

std::pair<string, string> patchFiles("age3x.exe", "data\\protox.xml");

int main() {
  cout << "=== Age of Empires III:WC - unit cap patcher v1.1 ===" << endl << endl;

  try {
    std::string& fileName = patchFiles.first;

    try {
      xercesc::XMLPlatformUtils::Initialize();
    } catch (const xercesc::XMLException& exp) {
      throw std::exception(("Failed to initialize XML libary. Error: " + Utils::convert(exp.getMessage())).c_str());
    }

    //Test, whether files exist and we could write into it
    if (ensureFilePatchable(patchFiles.first) || ensureFilePatchable(patchFiles.second))
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
      if (!data.IsValid() && !Utils::confirm("[WARN] unknown code found in 'age3x.exe', file might get corrupted by this patch. Do you want to continue?"))
        return 0;

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

    
  } catch(std::exception& e) {
    cerr << endl << "[ERR] " << e.what() << endl;
    cerr << "Patching failed!" << endl;
  } catch(...) {
    cerr << endl << "[ERR] caught unknown exception " << endl;
  }

  xercesc::XMLPlatformUtils::Terminate();
  system("PAUSE");
  return 0;  
}