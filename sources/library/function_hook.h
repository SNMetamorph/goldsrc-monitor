#pragma once

#include <polyhook2/ZydisDisassembler.hpp>
#include <polyhook2/Detour/NatDetour.hpp>

template <class T> class CFunctionHook
{
public:
    CFunctionHook() {};
    ~CFunctionHook() {};
    inline uint64_t GetTrampolineAddr() const { return m_pfnTrampoline; }
    inline bool IsHooked() const { return m_isHooked; }

    bool Hook(T origFunc, T callbackFunc)
    {
        m_pDetour = new PLH::NatDetour(
            reinterpret_cast<uint64_t>(origFunc),
            reinterpret_cast<uint64_t>(callbackFunc),
            &m_pfnTrampoline
        );
        m_isHooked = m_pDetour->hook();
        return m_isHooked;
    };

    bool Unhook()
    {
        if (m_pDetour && m_isHooked)
        {
            bool isUnhooked = m_pDetour->unHook();
            if (isUnhooked)
            {
                m_isHooked = false;
                delete m_pDetour;
                m_pDetour = nullptr;
            }
            return isUnhooked;
        }
        return false;
    };
    
private:
    bool m_isHooked = false;
    uint64_t m_pfnTrampoline = 0;
    PLH::NatDetour *m_pDetour = nullptr;
};
