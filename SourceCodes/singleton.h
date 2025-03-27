#pragma once

template<class T>
class Singleton{
public:
    static T* GetInstance()
    {
        static T instance;
        return &instance;
    }
private:
    Singleton();
    ~Singleton();
    Singleton(const Singleton&) = delete;
    Singleton &operator=(const Singleton&) = delete;
};