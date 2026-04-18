#include "../include/Conversions.hpp"

#include <algorithm>
#include <RarLib/Rar.hpp>
#include <RarLib/Unrar.hpp>
#include "../include/ZipArchive.hpp"

namespace fs = std::filesystem;

std::string toLower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
	
	return s;
}

bool extensionMatches(fs::path const& p, std::string const& ext)
{
	if(ext.empty())
		return true;
	
	#ifdef _WIN32
		return toLower(p.extension().string()) == toLower(ext);
	#else
		return p.extension() == ext;
	#endif
}

bool flattenDirectory(std::string const& dirPath)
{
	try
	{
		fs::path root(dirPath);
		
		if(!fs::exists(root) || !fs::is_directory(root))
			return false;
		
		for(fs::recursive_directory_iterator it=fs::recursive_directory_iterator(root);it!=fs::recursive_directory_iterator();++it)
		{
			fs::path const& current = it->path();
			
			if(fs::is_regular_file(current))
			{
				fs::path destination = root / current.filename();
				
				if(current.parent_path() == root)
					continue;
				
				fs::copy_file(current, destination, fs::copy_options::overwrite_existing);
				
				fs::remove(current);
			}
			
			for(fs::recursive_directory_iterator it=fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied);it!=fs::recursive_directory_iterator();)
			{
				fs::path const current = it->path();
				++it;
				
				if(fs::is_directory(current) && fs::is_empty(current))
					fs::remove(current);
			}
		}
		
		return true;
	}
	catch(std::exception const& err)
	{
		return false;
	}
	
	return true;
}

bool deleteFile(std::string const& filePath)
{
	try
	{
		fs::path p(filePath);
		
		if(!fs::exists(p) || !fs::is_regular_file(p))
			return false;
		
		return fs::remove(p);
	}
	catch(...)
	{
		return false;
	}
}

bool deleteDirectory(std::string const& dirPath)
{
	try
	{
		fs::path p(dirPath);
		
		if(!fs::exists(p) || !fs::is_directory(p))
			return false;
		
		fs::remove_all(p);
		
		return true;
	}
	catch(...)
	{
		return false;
	}
}

std::vector<std::string> listFiles(std::string const& dirPath, std::string const& extension)
{
	std::vector<std::string> result;
	
	try
	{
		fs::path root(dirPath);
		
		if(!fs::exists(root) || !fs::is_directory(root))
			return result;
		
		for(fs::directory_entry const& entry : fs::directory_iterator(root))
		{
			if(!fs::is_regular_file(entry))
				continue;
			
			fs::path const& p = entry.path();
			
			if(!extensionMatches(p, extension))
				continue;
			
			result.push_back(p.string());
		}
	}
	catch(...)
	{
		return result;
	}
	
	return result;
}

bool convertOneCbzToCbr(std::string const& fileName)
{
	std::string tempDirectoryName = removeExtension(fileName);
	ZipArchive* za = new ZipArchive(fileName, ZipArchive::Mode::Read);
	
	za->extractAll(tempDirectoryName);
	
	delete za;
	za = nullptr;
	
	if(!deleteFile(fileName))
		return false;
	
	if(!flattenDirectory(tempDirectoryName))
		return false;
	
	Rar rar;
	
	if(!rar.compressDirectory(tempDirectoryName, tempDirectoryName + ".cbr"))
		return false;
	
	if(!deleteDirectory(tempDirectoryName))
		return false;
	
	return true;
}

bool convertOneCbrToCbz(std::string const& fileName)
{
	std::string tempDirectoryName = removeExtension(fileName);
	Unrar unrar;
	
	if(!unrar.extractArchive(fileName, tempDirectoryName))
		return false;
	
	if(!deleteFile(fileName))
		return false;
	
	if(!flattenDirectory(tempDirectoryName))
		return false;
	
	std::vector<std::string> list = listFiles(tempDirectoryName, "");
	ZipArchive za(fileName, ZipArchive::Mode::Write);
	
	for(size_t i=0;i<list.size();++i)
		za.addFileFromDisk(tempDirectoryName + "/" + list[i], tempDirectoryName + ".cbz");
	
	if(!deleteDirectory(tempDirectoryName))
		return false;
	
	return true;
}

bool convertMultipleCbzToCbr(std::vector<std::string> const& files)
{
	for(size_t i=0;i<files.size();++i)
	{
		if(!convertOneCbzToCbr(files[i]))
			return false;
	}
	
	return true;
}

bool convertMultipleCbrToCbz(std::vector<std::string> const& files)
{
	for(size_t i=0;i<files.size();++i)
	{
		if(!convertOneCbrToCbz(files[i]))
			return false;
	}
	
	return true;
}