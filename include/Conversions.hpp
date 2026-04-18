#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <string>
#include <vector>
#include<filesystem>

bool flattenDirectory(std::string const& dirPath);
bool deleteFile(std::string const& filePath);
bool deleteDirectory(std::string const& dirPath);
std::vector<std::string> listFiles(std::string const& dirPath, std::string const& extension);
inline static std::string removeExtension(const std::string& filePath) { return std::filesystem::path(filePath).replace_extension().string(); }

bool convertOneCbzToCbr(std::string const& fileName);
bool convertOneCbrToCbz(std::string const& fileName);
bool convertMultipleCbzToCbr(std::vector<std::string> const& files);
bool convertMultipleCbrToCbz(std::vector<std::string> const& files);
inline static bool convertDirectoryOfCbzToDirectoryOfCbr(std::string const& directoryName) { return convertMultipleCbzToCbr(listFiles(directoryName, ".cbz")); }
inline static bool convertDirectoryOfCbrToDirectoryOfCbz(std::string const& directoryName) { return convertMultipleCbrToCbz(listFiles(directoryName, ".cbr")); }

#endif // CONVERSIONS_HPP