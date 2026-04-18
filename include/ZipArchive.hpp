#ifndef DEF_ZIPARCHIVE
#define DEF_ZIPARCHIVE

#include <string>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <zip.h>

class ZipException : public std::runtime_error
{
	public:
		using std::runtime_error::runtime_error;
};

class ZipArchive
{
	public:
		enum class Mode
		{
			Read,
			Write,
			Append
		};
		
		ZipArchive(std::string const& path, Mode mode);
		ZipArchive(ZipArchive&& src) noexcept;
		ZipArchive(ZipArchive const& src) = delete;
		~ZipArchive();
		
		ZipArchive& operator=(ZipArchive const& src) = delete;
		ZipArchive& operator=(ZipArchive&& src) noexcept;
		
		void addFile(std::string const& archivePath, std::vector<uint8_t> const& data);
		inline void addText(std::string const& archivePath, std::string const& text) { addFile(archivePath, std::vector<uint8_t>(text.begin(), text.end())); }
		void addFileFromDisk(std::filesystem::path const& filePath, std::string const& archivePath);
		inline void addFileFromDisk(std::string const& filePath, std::string const& archivePath) { addFileFromDisk(std::filesystem::path(filePath), archivePath); }
		std::vector<uint8_t> readFile(std::string const& fileName) const;
		std::string readText(std::string const& fileName) const;
		std::vector<std::string> listFiles() const;
		void extractFile(std::string const& fileName, std::filesystem::path const& output) const;
		inline void extractFile(std::string const& fileName, std::string const& output) const { extractFile(fileName, std::filesystem::path(output)); }
		void extractAll(std::filesystem::path const& outputDir) const;
		inline void extractAll(std::string const& outputDir) const { extractAll(std::filesystem::path(outputDir)); }
	
	private:
		zip_t* m_archive = nullptr;
		
		[[noreturn]] void throw_error(std::string const& where) const
		{
			std::string err = m_archive ? zip_strerror(m_archive) : "unknown";
			
			throw ZipException(where + "failed: " + err);
		}
};

#endif // DEF_ZIPARCHIVE