#pragma once

class File {
public:
  /** This function checks a file's existence.
   *
   * @param path the file to check
   *
   * @return true if the file exists, false otherwise
   */
  static bool exists(const std::string& path);

  /** This function tries to clear the readonly flag for the specified file.
   *  
   * @param path the file to make readable
   *
   * @return true if the flag was cleared, false if file was already readable
   * @throws std::exception if reading or writing the flags has failed.
   */
  static bool clearReadOnlyFlag(const std::string& path);



};