#pragma once
// polyhook headers
#include <polyhook2/ZydisDisassembler.hpp>
#include <polyhook2/Detour/x86Detour.hpp>

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
        m_pDetour = new PLH::x86Detour(
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
        if (m_pDetour)
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
    PLH::x86Detour *m_pDetour = nullptr;
    static PLH::ZydisDisassembler m_Disassembler;
};
template <class T> PLH::ZydisDisassembler CFunctionHook<T>::m_Disassembler(PLH::Mode::x86);
