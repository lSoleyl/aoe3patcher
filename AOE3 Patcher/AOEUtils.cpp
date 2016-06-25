#include "pch.hpp"
#include "AOEUtils.hpp"




AOEUtils::REG_KEY::REG_KEY(HKEY handle) : handle(handle) {}

AOEUtils::REG_KEY::REG_KEY(REG_KEY&& from) : handle(NULL) {
  std::swap(handle, from.handle);
}
AOEUtils::REG_KEY::~REG_KEY() {
  if (handle)
    RegCloseKey(handle);
}

AOEUtils::REG_KEY::operator HKEY() const {
  return handle;
}

AOEUtils::REG_KEY::operator bool() const {
  return handle != NULL;
}

AOEUtils::REG_KEY AOEUtils::OpenRootKey() {
  HKEY handle;
  auto result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Microsoft Games\\Age of Empires 3 Expansion Pack\\1.0", NULL, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &handle);
  if (result == ERROR_SUCCESS)
    return REG_KEY(handle);
  return REG_KEY(NULL);
}

std::string AOEUtils::GetInstallDir() {
  return ReadStringValue("SetupPath");
}

std::string AOEUtils::GetVersion() {
  return ReadStringValue("Version");
}

std::string AOEUtils::ReadStringValue(const std::string& name) {
  auto handle = OpenRootKey();
  if (!handle)
    throw std::exception("Cannot access game's root registry key.");

  std::vector<char> buffer(MAX_PATH);
  DWORD bufferSize = static_cast<DWORD>(buffer.size());
  auto result = RegQueryValueExA(handle, name.c_str(), NULL, NULL, (BYTE*)buffer.data(), &bufferSize);
  if (result != ERROR_SUCCESS)
    throw std::exception(("Couldn't access the '" + name + "' registry key.").c_str());
  
  return std::string(buffer.data());
}
