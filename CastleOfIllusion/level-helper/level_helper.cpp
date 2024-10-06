#include <iostream>
#include <string>
#include <fstream>
#include <map>

void createTranslation(std::fstream& translation_file, std::map<std::string, std::string>& translation)
{
    std::string origin;
    std::string mapped;

    translation[" "] = " ";

    while (translation_file >> origin && translation_file >> mapped)
    {
        if (translation.contains(origin))
        {
            std::cerr << "A translation for " << origin << " was provided more than once!";
            throw std::runtime_error();
        }

        translation[origin] = mapped;
    }
}

void translateFile(std::fstream& map, std::map<std::string, std::string> const& translation, std::ostream& output)
{
    std::string line;

    while(map.getline(line))
    {
        std::stringstream split_line(line);
        std::string word;
        while (split_line >> word)
        {
            if (!translation.contains(word))
            {
                std::cerr << "No translation for " << word << std::endl;
                throw std::runtime_error();
            }
            output << translation[word];
        }
        output << "\n";
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage:\nlevel_helper [readable_map_path] [translation_file_path]" << std::endl;
        return 1;
    }

    std::string map_path = argv[1];
    std::string translation_path = argv[2];

    std::fstream map_file(map_path);
    if (!map_file.is_open())
    {
        std::cerr << "Could not open file in " << map_path << std::endl;
        return 1;
    }

    std::fstream translation_file(translation_path);
    if(!translation_file.is_open())
    {
        std::cerr << "Could not open file in " << translation_path << std::endl;
        return 1;
    }

    std::map<std::string, std::string> translation_map;
    createTranslation(translation_file, translation_map);

    std::ostream output;
    translateFile(translation_file, translation_map, output);

    std::cout << output << std::endl;
}