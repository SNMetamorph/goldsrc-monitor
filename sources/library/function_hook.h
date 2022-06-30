#pragma once

#include <polyhook2/ZydisDisassembler.hpp>
#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Detour/x86Detour.hpp>

template <class T> class CFunctionHook
{
private:
#if APP_SUPPORT_64BIT
    typedef PLH::x64Detour PolyhookDetourType_t;
#else
    typedef PLH::x86Detour PolyhookDetourType_t;
#endif

public:
    CFunctionHook() {};
    ~CFunctionHook() {};
    inline uint64_t GetTrampolineAddr() const { return m_pfnTrampoline; }
    inline bool IsHooked() const { return m_isHooked; }

    bool Hook(T origFunc, T callbackFunc)
    {
        m_pDetour = new PolyhookDetourType_t(
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
    PolyhookDetourType_t *m_pDetour = nullptr;
};
