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
void checkForBackups()
{
	//if there is a zip named 'DDLoader_Backup.zip' then rename it to 'DDLoader_Backup2.zip'
	if (std::filesystem::exists("DDLoader_Backup.zip"))
	{
		std::filesystem::rename("DDLoader_Backup.zip", "DDLoader_Backup2.zip");
	}

}

void deleteExtraBackups()
{
	
	//if there is a zip named 'DDLoader_Backup2.zip' then delete it
	if (std::filesystem::exists("DDLoader_Backup2.zip"))
	{
		std::filesystem::remove("DDLoader_Backup2.zip");
	}
}

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

bool downloadZip()
{
	try
	{
		// send request
		sf::Http http;
		http.setHost("192.168.1.48", 8000);
		sf::Http::Request request("/loader/DDLoader.zip", sf::Http::Request::Get);
		sf::Http::Response response = http.sendRequest(request);

		// check status code
		if (response.getStatus() == sf::Http::Response::Ok) {
			// save file
			std::ofstream file;
			file.open("DDLoader.zip", std::ios::binary);
			file.write(response.getBody().c_str(), response.getBody().size());
			file.close();
			return true;
		}
		else {
			std::cout << "Error: could not download file. Writing troubleshooting to DDLoader_errorlog.txt\nDO NOT CLOSE PROGRAM PLEASE. IT WILL CLOSE ITSELF AFTER WRITING LOG." << std::endl;
			return false;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return false;
	}
}





void unzip(std::string fileName)
{
	std::string command = "powershell Expand-Archive -Path " + fileName + " -DestinationPath " + ".";
	system(command.c_str());
	std::filesystem::remove(fileName);
}


void deleteFile(std::string fileName)
{
	std::filesystem::remove(fileName);
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

void moveFile(std::string fileName, std::string directory)
{
	if (!std::filesystem::exists(fileName)) //check if the file exists
	{
		std::cout << "The file does not exist." << std::endl;
		return;
	}
	if (!std::filesystem::is_directory(directory)) //check if the directory exists
	{
		std::cout << "The directory does not exist." << std::endl;
		return;
	}
	if (std::filesystem::exists(directory + "\\" + fileName)) //check if the file already exists in the directory
	{
		std::cout << "The file already exists in the directory." << std::endl;
		return;
	}

	//use rename method to move the file
	std::filesystem::rename(fileName, directory + "\\" + fileName);
	std::cout << "The file has been moved to the directory." << std::endl;
}

void Log(std::string message)
{
	std::ofstream file;
	file.open("DDLoader_errorlog.txt", std::ios::app);
	file << message << std::endl;
	file.close();
}

bool hasInternet()
{
	bool success = false;
	//check if there is internet by pinging google
	try
	{
		sf::Http http;
		http.setHost("www.google.com");
		sf::Http::Request request;
		request.setMethod(sf::Http::Request::Get);
		request.setUri("/");
		sf::Http::Response response = http.sendRequest(request);
		success = true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		success = false;
	}
}

bool homeServerActive()
{

	bool success = false;
	//check if server is actually online by pinging http://192.168.1.48:8000 //47.41.98.12
	try
	{
		sf::Http http;
		http.setHost("192.168.1.48", 8000); //fix: removed "http://", passed in port as a separate parameter

		//send a simple, basic get request to make sure it's online.
		sf::Http::Request request;
		request.setMethod(sf::Http::Request::Get);
		request.setUri("/");
		sf::Http::Response response = http.sendRequest(request);
		if (response.getStatus() == sf::Http::Response::Status::Ok)
		{
			success = true;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		success = false;
	}

	//fix: return the value back
	return success;
}

void restoreBackup()
{
	
	//check if backup exists
	if (!std::filesystem::exists("DDLoader_Backup.zip"))
	{
		std::cout << "No backup exists. If this is first load, this is normal (:" << std::endl;
		return;
	}
	//unzip backup
	unzip("DDLoader_Backup.zip");
	//move DDLMM from resources folder to directory this is ran from
	moveFile("resources\\DDLMM.exe", ".");

	//delete backup
	deleteFile("DDLoader_Backup.zip");
	//run DDLMM
	runFile("DDLMM.exe");
}

int main()
{
	//check for backups
	checkForBackups();

	//move DDLMM to resources folder
	moveFile("DDLMM.exe", "resources");

	//zip up 'resources' folder
	zipFolder("resources");

	//rename zip to backup
	renameZip("resources.zip", "DDLoader_Backup.zip");

	//delete extra backups
	deleteExtraBackups();

	//download zip
	if (downloadZip())
	{
		
	}
	else
	{
		restoreBackup();
		Log("Failed to download zip. Restoring Backup...\nAttempting troubleshooting...\n\n\n=====================================\n");
		if (hasInternet())
		{
			
			Log("hasInternet() returned true.\n");
			if (homeServerActive())
			{
				Log("homeServerActive() returned true. If we've gotten to this point, something weird is going on. Please contact Daltonyx on Discord or join at https://discord.gg/daMWV3TTea and leave a message!\n");
				
			}
			else
			{
				Log("homeServerActive() returned false. (my pi probably crashed) Please contact Daltonyx on Discord or join at https://discord.gg/daMWV3TTea and leave a message!\n");
			}

		}
		else
		{
			Log("hasInternet() returned false.\n");
		}

		return 1;
	}

	//unzip zip
	unzip("DDLoader.zip");

	//delete zip
	deleteFile("DDLoader.zip");

	//run DDLoader
	runFile("DDLMM.exe");

	//return okay if the program exits properly
    return 0;
}