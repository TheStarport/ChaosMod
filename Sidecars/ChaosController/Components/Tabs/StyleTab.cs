using System.Numerics;
using ChaosMod.Common;
using ImGuiNET;

namespace ChaosMod.ChaosController.Components.Tabs;

public static partial class Tabs
{
    private static StyleTab? _styleTabInformation;

    public static void DisplayStyleTab(ChaosModConfig? config)
    {
        if (config is null)
        {
            return;
        }

        if (_styleTabInformation is null)
        {
            _styleTabInformation = new StyleTab
            {
                StartingProgressBarColorVec = ImGui.ColorConvertU32ToFloat4(config.ChaosSettings.ProgressBarColor),
                StartingProgressBarTextColorVec = ImGui.ColorConvertU32ToFloat4(config.ChaosSettings.ProgressBarTextColor)
            };

            _styleTabInformation.StartingProgressColor = new Vector3(_styleTabInformation.StartingProgressBarColorVec.X,
                _styleTabInformation.StartingProgressBarColorVec.Y, _styleTabInformation.StartingProgressBarColorVec.Z);
            _styleTabInformation.StartingProgressTextColor = new Vector3(_styleTabInformation.StartingProgressBarTextColorVec.X,
                _styleTabInformation.StartingProgressBarTextColorVec.Y, _styleTabInformation.StartingProgressBarTextColorVec.Z);
        }

        ImGui.ColorEdit3("Chaos Progress Color", ref _styleTabInformation.StartingProgressColor, ImGuiColorEditFlags.PickerHueBar);
        config.ChaosSettings.ProgressBarColor = ImGui.ColorConvertFloat4ToU32(new Vector4(_styleTabInformation.StartingProgressColor.X,
            _styleTabInformation.StartingProgressColor.Y, _styleTabInformation.StartingProgressColor.Z, 0xFF));
        ;
        ImGui.ColorEdit3("Chaos Progress Text Color", ref _styleTabInformation.StartingProgressTextColor, ImGuiColorEditFlags.PickerHueBar);
        config.ChaosSettings.ProgressBarTextColor =
            ImGui.ColorConvertFloat4ToU32(new Vector4(_styleTabInformation.StartingProgressTextColor.X, _styleTabInformation.StartingProgressTextColor.Y,
                _styleTabInformation.StartingProgressTextColor.Z, 0xFF));

        var options = new[]
        {
            "Progress Bar: Top",
            "Progress Bar: Sides",
            "Clock",
            "Countdown"
        };

        var index = (int)config.ChaosSettings.ProgressBarType;
        ImGui.Combo("Chaos Progress Bar Type:", ref index, options, options.Length);
        config.ChaosSettings.ProgressBarType = (ProgressBar)index;

        ImGui.Checkbox("Show Time Remaining On Effects", ref config.ChaosSettings.ShowTimeRemainingOnEffects);
    }

    private class StyleTab
    {
        public Vector4 StartingProgressBarColorVec;
        public Vector4 StartingProgressBarTextColorVec;
        public Vector3 StartingProgressColor;
        public Vector3 StartingProgressTextColor;
    }
}