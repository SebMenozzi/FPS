#ifndef INIFILE_H_INCLUDED
#define INIFILE_H_INCLUDED

#include "types.h"
#include <string>
#include <map>

class INIFile
{
  public:
    bool8 load(std::string path);
    bool8 save(std::string path);

    void change(std::string name, std::string value);
    void erase(std::string name);
    std::string read(std::string name);
    void clear(void);

  private:
    std::map<std::string, std::string> variables;
};

#endif // INIFILE_H_INCLUDED
