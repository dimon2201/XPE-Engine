#pragma once

#define Singleton(Class) \
                                \
public:                         \
    static Class& Get() {       \
        static Class instance;  \
        return instance;        \
    }                           \
                                \
private:                        \
    Class() {}                  \
                                \
public:                         \
    Class(Class const&) = delete;   \
    void operator=(Class const&) = delete;  \