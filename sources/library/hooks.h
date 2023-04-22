/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

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
