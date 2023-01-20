#include "Log.h"
#include <sstream>
#include <string>
#include <vector>

namespace
{
#if SHOW_CONSOLE_OUTPUT

#if WIN32
#include <windows.h>

    void print_to_console(const char* text)
    {
        OutputDebugStringA(text);
        OutputDebugStringA("\n");
    }
#else
    void print_to_console(const char* text)
    {
        std::printf("%s\n", text);
    }
#endif

#else

    void print_to_console(const char*) {}

#endif

    class Placeholder
    {
        const char BEGIN = '{';
        const char END = '}';

    public:
        Placeholder(const std::string& text, std::size_t index)
        {
            const bool hasValidIndex = index < text.size() - 1;
            if (!hasValidIndex) { return; }

            const bool hasUnescapedPlaceholderStart = text[index] == BEGIN && (index == 0 || text[index - 1] != BEGIN);
            if (!hasUnescapedPlaceholderStart) { return; }

            for (auto ii = index + 1; ii < text.size(); ++ii)
            {
                const char currentCharacter = text[ii];
                const bool isInvalidTokenChar = currentCharacter == BEGIN || currentCharacter == '\n';
                if (isInvalidTokenChar) { return; }

                if (currentCharacter == END)
                {
                    valid = true;
                    this->text = std::string(text.begin() + index + 1, text.begin() + ii);
                    break;
                }
            }
        }

        bool is_valid() const { return valid; }
        std::size_t get_size() const { return text.size() + 2; }
        const std::string& get_text() const { return text; }

    private:
        std::string text;
        bool valid = false;
    };

    inline bool try_parse(const std::string& text, int& number)
    {
        try
        {
            number = std::stoi(text);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

}

void Log::print(Level level, const char* format, const std::initializer_list<Arg*>& arguments)
{
    std::vector<Arg*> argIndex = arguments;
    std::stringstream output;
    switch(level)
    {
        default:
        case Level::Debug:
            break;
        case Level::Warning:
            output << "[WARN] ";
            break;
        case Level::Error:
            output << "[ERROR] ";
            break;
    }
    std::string formatStr = format;
    for (std::size_t ii = 0; ii < formatStr.size(); ++ii)
    {
        Placeholder placeholder(formatStr, ii);
        if (placeholder.is_valid())
        {
            ii += placeholder.get_size() - 1;
            int number = -1;
            try_parse(placeholder.get_text(), number);

            if (number >= 0 && static_cast<std::size_t>(number) < argIndex.size())
            {
                argIndex[number]->to_stream(output);
            }
            else
            {
                output << "???";
            }
        }
        else
        {
            output << format[ii];
        }
    }
    print_to_console(output.str().c_str());
    for ( auto arg : arguments) { delete arg; }
}
