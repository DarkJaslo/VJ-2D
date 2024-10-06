#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

// The translation file has this format:

// word_for_nothing
// word_a translation_a
// word_b translation_b
// word_c translation_c
// word_d translation_d
// ...
void createTranslation(std::fstream& translation_file, std::map<std::string, std::string>& translation)
{
    std::string origin;
    std::string mapped;

    translation[" "] = " ";

    // The first word in the translation file is the identifier of "nothing"
    translation_file >> origin;
    translation[origin] = " ";

    while (translation_file >> origin && translation_file >> mapped)
    {
        if (translation.contains(origin))
        {
            std::cerr << "A translation for " << origin << " was provided more than once!";
            throw std::runtime_error("Error");
        }

        translation[origin] = mapped;
    }

    std::cerr << "Translation (printed to cerr): " << std::endl;
    for (auto const& [k, v] : translation)
    {
        std::cerr << "\t" << k << " -> " << v << std::endl;
    }
    std::cerr << std::endl;
}

// The map file has a format like the following, separating identifiers with spaces:

// word_a word_b word_c word_for_nothing
// word_for_nothing word_for_nothing word_for_nothing
// thing_a word_for_nothing word_a thing_b
void translateFile(std::fstream& map, std::map<std::string, std::string>& translation, std::ostream& output)
{
    std::string line;

    while(getline(map, line))
    {
        std::istringstream split_line(line);
        std::string word;

        while (split_line >> word)
        {
            if (!translation.contains(word))
            {
                std::cerr << "No translation for " << word << std::endl;
                throw std::runtime_error("Error");
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

    std::ostream& output = std::cout;
    translateFile(map_file, translation_map, std::cout);

    std::cout << std::endl;
}