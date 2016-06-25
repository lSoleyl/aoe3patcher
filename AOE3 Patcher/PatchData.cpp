#include "PatchData.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

namespace constants {
  std::pair<size_t, size_t> popCapAddr(0x278BD/*200*/, 0x278F0/*250*/);

  size_t cmpPos = 0x278DB;
  std::string cmpCode("\x0f\x8f\x8c\x10\x08\x00", 6);

  char nop = '\x90';

  std::string nopSled(6, nop);
}

PatchData::PatchData() : cmpCode(constants::cmpCode), populationCaps(200,250) {}

PatchData::PatchData(std::ifstream& source) {  
  source.seekg(constants::popCapAddr.first);
  source.read(reinterpret_cast<char*>(&populationCaps.first), 4);
  source.seekg(constants::popCapAddr.second);
  source.read(reinterpret_cast<char*>(&populationCaps.second), 4);

  vector<char> buffer(constants::cmpCode.length());
  source.seekg(constants::cmpPos);
  source.read(buffer.data(), buffer.size());
  cmpCode.assign(buffer.begin(), buffer.end());
}


bool PatchData::IsPatched() const {
  return *this != PatchData(); //Check against original data
}
 
int PatchData::CurrentPopulationCap() const {
  return std::min(populationCaps.first, populationCaps.second);
}

void PatchData::SetPopulation(int populationCap) {
  populationCaps = make_pair(populationCap, populationCap);
  
  //Replace compare code by nop sled
  cmpCode = constants::nopSled;
}


void PatchData::Restore() {
  *this = PatchData();
}

void PatchData::PrintReport() {
  cout << (IsPatched() ? "PATCHED" : "Original") << " AOE with Population Cap of : " << CurrentPopulationCap() << endl;
}

void PatchData::PatchFile(std::ofstream& out) {
  out.seekp(constants::popCapAddr.first);
  out.write(reinterpret_cast<char*>(&populationCaps.first), 4);
  out.seekp(constants::popCapAddr.second);
  out.write(reinterpret_cast<char*>(&populationCaps.second), 4);
  out.seekp(constants::cmpPos);
  out.write(cmpCode.data(), cmpCode.length());
  out.close();
}

bool PatchData::operator==(const PatchData& other) const {
  return cmpCode == other.cmpCode && populationCaps == other.populationCaps;
}

bool PatchData::operator!=(const PatchData& other) const {
  return !(*this == other);
}

bool PatchData::IsValid() const {
  return cmpCode == constants::cmpCode || cmpCode == constants::nopSled;
}

PatchData& PatchData::operator=(const PatchData& other) {
  cmpCode = other.cmpCode;
  populationCaps = other.populationCaps;
  return *this;
}