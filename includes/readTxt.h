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
            newFile.open(addPath, std::ios::in);
            if (newFile.is_open())
            {
                std::string line;
                text.clear();

                while (std::getline(newFile, line))
                {
                    text += line;
                }
                newFile.close();
            } else {
                text = "unable to open file";
            }
            return text;
        };
    private:
        std::fstream newFile{};
        std::string text{};

};

