#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <Windows.h>

//in the standard namespace
using namespace std;

//variables
const int width{ 800 };
const int height{ 400 };


//methods

//method to zip a folder
bool zipFolder(std::string folderName)
{
	bool success = false;
	try
	{
		std::string command = "powershell Compress-Archive -Path " + folderName + " -DestinationPath " + folderName + ".zip";
		system(command.c_str());
		std::filesystem::remove_all(folderName);
		success = true;
	}
	catch (const std::filesystem::filesystem_error& ex)
	{
		if (ex.code() == std::errc::no_such_file_or_directory)
		{
			throw std::invalid_argument("The folder " + folderName + " does not exist, please make sure you haven't separated the files for DDLoader.");
		}
		else
		{
			throw ex;
		}
	}
	return success;
}

bool downloadZip(std::string url, std::string fileName)
{
	bool success = false;
	try
	{
		sf::Http http;
		http.setHost(url);
		sf::Http::Request request;
		request.setMethod(sf::Http::Request::Get);
		request.setUri("/" + fileName);
		sf::Http::Response response = http.sendRequest(request);
		std::ofstream file;
		file.open(fileName, std::ios::binary);
		file.write(response.getBody().c_str(), response.getBody().size());
		file.close();
		success = true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		success = false;
	}
	return success;
}

void unzip(std::string fileName)
{
	std::string command = "powershell Expand-Archive -Path " + fileName + " -DestinationPath " + fileName.substr(0, fileName.size() - 4);
	system(command.c_str());
	std::filesystem::remove(fileName);
}

void downloadFile(std::string url, std::string fileName)
{
	sf::Http http;
	http.setHost(url);
	sf::Http::Request request;
	request.setMethod(sf::Http::Request::Get);
	request.setUri("/" + fileName);
	sf::Http::Response response = http.sendRequest(request);
	std::ofstream file;
	file.open(fileName, std::ios::binary);
	file.write(response.getBody().c_str(), response.getBody().size());
	file.close();
}

void deleteFile(std::string fileName)
{
	std::filesystem::remove(fileName);
}

void deleteFolder(std::string folderName)
{
	std::filesystem::remove_all(folderName);
}

void moveFile(std::string fileName, std::string destination)
{
	std::filesystem::rename(fileName, destination);
}

void moveFolder(std::string folderName, std::string destination)
{
	std::filesystem::rename(folderName, destination);
}

void copyFile(std::string fileName, std::string destination)
{
	std::filesystem::copy(fileName, destination);
}

void copyFolder(std::string folderName, std::string destination)
{
	std::filesystem::copy(folderName, destination);
}

void createFolder(std::string folderName)
{
	std::filesystem::create_directory(folderName);
}

void createFile(std::string fileName)
{
	std::ofstream file;
	file.open(fileName);
	file.close();
}

void createShortcut(std::string fileName, std::string destination)
{
	std::string command = "powershell New-Item -Path " + destination + " -Name " + fileName + " -ItemType SymbolicLink -Value " + fileName;
	system(command.c_str());
}

void createShortcut(std::string fileName, std::string destination, std::string icon)
{
	std::string command = "powershell New-Item -Path " + destination + " -Name " + fileName + " -ItemType SymbolicLink -Value " + fileName + " -IconLocation " + icon;
	system(command.c_str());
}

void createShortcut(std::string fileName, std::string destination, std::string icon, std::string arguments)
{
	std::string command = "powershell New-Item -Path " + destination + " -Name " + fileName + " -ItemType SymbolicLink -Value " + fileName + " -IconLocation " + icon + " -ArgumentList " + arguments;
	system(command.c_str());
}

void renameFile(std::string fileName, std::string desiredName)
{
	std::filesystem::rename(fileName, desiredName);
}

void renameFolder(std::string folderName, std::string desiredName)
{
	std::filesystem::rename(folderName, desiredName);
}

void renameZip(std::string zipName, std::string zipDesiredName)
{
	
	std::filesystem::rename(zipName, zipDesiredName);
}

void runFile(std::string fileName)
{
	std::string command = "start " + fileName;
	system(command.c_str());
}

void moveEXEIntoZip()
{
	std::string command = "powershell Compress-Archive -Path DDLMM.exe -DestinationPath resources.zip";
	system(command.c_str());
	std::filesystem::remove("DDLoader.exe");
}

int main()
{
	//set up window
	sf::RenderWindow window(sf::VideoMode(width, height), "DDLoader Updater");
	sf::Event event;
	sf::Image icon;
	icon.loadFromFile("Icon.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	
	//zip up 'resources' folder
	zipFolder("resources");

	moveEXEIntoZip();

	renameZip("resources.zip", "DDLoader_Backup.zip");

	//download zip
	downloadZip("47.41.98.12:8000/loader/DDLoader.zip", "DDLoader.zip");

	//unzip zip
	unzip("DDLoader.zip");

	//delete zip
	deleteFile("DDLoader.zip");

	//run DDLoader
	runFile("DDLMM.exe");


	
	
	//return okay if the program exits properly
    return 0;
}