#include <functional>
#include <optional>

template<class T, class I, class O>
class pipe
{
public:
    template<class P>
    P& operator|(P& next)
    {
        if constexpr(std::is_same<typename P::output_type, void>::value)
        {
            while (next.is_open())
            {
                auto content = input_();
                if (content.has_value())
                {
                    next.consume(
                        static_cast<T*>(this)->transform(std::move(*content)));
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            next.input_ = std::bind(&pipe::read, this);
        }
        return next;
    }

private:
    std::optional<O> read()
    {
        auto content = input_();
        if (content.has_value())
        {
            return static_cast<T*>(this)->transform(std::move(*content));
        }
        else
        {
            return {};
        }
    }

private:
    using output_type = O;
    std::function<std::optional<I>()> input_;
    template<class TT, class II, class OO> friend class pipe;
};

template<class T, class I>
class pipe<T, I, void>
{
private:
    using output_type = void;
    std::function<std::optional<I>()> input_;
    template<class TT, class II, class OO> friend class pipe;
};

template<class T, class O>
class pipe<T, void, O>
{
public:
    template<class P>
    P& operator|(P& next)
    {
        if constexpr(std::is_same<typename P::output_type, void>::value)
        {
            while (next.is_open())
            {
                std::optional<O> content = static_cast<T*>(this)->produce();
                if (content.has_value())
                {
                    next.consume(std::move(*content));
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            next.input_ = std::bind(&pipe::read, this);
        }
        return next;
    }

private:
    using output_type = O;
    std::optional<O> read()
    {
        return static_cast<T*>(this)->produce();
    }
};

template<class T>
class pipe<T, void, void>;
