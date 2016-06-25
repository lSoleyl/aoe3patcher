#pragma once

class AOEUtils {
public:


  static std::string GetInstallDir();
  static std::string GetVersion();


private:
  //RAII struct for HKEY
  struct REG_KEY {
    REG_KEY(HKEY handle);
    REG_KEY(REG_KEY&& from);
    ~REG_KEY();

    operator HKEY() const;
    operator bool() const;
  private:
    HKEY handle;
  };

  static REG_KEY OpenRootKey();
  static std::string ReadStringValue(const std::string& name);
};