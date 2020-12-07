//
// Created by Jianlang Huang on 12/7/20.
//

#ifndef WEBFRAMEWORK_SINGLETON_H
#define WEBFRAMEWORK_SINGLETON_H

#include <memory>

template <typename T, typename X=void, int N=0>
class Singleton {
public:
    static T* GetInstance() {
        static T instance;
        return &instance;
    }
};

template <typename T, typename X=void, int N=0>
class SingletonPtr{
public:
    static std::shared_ptr<T> GetInstance(){
        static std::shared_ptr<T> instance(new T);
        return instance;
    }
};

#endif //WEBFRAMEWORK_SINGLETON_H
