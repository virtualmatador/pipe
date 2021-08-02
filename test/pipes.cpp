#include "pipe.hpp"

class numbers : public pipe<numbers, void, 0, int>
{
public:
    std::list<int> produce()
    {
        if (index_ < 6)
        {
            return {index_++};
        }
        return {};
    }

private:
    int index_ = 0;
};

class writer : public pipe<writer, int, 3, void>
{
public:
    bool is_open()
    {
        return index_ < 10;
    }
    void consume(std::list<int>&& content)
    {
        ++index_;
        for (auto& item : content)
        {
            results_.push_back(std::move(item));
        }
    }
    std::vector<int> results_;

private:
    std::size_t index_ = 0;
};

class doubler : public pipe<doubler, int, 2, int>
{
public:
    std::list<int> transform(std::list<int>&& content)
    {
        for (auto& item : content)
        {
            item *= 2;
        }
        return content;
    }
};

int main(int argc, const char* argv[])
{
    numbers n;
    doubler d;
    writer w;
    n | d | w;
    if (w.results_.size() != 6)
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
