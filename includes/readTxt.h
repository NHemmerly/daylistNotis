#pragma once
#include <iostream>
#include <fstream>
#include <string>

class Reader
{
    public:
        Reader() = default;
        const std::string& getText(const std::string& fileName)
        {
            std::string addPath = "../.secrets/" + fileName;
            if (!(fileExists(addPath)))
            {
                std::ofstream {addPath};
            }
            newFile.open(addPath, std::ios::in);
            if (newFile.is_open())
            {
                std::string line;
                int lineCount {0};
                text.clear();

                while (std::getline(newFile, line))
                {
                    text += line + "\n";
                    ++lineCount;
                }
                if (lineCount < 2)
                {
                    text.pop_back();
                }
                newFile.close();
            } else {
                text = "unable to open file";
            }
            return text;
        }

        const bool fileExists (const std::string& fileName)
        {
            std::string addPath = "../.secrets/" + fileName;
            if (FILE *file = fopen(addPath.c_str(), "r"))
            {
                fclose(file);
                return true;
            } else {
                return false;
            }
        }

        void writeText(const std::string& fileName, const std::string& text)
        {
            std::string addPath = "../.secrets/" + fileName;
            if (fileExists(addPath))
            {
                newFile.open(addPath, std::ios::out);
                if (newFile.is_open()){
                    
                    newFile << text;
                    newFile.close();
                } else {
                    std::cout << "can't open file. Text:" << "\n" << text;
                }
            } else {
                std::cout << "File does not exist" << std::endl;
            }
        }
    private:
        std::fstream newFile{};
        std::string text{};

};

