#pragma once

#include <fstream>
#include <cstdint>

class PatchData {
public:
  /** This constructor instantiates the data with the unpatched version
   */
  PatchData();

  /** This constructor reads the current patch data state from the given file stream
   */
  PatchData(std::ifstream& source);


  bool IsPatched() const;
  int CurrentPopulationCap() const;

  /** Sets the population cap to the desired value
   */
  void SetPopulation(int populationCap);

  /** Retores the patch state to the original values. 
   */
  void Restore();

  /** Prints a report of the current patch state to std::cout
   */
  void PrintReport();

  /** Actually patches the file.
   */
  void PatchFile(std::ofstream& out);


  bool operator==(const PatchData& other) const;
  bool operator!=(const PatchData& other) const;
  PatchData& operator=(const PatchData& other);
private:


  std::string cmpCode;
  std::pair<int32_t, int32_t> populationCaps;
  std::string fileName;
};

