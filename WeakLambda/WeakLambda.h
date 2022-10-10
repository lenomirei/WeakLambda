#pragma once

#include <functional>

template <typename T, typename F>
class WeakLambda
{
public:
    WeakLambda() = delete;

    WeakLambda(T* raw_obj, const F& func) : func_(func), weak_obj_(raw_obj->shared_from_this())
    {
    }

    WeakLambda(const std::shared_ptr<T>& ptr, const F& func) : func_(func), weak_obj_(ptr)
    {
    }

    WeakLambda(const WeakLambda& other) : func_(other.func_), weak_obj_(other.weak_obj_)
    {}

    WeakLambda(const WeakLambda&& other) : func_(std::move(other.func_)), weak_obj_(std::move(other.weak_obj_))
    {
    }

    WeakLambda(WeakLambda&& other) : func_(std::move(other.func_)), weak_obj_(std::move(other.weak_obj_))
    {
    }

    WeakLambda& operator=(const WeakLambda& other)
    {
        if (this != &other)
        {
            this->func = other.func_;
            this->weak_obj_ = other.weak_obj_;
        }
        return *this;

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
    WeakLambda<Holder, Functor> CreateWeakLambda(const Functor& lambda)
    {
        if (!holder_)
        {
            holder_ = std::make_shared<Holder>();
        }

        return WeakLambda<Holder, Functor>(holder_, lambda);
    }

private:
    std::shared_ptr<Holder> holder_;
};

template<typename T, bool>
struct WeakLambdaHelper
{   
    template<typename Functor>
    static WeakLambda<T, Functor> CreateWeakLambda(T* pointer, Functor lambda)
    {
        return WeakLambda<T, Functor>(pointer, lambda);
    }
};

template<typename T>
struct WeakLambdaHelper<T, true>
{
    template<typename Functor>
    static WeakLambda<EnableWeakLambdaCapture::Holder, Functor> CreateWeakLambda(T* pointer, Functor lambda)
    {
        return pointer->CreateWeakLambda(lambda);
    }
};

template<typename T, typename Functor>
auto weak_lambda(T* pointer, Functor lambda) -> WeakLambda<typename std::conditional<std::is_base_of<EnableWeakLambdaCapture, T>::value, EnableWeakLambdaCapture::Holder, T>::type, Functor>
{
    return WeakLambdaHelper<T, std::is_base_of<EnableWeakLambdaCapture, T>::value>::CreateWeakLambda(pointer, lambda);
}