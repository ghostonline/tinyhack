#pragma once

struct RunContext;

namespace app
{
    bool is_finished(RunContext* context);
    RunContext* create_context();
    void run_step(RunContext* context);
    void destroy_context(RunContext* context);
};
