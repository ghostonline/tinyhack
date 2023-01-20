#pragma once

class Console;

class ProgressBar
{
public:
    void set_level(unsigned new_level) { level = new_level; }
    void set_progress(unsigned progress, unsigned max);
    int get_width() const;
    void draw(Console& console, int y) const;

private:
    unsigned progress = 0;
    unsigned max = 0;
    unsigned level = 0;
};

inline void ProgressBar::set_progress(unsigned new_progress, unsigned new_max)
{
    progress = new_progress;
    max = new_max;
}
