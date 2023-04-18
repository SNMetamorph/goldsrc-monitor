#pragma once
#include <memory>

class CHooks
{
public:
    class Impl;
    class Logger;

    CHooks();
    ~CHooks();
    void Apply();
    void Remove();

private:
    std::unique_ptr<Impl> m_pImpl;
};
