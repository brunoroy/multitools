#ifndef OPTIONMANAGER_H
#define OPTIONMANAGER_H

#include <vector>
#include <string>
#include <iostream>

typedef short Option;
typedef void* OptionPointer;
typedef unsigned int Quark;

const char OPTION_CHAR = '-';

const Option ARGUMENT_OPTION = 0;
const Option ARGUMENT_VALUE = 1;

const Option OPTION_ARG_REMAIN = -2;
const Option OPTION_ARG_INVALID = -1;
const Option OPTION_ARG_NONE = 0;
const Option OPTION_ARG_INT = 1;
const Option OPTION_ARG_FLOAT = 2;
const Option OPTION_ARG_DOUBLE = 3;
const Option OPTION_ARG_STRING = 4;

struct OptionArgument
{
    short type;
    std::string value;
};

struct OptionEntry
{
    OptionEntry(const std::string name, const std::string shortName, Option arg, OptionPointer argData, const std::string description)
        : name(name), shortName(shortName), arg(arg), argData(argData), description(description){}

    const std::string name;
    const std::string shortName;
    Option arg;
    OptionPointer argData;
    const std::string description;
};

struct OptionParserError
{
    Quark domain;
    int code;
    char* message;
};

class OptionManager
{
    public:
        inline OptionManager(int argc, char** argv, uint argMin = 1);
        inline ~OptionManager();

        inline void setOptionContext(std::string optionContext);
        inline void addUsage(std::string usage);
        inline void addExample(std::string example);
        inline void setOptionEntries(const std::vector<OptionEntry> optionEntries);
        inline bool parseOptions(OptionParserError** error);
        inline std::vector<std::string> getOptionRemains();
        inline void printHelp();
        inline void printArguments();

    private:
        inline void initialize(char** argv);
        inline Option getOptionType(std::string option);
        inline Option getArgumentType(const unsigned short index, char** argv);
        inline Option isValidOption(OptionArgument argument);
        inline bool isValueRequired(const short index);
        inline bool isOptionRemain(OptionArgument argument, const short index);
        inline Option findOptionEntry(OptionArgument argument);
        inline bool isOptionExist(std::string option, const short index);

    private:
        std::string _appName;
        std::vector<std::string> _usages;
        std::vector<std::string> _examples;
        std::string _optionContext;
        short _optionArgumentsCount;
        short _optionArgumentsMin;
        std::vector<OptionArgument> _optionArguments;
        short _entriesCount;
        short _usagesCount;
        short _examplesCount;
        std::vector<OptionEntry> _optionEntries;
        std::vector<std::string> _optionRemains;

        inline void printUsages();
};

OptionManager::OptionManager(int argc, char** argv, uint argMin)
{
    _optionArgumentsMin = argMin;
    _optionArgumentsCount = argc;
    initialize(argv);
}

OptionManager::~OptionManager()
{
    _optionArguments.clear();
    _optionEntries.clear();
    _optionRemains.clear();
}

Option OptionManager::getOptionType(std::string option)
{
    for (unsigned int i = 0; i < _optionEntries.size(); ++i)
    {
        if (_optionEntries.at(i).name.compare(option) == 0 || _optionEntries.at(i).shortName.compare(option) == 0)
            return _optionEntries.at(i).arg;
    }

    return OPTION_ARG_REMAIN;
}

Option OptionManager::getArgumentType(const unsigned short index, char** argv)
{
    if (argv[index][0] == OPTION_CHAR)
        return ARGUMENT_OPTION;
    else
        return ARGUMENT_VALUE;
}

void OptionManager::initialize(char** argv)
{
    OptionArgument optionArgument;

    _usages.clear();
    _examples.clear();
    _optionArguments.clear();
    _appName = argv[0];

    for (int i = 1; i < _optionArgumentsCount; ++i)
    {
        optionArgument.type = getArgumentType(i, argv);
        if (argv[i][0] == OPTION_CHAR)
        {
            if (argv[i][1] == OPTION_CHAR)
                optionArgument.value = argv[i]+2;
            else
                optionArgument.value = argv[i]+1;
        }
        else
            optionArgument.value = argv[i];
        _optionArguments.push_back(optionArgument);
    }
    _optionArgumentsCount = _optionArguments.size();
}

void OptionManager::setOptionContext(std::string optionContext)
{
    _optionContext = optionContext;
}

void OptionManager::addUsage(std::string usage)
{
    _usages.push_back(usage);
    _usagesCount = _usages.size();
}

void OptionManager::addExample(std::string example)
{
    _examples.push_back(example);
    _examplesCount = _examples.size();
}

void OptionManager::setOptionEntries(const std::vector<OptionEntry> optionEntries)
{
    _entriesCount = optionEntries.size();

    for (int i = 0; i < _entriesCount; ++i)
        _optionEntries.push_back(optionEntries[i]);

    _optionEntries.push_back(OptionEntry("remains", "*", OPTION_ARG_REMAIN, &_optionRemains, "all option argument remains..."));
}

bool OptionManager::isOptionExist(std::string option, const short index)
{
    if (_optionEntries.at(index).name.compare(option) == 0 || _optionEntries.at(index).shortName.compare(option) == 0)
        return true;
    else
        return false;
}

bool OptionManager::isValueRequired(const short index)
{
    Option optionEntry = findOptionEntry(_optionArguments.at(index-1));
    if (optionEntry == OPTION_ARG_INVALID)
        return false;
    else if (_optionEntries.at(optionEntry).arg > 0)
        return true;
    else
        return false;
}

bool OptionManager::isOptionRemain(OptionArgument argument, const short index)
{
    bool optionRemained = false;

    if (argument.type == ARGUMENT_VALUE)
    {
        if (index == 0)
            optionRemained = true;
        else
        {
            if (!isValueRequired(index))
                optionRemained = true;
            else
                optionRemained = false;
        }
    }

    return optionRemained;
}

Option OptionManager::isValidOption(OptionArgument argument)
{
    Option optionIndex = findOptionEntry(argument);
    if (optionIndex != OPTION_ARG_INVALID && argument.type == ARGUMENT_OPTION)
    {
        return optionIndex;
    }
    else if (optionIndex == OPTION_ARG_INVALID && argument.type == ARGUMENT_OPTION)
        return OPTION_ARG_INVALID;
    else if (argument.type == ARGUMENT_VALUE)
        return _optionEntries.size()-1;

    return OPTION_ARG_INVALID;
}

Option OptionManager::findOptionEntry(OptionArgument argument)
{
    Option optionEntry = OPTION_ARG_INVALID;

    if (argument.type == ARGUMENT_OPTION)
    {
        for (int i = 0; i < _entriesCount; ++i)
        {
            if (isOptionExist(argument.value, i))
                return i;
        }
    }

    return optionEntry;
}

bool OptionManager::parseOptions(OptionParserError** error)
{
    std::vector<std::string> remainArgs;
    remainArgs.clear();

    if (_optionArgumentsCount >= _optionArgumentsMin)
    {
        for (int i = 0; i < _optionArgumentsCount; ++i)
        {
            OptionArgument optionArgument = _optionArguments.at(i);
            if (optionArgument.value.compare("help") == 0 || optionArgument.value.compare("h") == 0)
            {
                printHelp();
                return false;
            }
            else
            {
                if (optionArgument.type == ARGUMENT_OPTION || isOptionRemain(optionArgument, i))
                {
                    Option optionEntryIndex = isValidOption(optionArgument);
                    if (optionEntryIndex != OPTION_ARG_INVALID)
                    {
                        switch (_optionEntries.at(optionEntryIndex).arg)
                        {
                            case OPTION_ARG_NONE:
                            {
                                *(bool*)_optionEntries[optionEntryIndex].argData = true;
                            }
                            break;
                            case OPTION_ARG_INT:
                            {
                                if (i < (_optionArgumentsCount-1))
                                    *(int*)_optionEntries[optionEntryIndex].argData = std::stoi(_optionArguments[i+1].value);
                                else
                                {
                                    std::cout << "Missing argument for -" << _optionArguments[i].value << std::endl;
                                    return false;
                                }
                            }
                            break;
                            case OPTION_ARG_FLOAT:
                            {
                                if (i < (_optionArgumentsCount-1))
                                    *(float*)_optionEntries[optionEntryIndex].argData = std::stof(_optionArguments[i+1].value);
                                else
                                {
                                    std::cout << "Missing argument for -" << _optionArguments[i].value << std::endl;
                                    return false;
                                }
                            }
                            break;
                            case OPTION_ARG_DOUBLE:
                            {
                                if (i < (_optionArgumentsCount-1))
                                    *(double*)_optionEntries[optionEntryIndex].argData = std::stod(_optionArguments[i+1].value);
                                else
                                {
                                    std::cout << "Missing argument for -" << _optionArguments[i].value << std::endl;
                                    return false;
                                }
                            }
                            break;
                            case OPTION_ARG_STRING:
                            {
                                if (i < (_optionArgumentsCount-1))
                                    *(char**)_optionEntries[optionEntryIndex].argData = (char*)_optionArguments[i+1].value.c_str();
                                else
                                {
                                    std::cout << "Missing argument for -" << _optionArguments[i].value << std::endl;
                                    return false;
                                }
                            }
                            break;
                            case OPTION_ARG_REMAIN:
                            {
                                _optionRemains.push_back(_optionArguments[i].value);
                            }
                            break;
                        }
                    }
                    else
                    {
                        std::cout << "Invalid option(s)..." << std::endl;
                        return false;
                    }
                }
            }
        }
    }
    else
    {
        printHelp();
        return false;
    }

    /*if (error == NULL)
        std::cout << "error: " << error->message << std::endl;*/

    return true;
}

std::vector<std::string> OptionManager::getOptionRemains()
{
    return _optionRemains;
}

void OptionManager::printHelp()
{
    printf("\n");
    printf("Description:\n");
    printf("  %s\n\n", _optionContext.c_str());

    if (_usagesCount > 0)
    {
        if (_usagesCount == 1)
            printf("Usage:\n");
        else
            printf("Usages:\n");

        for (int i = 0; i < _usagesCount; ++i)
        {
            printf("  %s [OPTION...] %s\n", _appName.substr(2).c_str(), _usages.at(i).c_str());
        }
        printf("\n");
    }

    if (_examplesCount > 0)
    {
        if (_examplesCount == 1)
            printf("Example:\n");
        else
            printf("Examples:\n");

        for (int i = 0; i < _examplesCount; ++i)
        {
            printf("  %s %s\n", _appName.substr(2).c_str(), _examples.at(i).c_str());
        }
        printf("\n");

    }

    printf("Application Options:\n");

    for (int i = 0; i < _entriesCount; ++i)
    {
        printf("-%s, --%-20s %s\n", _optionEntries[i].shortName.c_str(), _optionEntries[i].name.c_str(), _optionEntries[i].description.c_str());
    }
    printf("\n");
}

void OptionManager::printArguments()
{
    for (int i = 0; i < _optionArgumentsCount; ++i)
    {
        std::cout << "type: " << _optionArguments.at(i).type << std::endl;
        std::cout << "value: " << _optionArguments.at(i).value << std::endl << std::endl;
    }
}

#endif // OPTIONMANAGER_H
