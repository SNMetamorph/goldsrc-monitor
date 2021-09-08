#pragma once

#include <polyhook2/ZydisDisassembler.hpp>

#if APP_SUPPORT_64BIT
#include <polyhook2/Detour/x64Detour.hpp>
#define ARCH_DETOUR     x64Detour
#define ARCH_ZYDIS_MODE x64
#else
#include <polyhook2/Detour/x86Detour.hpp>
#define ARCH_DETOUR     x86Detour
#define ARCH_ZYDIS_MODE x86
#endif

template <class T> class CFunctionHook
{
public:
    CFunctionHook() {};
    ~CFunctionHook() {};
    inline uint64_t GetTrampolineAddr() const { return m_pfnTrampoline; }
    inline bool IsHooked() const { return m_isHooked; }

    bool Hook(T origFunc, T callbackFunc)
    {
        if (origFunc == nullptr) {
            return false;
        }
        m_pDetour = new PLH::ARCH_DETOUR(
            reinterpret_cast<char *>(origFunc),
            reinterpret_cast<char *>(callbackFunc),
            &m_pfnTrampoline,
            m_Disassembler
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
    PLH::ARCH_DETOUR *m_pDetour = nullptr;
    static PLH::ZydisDisassembler m_Disassembler;
};
template <class T> PLH::ZydisDisassembler CFunctionHook<T>::m_Disassembler(PLH::Mode::ARCH_ZYDIS_MODE);
