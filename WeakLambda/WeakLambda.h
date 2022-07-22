#pragma once

#include <functional>


class EnableWeakLambdaCapture
{
public:
    class Holder : public std::enable_shared_from_this<Holder>
    {};

    template<typename Functor, typename... T>
    std::function<void(T... lambda_args)> CreateWeakCallback(const Functor& lambda)
    {
        if (!holder_)
        {
            holder_ = std::make_shared<Holder>();
        }

        auto weak_callback = [weak_p = std::weak_ptr<Holder>(holder_->shared_from_this()), lambda](T... lambda_args){
            std::shared_ptr<Holder> ptr = weak_p.lock();
            if (ptr)
                lambda(lambda_args...);
        };

        return weak_callback;
    }

private:
    std::shared_ptr<Holder> holder_;
};

template<typename T, typename Functor, typename... Args>
std::function<void(Args... lambda_args)> weak_lambda(T* pointer, Functor lambda)
{
    return pointer->CreateWeakCallback(lambda);
}