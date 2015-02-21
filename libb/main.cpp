//
//  main.cpp
//  libb
//
//  Created by maddev on 2/20/15.
//  Copyright (c) 2015 com.integral. All rights reserved.
//

#include <iostream>
#include <set>
#include <vector>
#include <ostream>

using namespace std;

set<string> FolderToInclude;
set<string> ParsedLibs;
set<string> CopiedFiles;
string RootAppPath;
string ExecutableName;

std::vector<std::string> split(const std::string& s, char seperator);
std::string &ltrim(std::string &s);
std::string &trim(std::string &s);

void init(){
    FolderToInclude.insert("/opt/local/lib/");
}

void adjustLibsFolder(const string &mainLib, const set<string> &dependencies, bool isMain){
    string cmd;
    
    if (!isMain && CopiedFiles.find(mainLib) == CopiedFiles.end()){
        cmd = "cp " + mainLib + " " + RootAppPath + "/Contents/Frameworks/";
        system(cmd.c_str());
        CopiedFiles.insert(mainLib);
    }
    
    set<string>::iterator it = dependencies.begin();
    for (; it != dependencies.end(); ++it) {
        if (CopiedFiles.find(*it) == CopiedFiles.end()){
            cmd = "cp " + *it + " " + RootAppPath + "/Contents/Frameworks/";
            system(cmd.c_str());
            CopiedFiles.insert(*it);
        }
        
        vector<string> parts = split(*it, '/');
        cmd = "install_name_tool -change " + *it + " @executable_path/../Frameworks/" + parts[parts.size() - 1];
        cmd += " " + RootAppPath + "/Contents/";
        if (isMain){
            cmd += "/MacOS/" + ExecutableName;
        } else {
            parts = split(mainLib, '/');
            cmd += "/Frameworks/" + parts[parts.size() - 1];
        }
        
        system(cmd.c_str());
    }
}

void extractLibs(const string &lib, bool isMain){
    if(ParsedLibs.find(lib) != ParsedLibs.end()){
        return;
    }
    
    ParsedLibs.insert(lib);
    
    string os;
    os += "otool -L " + lib;
    FILE *p = popen(os.c_str(), "r");
    char line[2048] = {0};
    set<string> dependencies;
    while (fgets(line, 2047, p)){
        set<string>::iterator it = FolderToInclude.begin();
        string libDep = line;
        trim(libDep);
        for (; it != FolderToInclude.end(); ++it) {
            if (libDep.compare(0, it->size(), *it) == 0){
                vector<string> parts = split(libDep, ' ');
                if (!parts.size())
                    continue;
                if (dependencies.find(parts[0]) == dependencies.end() && libDep.compare(0, lib.size(), lib) != 0){
                    dependencies.insert(parts[0]);
                }
            }
        }
    }
    pclose(p);
    
    adjustLibsFolder(lib, dependencies, isMain);
    
    set<string>::iterator it = dependencies.begin();
    for (; it != dependencies.end(); ++it) {
        extractLibs(*it, false);
    }
}

int main(int argc, const char * argv[]) {
    init();

    RootAppPath = "~/Documents/OrthoSensorLumbarLordosisRealignment.app";
    ExecutableName = "\"Spine Alignment Software\"";
    string cmd = "mkdir " + RootAppPath + "/Contents/Frameworks";
    system(cmd.c_str());
    extractLibs(RootAppPath + "/Contents/MacOS/" + ExecutableName, true);
    
    return 0;
}

std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;
    
    std::string::size_type prev_pos = 0, pos = 0;
    
    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        
        output.push_back(substring);
        
        prev_pos = ++pos;
    }
    
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    
    return output;
}

inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

