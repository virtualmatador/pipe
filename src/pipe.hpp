#include <functional>
#include <list>

template<class T, class I, std::size_t IN, class O>
class pipe
{
public:
    template<class P>
    P& operator|(P& next)
    {
        if constexpr(std::is_same<typename P::output_type, void>::value)
        {
            bool is_open = next.is_open();
            std::list<O> output_buffer;
            while (is_open)
            {
                std::list<O> content = read();
                if (content.size() > 0)
                {
                    output_buffer.splice(output_buffer.end(), content);
                    while (output_buffer.size() >= P::input_size)
                    {
                        std::list<O> sized_output;
                        for (std::size_t i = 0; i < P::input_size; ++i)
                        {
                            sized_output.splice(sized_output.end(),
                                output_buffer, output_buffer.begin());
                        }
                        next.consume(std::move(sized_output));
                        is_open = next.is_open();
                        if (!is_open)
                        {
                            break;
                        }
                    }
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
    std::list<O> read()
    {
        while (input_buffer_.size() < IN)
        {
            std::list<I> content = input_();
            if (content.size() > 0)
            {
                input_buffer_.splice(input_buffer_.end(), std::move(content));
            }
            else
            {
                return {};
            }
        }
        std::list<O> output;
        while (input_buffer_.size() >= IN)
        {
            std::list<I> sized_input;
            for (std::size_t i = 0; i < IN; ++i)
            {
                sized_input.splice(sized_input.end(),
                    input_buffer_, input_buffer_.begin());
            }
            output.splice(output.end(), static_cast<T*>(this)->transform(
                std::move(sized_input)));
        }
        return output;
    }

private:
    using output_type = O;
    static const std::size_t input_size = IN;
    std::function<std::list<I>()> input_;
    std::list<I> input_buffer_;
    template<class TT, class II, std::size_t IIN, class OO> friend class pipe;
};

template<class T, class I, std::size_t IN>
class pipe<T, I, IN, void>
{
private:
    using output_type = void;
    static const std::size_t input_size = IN;
    std::function<std::list<I>()> input_;
    template<class TT, class II, std::size_t IIN, class OO> friend class pipe;
};

template<class T, class O>
class pipe<T, void, 0, O>
{
public:
    template<class P>
    P& operator|(P& next)
    {
        if constexpr(std::is_same<typename P::output_type, void>::value)
        {
            bool is_open = next.is_open();
            std::list<O> output_buffer;
            while (is_open)
            {
                std::list<O> content = read();
                if (content.size() > 0)
                {
                    output_buffer.splice(output_buffer.end(), content);
                    while (output_buffer.size() >= P::input_size)
                    {
                        std::list<O> sized_output;
                        for (std::size_t i = 0; i < P::input_size; ++i)
                        {
                            sized_output.splice(sized_output.end(),
                                output_buffer, output_buffer.begin());
                        }
                        next.consume(std::move(sized_output));
                        is_open = next.is_open();
                        if (!is_open)
                        {
                            break;
                        }
                    }
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
    std::list<O> read()
    {
        return static_cast<T*>(this)->produce();
    }
};

template<class T, std::size_t IN>
class pipe<T, void, IN, void>;
