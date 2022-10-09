#include <iostream>
#include <functional>

#include "WeakLambda.h"

class A : public EnableWeakLambdaCapture
{
public:
    int dedede = 10;
    auto hahaha()
    {
        auto result_lambda = weak_lambda(this, [this]() {
            std::cout << this->dedede << std::endl;
        });

        return result_lambda;
    }
};

class B : public std::enable_shared_from_this<B>
{
public:
    int dedede = 10;
    auto hahaha()
    {
        auto result_lambda = WeakLambda < B, std::function<void()> > (this, [this]() {
            std::cout << this->dedede << std::endl;
            });

        return result_lambda;
    }
};

int main()
{
    std::function<void()> callback_a = nullptr;
    std::function<void()> callback_b = nullptr;
    {
        A* a = new A;
        std::shared_ptr<B> b = std::make_shared<B>();
        //std::shared_ptr<A> a = std::make_shared<A>();
        callback_a = a->hahaha();
        callback_b = b->hahaha();
        delete a;
    }


    callback_a();
    callback_b();

    return 0;
}