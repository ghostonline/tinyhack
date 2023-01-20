#pragma once

#include <ds/TimeSpan.h>

#include <deque>
#include <string>

struct UpdateArgs;

struct Message
{
    Message(TimeSpan timeout, std::string text) : timeout(timeout), text(text) {}

    TimeSpan timeout;
    std::string text;
};

class MessageLog
{
public:
    void update(const UpdateArgs& args);

    void add_message(std::string message);

    const std::deque<Message>& get_messages() const { return log; }

private:
    TimeSpan now;
    std::deque<Message> log;
};
