#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <process.h>
#include <thread>
#include <regex>
#include <shlobj.h>
#include <filesystem>
#include <objbase.h>

using namespace std;

ofstream write;

bool hasEnding(std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length())
    {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else
    {
        return false;
    }
}

void processFile(string fileName)
{
    string line;
    ifstream myfile(fileName);
    stringstream os;
    if (myfile.is_open())
    {
        string line;
        getline(myfile, line);
        string table("table");
        string tablee("tableextension");
        if (line.substr(0, table.length()) != table && line.substr(0, tablee.length()) != tablee)
        {
            return;
        }
        os << line << endl;
        while (getline(myfile, line))
        {
            os << line << endl;
        }
    }
    else
    {
        printf("%s is not open.", fileName.c_str());
    }
    string temp = os.str();

    // Copied from:
    // https://www.reddit.com/r/learnprogramming/comments/hr4mip/c_extract_substring_using_regex/
    regex rgx("field(.*; \".*\"; .*)");
    std::sregex_iterator iter(temp.begin(), temp.end(), rgx);
    std::sregex_iterator end;
    while (iter != end)
    {
        string s((*iter)[0]);
        regex fieldID_rgx("field\\([0-9]*");
        std::sregex_iterator fieldID_iter(s.begin(), s.end(), fieldID_rgx);
        string fieldID((*fieldID_iter)[0]);
        fieldID = fieldID.substr(6);
        regex objectID_rgx("extends.*");
        std::sregex_iterator objectID_iter(temp.begin(), temp.end(), objectID_rgx);
        string objectID;
        if (objectID_iter != end)
        {
            objectID = (*objectID_iter)[0];
            objectID = objectID.substr(8);
        }
        else
        {
            objectID_rgx = regex("table .*");
            objectID_iter = std::sregex_iterator(temp.begin(), temp.end(), objectID_rgx);
            objectID = (*objectID_iter)[0];
            objectID = objectID.substr(12);
        }
        regex fieldName_rgx("\".*\";");
        std::sregex_iterator fieldName_iter(s.begin(), s.end(), fieldName_rgx);
        string fieldName((*fieldName_iter)[0]);
        fieldName = fieldName.substr(0, fieldName.size() - 1);
        write << "AddField(Database::" << objectID << ", missing, Database::" << objectID
              << ", " << fieldID << "); // " << objectID << " - " << fieldName << endl;
        ++iter;
    }
}

void processFolder(string folder)
{
    // Copied from:
    // https://stackoverflow.com/questions/67273/how-do-you-iterate-through-every-file-directory-recursively-in-standard-c
    string process_path;
    string search_path;
    process_path = folder + "\\";
    search_path = folder + "\\*";
    WIN32_FIND_DATAA fd;
    HANDLE hFind = ::FindFirstFileA(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                string temp;
                if (hasEnding(folder, ".al"))
                {
                    temp = process_path;
                }
                else
                {
                    std::string ws(fd.cFileName);
                    std::string s(ws.begin(), ws.end());
                    temp = process_path + s;
                }
                processFile(temp.c_str());
            }
        } while (::FindNextFileA(hFind, &fd));
        ::FindClose(hFind);
    }
}

void CopyAfter() {
    string line;
    ifstream myfile("CopyAfter.txt");
    if (myfile.is_open())
    {
        while (myfile.good())
        {
            getline(myfile, line);
            write << line << endl;
        }
        myfile.close();
    }
    else
    {
        printf("%s is not open.", "CopyAfter.txt");
    }
}

int main(int argc, char const *argv[])
{
    string s(argv[1]);
    write.open(s + "\\DataMigrationMgtCodeunit.al");
    write << "codeunit 57044 \"SIT DataMigrationMgt\"\n{\nDescription = '';\nprocedure CreateUpgradeData()\nbegin" << endl;
    cout << "Creating..." << endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    processFolder(s);
    auto t2 = std::chrono::high_resolution_clock::now();
    write << "end;" << endl;
    CopyAfter();
    write << "\n}";
    write.close();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    if (duration == 0)
    {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "\nExecution-Time: " << duration << " milliseconds.\n"
                  << endl;
    }
    else
    {
        std::cout << "\nExecution-Time: " << duration << " seconds.\n"
                  << endl;
    }
    chrono::milliseconds dura(500);
    this_thread::sleep_for(dura);
    return 0;
}