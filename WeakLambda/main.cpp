#include <iostream>
#include <functional>

#include "WeakLambda.h"

class A : public EnableWeakLambdaCapture
{
public:
    int dedede = 10;
    std::function<void()> hahaha()
    {
        auto result_lambda = weak_lambda(this, [this]() {
            std::cout << this->dedede << std::endl;
        });

        return result_lambda;
    }
};

int main()
{
    std::function<void()> callback = nullptr;
    {
        A* a = new A;
        //std::shared_ptr<A> a = std::make_shared<A>();
        callback = a->hahaha();
        delete a;
    }


    callback();

    return 0;
}