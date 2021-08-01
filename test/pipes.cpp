#include <iostream>

#include "pipe.hpp"

class numbers : public pipe<numbers, void, int>
{
public:
    std::optional<int> produce()
    {
        if (index_ < 4)
        {
            return index_++;
        }
        return {};
    }

private:
    std::size_t index_ = 0;
};

class writer : public pipe<writer, int, void>
{
public:
    bool is_open()
    {
        return index_ < 10;
    }
    void consume(int&& content)
    {
        ++index_;
        results_.push_back(content);
    }
    std::vector<int> results_;

private:
    std::size_t index_ = 0;
};

class doubler : public pipe<doubler, int, int>
{
public:
    int transform(int&& content)
    {
        return content * 2;
    }
};

int main(int argc, const char* argv[])
{
    numbers n;
    doubler d;
    writer w;
    n | d | w;
    if (w.results_.size() != 4)
    {
        return -1;
    }
    for (int i = 0; i < w.results_.size(); ++i)
    {
        if (w.results_[i] != i * 2)
        {
            return -1;
        }
    }
    return 0;
}
