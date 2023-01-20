#pragma once

#include "LogArg.h"

#include <ostream>

#define SHOW_CONSOLE_OUTPUT DEBUG_BUILD

class Log
{
    enum class Level
    {
        Debug,
        Warning,
        Error,
    };

public:
#if SHOW_CONSOLE_OUTPUT
	template<typename ...Args>
	static void print(const char* format, const Args& ... args)
	{
        print(Level::Debug, format, { new ArgTemplate<Args>(&args) ... });
	}
    template<typename ...Args>
    static void warn(const char* format, const Args& ... args)
    {
        print(Level::Warning, format, { new ArgTemplate<Args>(&args) ... });
    }
    template<typename ...Args>
    static void error(const char* format, const Args& ... args)
    {
        print(Level::Error, format, { new ArgTemplate<Args>(&args) ... });
    }
#else
	template<typename ...Args>
	static void print(const char*, const Args& ...) {}
    template<typename ...Args>
    static void warn(const char*, const Args& ...) {}
    template<typename ...Args>
    static void error(const char*, const Args& ...) {}
#endif

private:
    class Arg
    {
    public:
        virtual ~Arg() {}
        virtual void to_stream(std::ostream& stream) = 0;
    };

    template<typename Type>
    class ArgTemplate : public Arg
    {
    public:
        ArgTemplate(const Type* arg) : arg(arg) {}
        virtual void to_stream(std::ostream& stream) override
        {
            if (arg)
            {
                stream << *arg;
            }
            else
            {
                stream << "nullptr";
            }
        }

    private:
        const Type* arg;
    };

    static void print(Level level, const char* format, const std::initializer_list<Arg*>& arguments);
};
