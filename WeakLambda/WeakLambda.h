#pragma once

template <typename T, typename Func>
class WeakLambda
{
public:
    WeakLambda() = delete;

    WeakLambda(T* raw_obj, const Func& func) : func_(func), weak_obj_(raw_obj->shared_from_this())
    {
    }

    WeakLambda(const std::shared_ptr<T>& ptr, const Func& func) : func_(func), weak_obj_(ptr)
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
    auto operator()(ArgTypes... args) -> typename std::result_of<Func(ArgTypes...)>::type
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
    Func func_;
    std::weak_ptr<T> weak_obj_;
};


class EnableWeakLambdaCapture
{
public:
    class Holder : public std::enable_shared_from_this<Holder>
    {};

    template<typename Func>
    WeakLambda<Holder, Func> CreateWeakLambda(const Func& lambda)
    {
        if (!holder_)
        {
            holder_ = std::make_shared<Holder>();
        }

        return WeakLambda<Holder, Func>(holder_, lambda);
    }

private:
    std::shared_ptr<Holder> holder_;
};

template<typename T, bool>
struct WeakLambdaHelper
{   
    template<typename Func>
    static WeakLambda<T, Func> CreateWeakLambda(T* pointer, Func lambda)
    {
        return WeakLambda<T, Func>(pointer, lambda);
    }
};

template<typename T>
struct WeakLambdaHelper<T, true>
{
    template<typename Func>
    static WeakLambda<EnableWeakLambdaCapture::Holder, Func> CreateWeakLambda(T* pointer, Func lambda)
    {
        return pointer->CreateWeakLambda(lambda);
    }
};

template<typename T, typename Func>
auto weak_lambda(T* pointer, Func lambda) -> WeakLambda<typename std::conditional<std::is_base_of<EnableWeakLambdaCapture, T>::value, EnableWeakLambdaCapture::Holder, T>::type, Func>
{
    return WeakLambdaHelper<T, std::is_base_of<EnableWeakLambdaCapture, T>::value>::CreateWeakLambda(pointer, lambda);
}