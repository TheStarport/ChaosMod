#pragma once
#include "Structures.hpp"

#ifdef MIDL_INTERFACE_NEW
    #undef MIDL_INTERFACE_NEW
    #undef INTERFACE_NEW
#endif

#define INTERFACE_NEW(x)            __declspec(uuid(x))
#define MIDL_INTERFACE_NEW(x)       INTERFACE_NEW(x) __declspec(novtable)

#define EventTractor                "tractor"
#define EventNext                   "next"
#define EventPrev                   "prev"
#define EventNextSubTarget          "next_subtarget"
#define EventPrevSubTarget          "prev_subtarget"
#define EventNull                   ""

#define PerformToggleOff            Perform(0x30, 0, 0)
#define PerformToggleOn             Perform(0x30, 1, 0)

#define PerformButtonOff            Perform(0xC, 0, 0)
#define PerformButtonOn             Perform(0xC, 1, 0)

#define PerformSliderSetCapacity(x) Perform(0x37, 1, x)
#define PerformSliderSetPosition(x) Perform(0x38, x, 0)

// Actually this interface probably is built-in already, but may be not

__interface MIDL_INTERFACE_NEW("00000000-0000-0000-C000-000000000046") IUnknownRedef
{
public:
    // In QueryInterace rid ( * 16 bytes passed) must be compared against IHud rid (16 bytes) and when matches do return a pointer at IHud vmt.
    virtual HRESULT __stdcall QueryInterface(REFIID rid, LPVOID * ppvObj);
    // Nah, easy way is create global variable of the object class and return -1 in this method
    virtual int __stdcall _AddRef();
    // As per above
    virtual int __stdcall _Release();
};

__interface MIDL_INTERFACE_NEW("45F1E847-B5D0-421E-8E51-E45A3B64A648") IHud : public IUnknownRedef
{
public:
    // Sender is TControl class
    virtual void __stdcall Draw(TControl * sender);
    virtual void __stdcall Update(TControl * sender);
    virtual void __stdcall Execute(TControl * sender);
    virtual void __stdcall Terminate(TControl * sender);
};

__interface MIDL_INTERFACE_NEW("F99B8A4F-99F7-4DA6-91EC-74DD3286464A") IHudFacility : public IUnknownRedef
{
public:
    virtual bool __stdcall AddHud(IHud * hud);
};
