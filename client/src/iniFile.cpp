#include "iniFile.h"
#include <fstream>
#include <sstream>

bool8 INIFile::load(std::string path)
{
  // Delete possible variables in memory
  this->clear();

  // Open the file in reading mode
  std::ifstream file(path.c_str(), std::ios::in);

  if(!file) {
    return FALSE;
  }

  // For each line
  std::string line;
  while(getline(file, line)) {
    // Creation of an extraction stream from this line
    std::istringstream stream(line);

    // Retrieve the name of the variable until the '='
    std::string name;
    std::getline(stream, name, '=');

    // Retrieve everything else after the '=', the value
    std::string value;
    std::getline(stream, value);

    // Add the variable in memory
    this->variables[name] = value;
  }

  return TRUE;
}

bool8 INIFile::save(std::string path)
{
  // Open / Create the file in writing mode
  std::ofstream file(path.c_str(), std::ios::out | std::ios::trunc);

  if(!file) {
    return FALSE;
  }

  // Add variables in memory in the file
  for (std::map<std::string,std::string>::iterator it = this->variables.begin(); it != this->variables.end(); it++) {
    file << it->first << "=" << it->second << '\n';
  }

  file.close();

  return TRUE;
}

void INIFile::change(std::string name, std::string value)
{
  // Change the variable in memory
  this->variables[name] = value;
}

void INIFile::erase(std::string name)
{
  // Erase the variable in memory
  this->variables.erase(name);
}

std::string INIFile::read(std::string name)
{
  // Return the variable in memory
  return this->variables[name];
}

void INIFile::clear(void)
{
  // Delete every variables in memory
  this->variables.clear();
}
