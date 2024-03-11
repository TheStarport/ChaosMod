#pragma once

namespace ImGuiExt
{
    bool BeginPiePopup(const char* name);
    void EndPiePopup();

    bool PieMenuItem(const char* name, bool bEnabled = true);
    bool BeginPieMenu(const char* name, bool bEnabled = true);
    void EndPieMenu();
} // namespace ImGuiExt
