#pragma once


class Utils {
public:
  
  /** Converts a UTF16 string into a CP1252 string
   */
  static std::string convert(const std::wstring& text);

  /** This function handles a windows error (using GetLastError and FormatMessage).
   *  The error message is rethrown as std::exception
   */
  static void handleWinError();


  static bool confirm(const std::string& message);
};