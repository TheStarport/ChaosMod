using System.Numerics;
using ChaosMod.Common;
using ImGuiNET;

namespace ChaosMod.ChaosController.Components.Tabs;

public static partial class Tabs
{
    public static void DisplayEffectToggleTab(ChaosModConfig? config)
    {
        if (config is null)
        {
            return;
        }

        if (config.ChaosSettings.ToggledEffects.Count is 0)
        {
            ImGui.Text("Toggled effects will not appear until the config is saved within the game at least once.");
            return;
        }

        foreach (var category in config.ChaosSettings.ToggledEffects)
        {
            ImGui.PushID(category.Key);
            var allSelected = category.Value.Values.All(x => x);

            if (ImGui.Checkbox("All?##ID", ref allSelected))
            {
                var e = category.Key;
                config.ChaosSettings.ToggledEffects[category.Key] = category.Value.ToDictionary(p => p.Key, _ => allSelected);
            }

            ImGui.SameLine();
            ImGui.PushStyleVar(ImGuiStyleVar.SeparatorTextAlign, new Vector2(0.45f, 0.5f));
            ImGui.SeparatorText(category.Key);
            ImGui.PopStyleVar();

            ImGui.NewLine();

            if (ImGui.BeginTable(category.Key, 2))
            {
                foreach (var effect in category.Value)
                {
                    var isEnabled = effect.Value;

                    ImGui.TableNextColumn();
                    if (ImGui.Checkbox(effect.Key, ref isEnabled))
                    {
                        config.ChaosSettings.ToggledEffects[category.Key][effect.Key] = isEnabled;
                    }
                }
                ImGui.EndTable();
            }

            ImGui.NewLine();
            ImGui.PopID();
        }
    }
}