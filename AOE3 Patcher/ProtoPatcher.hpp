#pragma once

class ProtoPatcher {
public:
  ProtoPatcher();
  ~ProtoPatcher();

  bool HasLimits() const;

  //These function can toggle the build limits for houses and will also write them back into the file
  void RemoveLimits();
  void RestoreLimits();
  
private:
  void SaveToFile();

  struct Internals;
  Internals* data;
};