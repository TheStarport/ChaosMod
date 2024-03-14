#include "PCH.hpp"

#include "ImGui/Components/PiMenu.hpp"

#include "imgui_internal.h"

namespace ImGuiExt
{
    struct PieMenuContext
    {
            static constexpr int maxPieMenuStack = 8;
            static constexpr int maxPieItemCount = 12;
            static constexpr int radiusEmpty = 30;
            static constexpr int radiusMin = 30;
            static constexpr int minItemCount = 3;
            static constexpr int minItemCountPerLevel = 3;

            struct PieMenu
            {
                    int currentIndex;
                    float maxItemSqrDiameter;
                    float lastMaxItemSqrDiameter;
                    int hoveredItem;
                    int lastHoveredItem;
                    int clickedItem;
                    bool itemIsSubMenu[maxPieItemCount];
                    ImVector<char> itemNames[maxPieItemCount];
                    ImVec2 itemSizes[maxPieItemCount];
            };

            PieMenuContext()
            {
                currentIndex = -1;
                lastFrame = 0;
            }

            PieMenu pieMenuStack[maxPieMenuStack]{};
            int currentIndex;
            int maxIndex = 0;
            int lastFrame;
            ImVec2 center{};
            int mouseButton = 0;
            bool close = false;
    };

    static PieMenuContext pieMenuContext;

    bool IsPopupOpen(const char* pName)
    {
        const ImGuiID id = ImGui::GetID(pName);
        ImGuiContext& g = *GImGui;
        return g.OpenPopupStack.Size > g.BeginPopupStack.Size && g.OpenPopupStack[g.BeginPopupStack.Size].PopupId == id;
    }

    void BeginPieMenuEx()
    {
        IM_ASSERT(pieMenuContext.currentIndex < PieMenuContext::maxPieMenuStack);

        ++pieMenuContext.currentIndex;
        ++pieMenuContext.maxIndex;

        PieMenuContext::PieMenu& oPieMenu = pieMenuContext.pieMenuStack[pieMenuContext.currentIndex];
        oPieMenu.currentIndex = 0;
        oPieMenu.maxItemSqrDiameter = 0.f;
        if (!ImGui::IsMouseReleased(0))
        {
            oPieMenu.hoveredItem = -1;
        }
        if (pieMenuContext.currentIndex > 0)
        {
            oPieMenu.maxItemSqrDiameter = pieMenuContext.pieMenuStack[pieMenuContext.currentIndex - 1].maxItemSqrDiameter;
        }
    }

    void EndPieMenuEx()
    {
        IM_ASSERT(pieMenuContext.currentIndex >= 0);
        PieMenuContext::PieMenu& oPieMenu = pieMenuContext.pieMenuStack[pieMenuContext.currentIndex];

        --pieMenuContext.currentIndex;
    }

    bool BeginPiePopup(const char* name)
    {
        if (IsPopupOpen(name))
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

            pieMenuContext.close = false;

            ImGui::SetNextWindowPos(ImVec2(-100.f, -100.f), ImGuiCond_Appearing);
            if (ImGui::BeginPopupModal(name,
                                       nullptr,
                                       ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                                           ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollWithMouse))
            {
                const int iCurrentFrame = ImGui::GetFrameCount();
                if (pieMenuContext.lastFrame < (iCurrentFrame - 1))
                {
                    pieMenuContext.center = ImGui::GetIO().MousePos;
                }
                pieMenuContext.lastFrame = iCurrentFrame;

                pieMenuContext.maxIndex = -1;
                BeginPieMenuEx();

                return true;
            }

            ImGui::End();
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(2);
        }
        return false;
    }

    void EndPiePopup()
    {
        EndPieMenuEx();

        const ImGuiStyle& style = ImGui::GetStyle();
        const ImVec2& texUvWhitePixel = ImGui::GetDrawListSharedData()->TexUvWhitePixel;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->PushClipRectFullScreen();

        const ImVec2 mousePos = ImGui::GetIO().MousePos;
        const ImVec2 dragDelta = ImVec2(mousePos.x - pieMenuContext.center.x, mousePos.y - pieMenuContext.center.y);
        const float dragDistSqr = dragDelta.x * dragDelta.x + dragDelta.y * dragDelta.y;

        float currentRadius = static_cast<float>(PieMenuContext::radiusEmpty);

        auto area = ImRect(pieMenuContext.center, pieMenuContext.center);

        bool itemHovered = false;

        constexpr float defaultRotate = -IM_PI / 2.f;
        float lastRotate = defaultRotate;
        for (int index = 0; index <= pieMenuContext.maxIndex; ++index)
        {
            PieMenuContext::PieMenu& pieMenu = pieMenuContext.pieMenuStack[index];

            const float menuHeight = sqrt(pieMenu.maxItemSqrDiameter);

            const float minRadius = currentRadius;
            const float maxRadius = minRadius + (menuHeight * pieMenu.currentIndex) / (2.f);

            const float itemArcSpan = 2 * IM_PI / ImMax(PieMenuContext::minItemCount + PieMenuContext::minItemCountPerLevel * index, pieMenu.currentIndex);
            float dragAngle = atan2f(dragDelta.y, dragDelta.x);

            const float fRotate = lastRotate - itemArcSpan * (pieMenu.currentIndex - 1.f) / 2.f;
            int item_hovered = -1;
            for (int itemN = 0; itemN < pieMenu.currentIndex; itemN++)
            {
                const char* itemLabel = pieMenu.itemNames[itemN].Data;
                const float innerSpacing = style.ItemInnerSpacing.x / minRadius / 2;
                const float minInnerSpacing = style.ItemInnerSpacing.x / (minRadius * 2.f);
                const float maxInnerSpacing = style.ItemInnerSpacing.x / (maxRadius * 2.f);
                const float itemInnerAngMin = itemArcSpan * (itemN - 0.5f + minInnerSpacing) + fRotate;
                const float itemInnerAngMax = itemArcSpan * (itemN + 0.5f - minInnerSpacing) + fRotate;
                const float itemOuterAngMin = itemArcSpan * (itemN - 0.5f + maxInnerSpacing) + fRotate;
                const float itemOuterAngMax = itemArcSpan * (itemN + 0.5f - maxInnerSpacing) + fRotate;

                bool hovered = false;
                if (dragDistSqr >= minRadius * minRadius && dragDistSqr < maxRadius * maxRadius)
                {
                    while ((dragAngle - itemInnerAngMin) < 0.f)
                    {
                        dragAngle += 2.f * IM_PI;
                    }
                    while ((dragAngle - itemInnerAngMin) > 2.f * IM_PI)
                    {
                        dragAngle -= 2.f * IM_PI;
                    }

                    if (dragAngle >= itemInnerAngMin && dragAngle < itemInnerAngMax)
                    {
                        hovered = true;
                        itemHovered = !pieMenu.itemIsSubMenu[itemN];
                    }
                }

                const int segments = static_cast<int>(32 * itemArcSpan / (2 * IM_PI)) + 1;

                const ImU32 color = ImGui::GetColorU32(hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
                // iColor |= 0xFF000000;

                const float angleStepInner = (itemInnerAngMax - itemInnerAngMin) / segments;
                const float angleStepOuter = (itemOuterAngMax - itemOuterAngMin) / segments;
                drawList->PrimReserve(segments * 6, (segments + 1) * 2);
                for (int seg = 0; seg <= segments; ++seg)
                {
                    const float cosInner = cosf(itemInnerAngMin + angleStepInner * seg);
                    const float sinInner = sinf(itemInnerAngMin + angleStepInner * seg);
                    const float cosOuter = cosf(itemOuterAngMin + angleStepOuter * seg);
                    const float sinOuter = sinf(itemOuterAngMin + angleStepOuter * seg);

                    if (seg < segments)
                    {
                        drawList->PrimWriteIdx(drawList->_VtxCurrentIdx + 0);
                        drawList->PrimWriteIdx(drawList->_VtxCurrentIdx + 2);
                        drawList->PrimWriteIdx(drawList->_VtxCurrentIdx + 1);
                        drawList->PrimWriteIdx(drawList->_VtxCurrentIdx + 3);
                        drawList->PrimWriteIdx(drawList->_VtxCurrentIdx + 2);
                        drawList->PrimWriteIdx(drawList->_VtxCurrentIdx + 1);
                    }
                    drawList->PrimWriteVtx(ImVec2(pieMenuContext.center.x + cosInner * (minRadius + style.ItemInnerSpacing.x),
                                                  pieMenuContext.center.y + sinInner * (minRadius + style.ItemInnerSpacing.x)),
                                           texUvWhitePixel,
                                           color);
                    drawList->PrimWriteVtx(ImVec2(pieMenuContext.center.x + cosOuter * (maxRadius - style.ItemInnerSpacing.x),
                                                  pieMenuContext.center.y + sinOuter * (maxRadius - style.ItemInnerSpacing.x)),
                                           texUvWhitePixel,
                                           color);
                }

                const float fRadCenter = (itemArcSpan * itemN) + fRotate;
                ImVec2 oOuterCenter = ImVec2(pieMenuContext.center.x + cosf(fRadCenter) * maxRadius, pieMenuContext.center.y + sinf(fRadCenter) * maxRadius);
                area.Add(oOuterCenter);

                if (pieMenu.itemIsSubMenu[itemN])
                {
                    ImVec2 trianglePos[3];

                    const float radLeft = fRadCenter - 5.f / maxRadius;
                    const float radRight = fRadCenter + 5.f / maxRadius;

                    trianglePos[0].x = pieMenuContext.center.x + cosf(fRadCenter) * (maxRadius - 5.f);
                    trianglePos[0].y = pieMenuContext.center.y + sinf(fRadCenter) * (maxRadius - 5.f);
                    trianglePos[1].x = pieMenuContext.center.x + cosf(radLeft) * (maxRadius - 10.f);
                    trianglePos[1].y = pieMenuContext.center.y + sinf(radLeft) * (maxRadius - 10.f);
                    trianglePos[2].x = pieMenuContext.center.x + cosf(radRight) * (maxRadius - 10.f);
                    trianglePos[2].y = pieMenuContext.center.y + sinf(radRight) * (maxRadius - 10.f);

                    drawList->AddTriangleFilled(trianglePos[0], trianglePos[1], trianglePos[2], ImColor(255, 255, 255));
                }

                const ImVec2 textSize = pieMenu.itemSizes[itemN];
                auto textPos =
                    ImVec2(pieMenuContext.center.x + cosf((itemInnerAngMin + itemInnerAngMax) * 0.5f) * (minRadius + maxRadius) * 0.5f - textSize.x * 0.5f,
                           pieMenuContext.center.y + sinf((itemInnerAngMin + itemInnerAngMax) * 0.5f) * (minRadius + maxRadius) * 0.5f - textSize.y * 0.5f);
                drawList->AddText(textPos, ImColor(255, 255, 255), itemLabel);

                if (hovered)
                {
                    item_hovered = itemN;
                }
            }

            currentRadius = maxRadius;

            pieMenu.lastMaxItemSqrDiameter = pieMenu.maxItemSqrDiameter;

            pieMenu.hoveredItem = item_hovered;

            if (dragDistSqr >= maxRadius * maxRadius)
            {
                item_hovered = pieMenu.lastHoveredItem;
            }

            pieMenu.lastHoveredItem = item_hovered;

            lastRotate = itemArcSpan * pieMenu.lastHoveredItem + fRotate;
            if (item_hovered == -1 || !pieMenu.itemIsSubMenu[item_hovered])
            {
                break;
            }
        }

        drawList->PopClipRect();

        if (area.Min.x < 0.f)
        {
            pieMenuContext.center.x = (pieMenuContext.center.x - area.Min.x);
        }
        if (area.Min.y < 0.f)
        {
            pieMenuContext.center.y = (pieMenuContext.center.y - area.Min.y);
        }

        const ImVec2 oDisplaySize = ImGui::GetIO().DisplaySize;
        if (area.Max.x > oDisplaySize.x)
        {
            pieMenuContext.center.x = (pieMenuContext.center.x - area.Max.x) + oDisplaySize.x;
        }
        if (area.Max.y > oDisplaySize.y)
        {
            pieMenuContext.center.y = (pieMenuContext.center.y - area.Max.y) + oDisplaySize.y;
        }

        if (pieMenuContext.close || (!itemHovered && ImGui::IsMouseReleased(0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }

    bool BeginPieMenu(const char* name, bool bEnabled)
    {
        IM_ASSERT(pieMenuContext.currentIndex >= 0 && pieMenuContext.currentIndex < PieMenuContext::maxPieItemCount);

        PieMenuContext::PieMenu& pieMenu = pieMenuContext.pieMenuStack[pieMenuContext.currentIndex];

        const ImVec2 textSize = ImGui::CalcTextSize(name, nullptr, true);
        pieMenu.itemSizes[pieMenu.currentIndex] = textSize;

        if (const float sqrDiameter = textSize.x * textSize.x + textSize.y * textSize.y; sqrDiameter > pieMenu.maxItemSqrDiameter)
        {
            pieMenu.maxItemSqrDiameter = sqrDiameter;
        }

        pieMenu.itemIsSubMenu[pieMenu.currentIndex] = true;

        const int len = static_cast<int>(strlen(name));
        ImVector<char>& itemName = pieMenu.itemNames[pieMenu.currentIndex];
        itemName.resize(len + 1);
        itemName[len] = '\0';
        memcpy(itemName.Data, name, len);

        if (pieMenu.lastHoveredItem == pieMenu.currentIndex)
        {
            ++pieMenu.currentIndex;

            BeginPieMenuEx();
            return true;
        }
        ++pieMenu.currentIndex;

        return false;
    }

    void EndPieMenu()
    {
        IM_ASSERT(pieMenuContext.currentIndex >= 0 && pieMenuContext.currentIndex < PieMenuContext::maxPieItemCount);
        --pieMenuContext.currentIndex;
    }

    bool PieMenuItem(const char* name, bool bEnabled)
    {
        IM_ASSERT(pieMenuContext.currentIndex >= 0 && pieMenuContext.currentIndex < PieMenuContext::maxPieItemCount);

        PieMenuContext::PieMenu& pieMenu = pieMenuContext.pieMenuStack[pieMenuContext.currentIndex];

        const ImVec2 oTextSize = ImGui::CalcTextSize(name, nullptr, true);
        pieMenu.itemSizes[pieMenu.currentIndex] = oTextSize;

        const float sqrDiameter = oTextSize.x * oTextSize.x + oTextSize.y * oTextSize.y;

        if (sqrDiameter > pieMenu.maxItemSqrDiameter)
        {
            pieMenu.maxItemSqrDiameter = sqrDiameter;
        }

        pieMenu.itemIsSubMenu[pieMenu.currentIndex] = false;

        const int len = static_cast<int>(strlen(name));
        ImVector<char>& itemName = pieMenu.itemNames[pieMenu.currentIndex];
        itemName.resize(len + 1);
        itemName[len] = '\0';
        memcpy(itemName.Data, name, len);

        const bool active = pieMenu.currentIndex == pieMenu.hoveredItem;
        ++pieMenu.currentIndex;

        if (active && ImGui::IsMouseReleased(0))
        {
            pieMenuContext.close = true;
            return true;
        }

        return false;
    }
} // namespace ImGuiExt
