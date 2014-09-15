#include <memory>
#include <vector>
#include <iterator>
#include <sstream>
#include <fstream>
#include <cstring>

#include <glm/glm.hpp>
#include <dirent.h>

#include "defines.h"
#include "optionManager.h"

static bool verbose = false;
static char *convertFolder;

static OptionEntry entries[] =
{
    { "verbose", "v", OPTION_ARG_NONE, &verbose, "enable printing of messages"},
    { "show-stats", "c", OPTION_ARG_STRING, &convertFolder, "converts PLY to Matrix file format"},
};

std::vector<std::string> split(const std::string input)
{
    std::istringstream streamInput(input);
    return {std::istream_iterator<std::string>{streamInput}, std::istream_iterator<std::string>{}};
}

bool converToMatrixFile(std::string loadFilename, std::string saveFilename, const uint stride)
{
    bool isAttributes = false;
    std::string line;
    std::ifstream inputFile(loadFilename);
    std::ofstream outputFile(saveFilename);
    std::vector<std::string> values;

    if (inputFile.is_open())
    {
        uint x = 0;
        uint z = 0;
        glm::vec3 pos(0.0f);
        while (std::getline(inputFile, line))
        {
            //if (line.compare(PLY_END_HEADER) == 0)
            if (line.find(PLY_END_HEADER) != std::string::npos)
                isAttributes = true;
            else if (line.compare(PLY_END_ATTRIBUTE) == 0)
                isAttributes = false;
            else if (isAttributes)
            {                
                values = split(line);
                pos[0] = std::stof(values.at(0));
                pos[1] = std::stof(values.at(1));
                pos[2] = std::stof(values.at(2));
                outputFile << x << " " << z << " " << pos[1] << std::endl;

                if (z < (stride-1))
                    z++;
                else
                {
                    x++;
                    z = 0;
                }
            }
        }
        inputFile.close();
        outputFile.close();
        return true;
    }

    return false;
}

int main(int argc, char *argv[])
{
    std::shared_ptr<OptionManager> om;
    om.reset(new OptionManager(argc, argv));
    om->setOptionContext("A small multitools console software to help for simple tasks.");
    om->addUsage("-c <folder>");
    om->addExample("-v -c data/");
    std::vector<OptionEntry> optionEntries(entries, entries + sizeof(entries)/sizeof(entries[0]));
    om->setOptionEntries(optionEntries);

    OptionParserError *error = NULL;
    if (om->parseOptions(&error)) //Read option arguments
    {
        if (convertFolder)
        {
            DIR *folder;
            struct dirent *file;
            if (folder = opendir(convertFolder))
            {
                uint filesCount = 0;
                while (file = readdir(folder))
                {
                    if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0 && strcmp(file->d_name, "hclassic") != 0)
                    {
                        std::string loadPath(convertFolder);

                        loadPath.append(file->d_name);
                        std::string savePath("output/");
                        std::string newFilename(std::string(file->d_name).substr(0,std::string(file->d_name).find_last_of(".")));
                        newFilename.append(".dat");
                        savePath.append(newFilename);
                        if (converToMatrixFile(loadPath, savePath, 104) && verbose)
                        {
                            std::clog << "file " << newFilename << " created." << std::endl;
                            ++filesCount;
                        }
                    }
                }
                closedir(folder);
                if (verbose)
                    std::clog << filesCount << " files have been converted." << std::endl;
            }
        }
    }

    return 0;
}
