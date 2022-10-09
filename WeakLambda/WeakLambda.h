#pragma once

#include <functional>

template <typename T, typename F>
class WeakBind
{
public:
    WeakBind() = delete;
    WeakBind(const std::shared_ptr<T>& ptr, const F& func) : func_(func), weak_obj_(ptr)
    {
    }
    

    template<typename... ArgTypes>
    auto operator()(ArgTypes... args) -> typename std::result_of<F(ArgTypes...)>::type
    {
        std::shared_ptr<T> obj = weak_obj_.lock();
        if (obj)
        {
            return func_(args...);
        }
        else
        {
            return decltype(func_(args...))();
        }
    }
private:
    F func_;
    std::weak_ptr<T> weak_obj_;
};


class EnableWeakLambdaCapture
{
public:
    class Holder : public std::enable_shared_from_this<Holder>
    {};

    template<typename Functor>
    WeakBind<Holder, Functor> CreateWeakCallback(const Functor& lambda)
    {
        if (!holder_)
        {
            holder_ = std::make_shared<Holder>();
        }

        return WeakBind<Holder, Functor>(holder_, lambda);
    }

private:
    std::shared_ptr<Holder> holder_;
};

template<typename T, typename Functor>
auto weak_lambda(T* pointer, Functor lambda) -> WeakBind<EnableWeakLambdaCapture::Holder, Functor>
{
    return pointer->CreateWeakCallback(lambda);
}