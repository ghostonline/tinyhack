#include "MessageLog.h"

#include "UpdateArgs.h"

static const TimeSpan ShowTime = TimeSpan::from_seconds(5.0);

void MessageLog::update(const UpdateArgs& args)
{
    now = args.time_since_app_start;
    while (!log.empty() && log.back().timeout < now)
    {
        log.pop_back();
    }
}

void MessageLog::add_message(std::string message)
{
    log.emplace_front(now + ShowTime, message);
}
