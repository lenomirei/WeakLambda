#pragma once

#include <functional>

class EnableWeakLambdaCapture : public std::enable_shared_from_this<EnableWeakLambdaCapture>
{

};

template<typename Functor, typename... T>
std::function<void(T... lambda_args)> weak_lambda(EnableWeakLambdaCapture* this_pointer, Functor lambda)
{
    return [weak_p = std::weak_ptr<EnableWeakLambdaCapture>(this_pointer->shared_from_this()), lambda](T... lambda_args){
        std::shared_ptr<EnableWeakLambdaCapture> ptr = weak_p.lock();
        if (ptr)
            lambda(lambda_args...);
    };
}