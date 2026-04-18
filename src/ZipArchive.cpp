#include "../include/ZipArchive.hpp"

#include <fstream>

ZipArchive::ZipArchive(std::string const& path, Mode mode) : m_archive(nullptr)
{
	int flags = 0;
	
	switch(mode)
	{
		case Mode::Read :
			flags = ZIP_RDONLY;
			break;
		
		case Mode::Write :
			flags = ZIP_CREATE | ZIP_TRUNCATE;
			break;
		
		case Mode::Append :
			flags = ZIP_CREATE;
			break;
	}
	
	int err = 0;
	m_archive = zip_open(path.c_str(), flags, &err);
	
	if(!m_archive)
		throw ZipException("Failed to open zip : " + path);
}

ZipArchive::ZipArchive(ZipArchive&& src) noexcept
{
	m_archive = src.m_archive;
	src.m_archive = nullptr;
}

ZipArchive::~ZipArchive()
{
	if(m_archive)
		zip_close(m_archive);
}


ZipArchive& ZipArchive::operator=(ZipArchive&& src) noexcept
{
	if(this != &src)
	{
		if(m_archive)
			zip_close(m_archive);
		
		m_archive = src.m_archive;
		src.m_archive = nullptr;
	}
	
	return *this;
}


void ZipArchive::addFile(std::string const& archivePath, std::vector<uint8_t> const& data)
{
	zip_source_t* src = zip_source_buffer(m_archive, data.data(), data.size(), 0);
	
	if(!src)
		throw_error("zip_source_buffer");
	
	if(zip_file_add(m_archive, archivePath.c_str(), src, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0)
	{
		zip_source_free(src);
		
		throw_error("zip_file_add");
	}
}

void ZipArchive::addFileFromDisk(std::filesystem::path const& filePath, std::string const& archivePath)
{
	zip_source_t* src = zip_source_file(m_archive, filePath.string().c_str(), 0, 0);
	
	if(!src)
		throw_error("zip_source_file");
	
	if(zip_file_add(m_archive, archivePath.c_str(), src, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0)
	{
		zip_source_free(src);
		
		throw_error("zip_file_add");
	}
}

std::vector<uint8_t> ZipArchive::readFile(std::string const& fileName) const
{
	zip_stat_t st{};
	
	if(zip_stat(m_archive, fileName.c_str(), 0, &st) != 0)
		throw_error("zip_stat");
	
	zip_file_t* file = zip_fopen(m_archive, fileName.c_str(), 0);
	
	if(!file)
		throw_error("zip_fopen");
	
	std::vector<uint8_t> buffer(st.size);
	
	zip_fread(file, buffer.data(), buffer.size());
	zip_fclose(file);
	
	return buffer;
}

std::string ZipArchive::readText(std::string const& fileName) const
{
	std::vector<uint8_t> data = readFile(fileName);
	
	return std::string(data.begin(), data.end());
}

std::vector<std::string> ZipArchive::listFiles() const
{
	std::vector<std::string> result;
	
	zip_int64_t count = zip_get_num_entries(m_archive, 0);
	
	for(zip_uint64_t i=0;i<count;++i)
	{
		char const* fileName = zip_get_name(m_archive, i, 0);
		
		if(fileName)
			result.emplace_back(fileName);
	}
	
	return result;
}

void ZipArchive::extractFile(std::string const& fileName, std::filesystem::path const& output) const
{
	std::vector<uint8_t> data = readFile(fileName);
	
	std::filesystem::create_directories(output.parent_path());
	
	std::ofstream ofs(output, std::ios::binary);
	
	if(!ofs.is_open())
		throw ZipException("Failed to write file: " + output.string());
	
	ofs.write(reinterpret_cast<char const*>(data.data()), data.size());
}

void ZipArchive::extractAll(std::filesystem::path const& outputDir) const
{
	for(std::string const& name : listFiles())
	{
		std::filesystem::path outPath = outputDir / name;
		
		if(!name.empty() && name.back() == '/')
		{
			std::filesystem::create_directories(outPath);
			
			continue;
		}
		
		extractFile(name, outPath);
	}
}