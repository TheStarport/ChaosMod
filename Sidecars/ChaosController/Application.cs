using System.Diagnostics;
using System.Numerics;
using System.Runtime.InteropServices;
using ChaosMod.Api;
using ChaosMod.ChaosController.Components;
using ChaosMod.ChaosController.Components.Tabs;
using ChaosMod.Common;
using ImGuiNET;
using SDL3;

namespace ChaosMod.ChaosController;

public class Application : IDisposable
{
    private readonly nint _device;

    private readonly Dictionary<Font, ImFontPtr> _fonts = [];
    private readonly Sdl3 _platform;
    private readonly ImGuiRenderer _renderer;

    private readonly Stopwatch _timer = Stopwatch.StartNew();
    private readonly nint _window;

    private ChaosModConfig? _chaosModConfig;
    private SDL.Rect _screenClipRect;
    private TimeSpan _time = TimeSpan.Zero;

    private readonly WebApi _webApi = new();
    private readonly CancellationTokenSource _apiCts = new();

    public Application(string name, int width, int height)
    {
        if (!SDL.Init(SDL.InitFlags.Video))
        {
            throw new Exception($"SDL_Init failed: {SDL.GetError()}");
        }

        // Create window & renderer
        const SDL.WindowFlags windowFlags = SDL.WindowFlags.Transparent | SDL.WindowFlags.MouseFocus | SDL.WindowFlags.Borderless;
        if (!SDL.CreateWindowAndRenderer(name, width, height, windowFlags, out _window, out _device))
        {
            throw new Exception($"SDL_CreateWindowAndRenderer failed: {SDL.GetError()}");
        }

        // Enable VSync
        SDL.SetRenderVSync(_device, 1);

        // Setup screen clip rect
        SetupScreenClipRect();

        // Create ImGui context
        var context = ImGui.CreateContext();
        ImGui.SetCurrentContext(context);
        SetDefaultStyle();

        _platform = new Sdl3(_window, _device);
        _renderer = new ImGuiRenderer(_device);

        // Set fonts
        var fonts = ImGui.GetIO().Fonts;
        _fonts[Font.Default] = fonts.AddFontFromFileTTF("TitilliumWeb.ttf", 24);

        // Enable Font Awesome support
        unsafe
        {
            ushort[] ranges =
            [
                Icon.IconMin,
                Icon.IconMax,
                0
            ];

            ImFontConfigPtr fontConfig = ImGuiNative.ImFontConfig_ImFontConfig();
            fontConfig.MergeMode = true;
            fontConfig.OversampleH = 3;
            fontConfig.OversampleV = 3;
            fontConfig.GlyphOffset = Vector2.Zero;
            fontConfig.FontDataOwnedByAtlas = false;
            fontConfig.PixelSnapH = true;
            fontConfig.GlyphMaxAdvanceX = float.MaxValue;
            fontConfig.RasterizerMultiply = 2.0f;

            var rangesIntPtr = Marshal.UnsafeAddrOfPinnedArrayElement(ranges, 0);
            fonts.AddFontFromFileTTF("Font-Awesome-5 Free-Solid-900.otf", 18,
                fontConfig,
                rangesIntPtr);

            ranges =
            [
                BrandIcon.IconMin,
                BrandIcon.IconMax,
                0
            ];

            rangesIntPtr = Marshal.UnsafeAddrOfPinnedArrayElement(ranges, 0);
            fonts.AddFontFromFileTTF("Font-Awesome-5-Brands-Regular-400.otf", 18,
                fontConfig,
                rangesIntPtr);
        }

        // Start up web api
        _ = _webApi.RunAsync(_apiCts.Token);
    }

    public bool IsRunning { get; private set; }

    public void Dispose()
    {
        _apiCts.Cancel();

        GC.SuppressFinalize(this);
        IsRunning = false;
        ImGui.DestroyContext();
        _renderer.Dispose();
        SDL.DestroyWindow(_window);
        SDL.DestroyRenderer(_device);
        SDL.Quit();
    }

    ~Application()
    {
        Dispose();
    }

    public void Run()
    {
        IsRunning = true;

        while (IsRunning)
        {
            ImGui.GetIO().DeltaTime = (float)(_timer.Elapsed - _time).TotalSeconds;
            _time = _timer.Elapsed;

            PollEvents();
            Update();
            Render();
        }
    }

    private void PollEvents()
    {
        if (ImGui.GetIO().WantTextInput && !SDL.TextInputActive(_window))
        {
            SDL.StartTextInput(_window);
        }
        else if (!ImGui.GetIO().WantTextInput && SDL.TextInputActive(_window))
        {
            SDL.StopTextInput(_window);
        }

        while (SDL.PollEvent(out var ev))
        {
            _platform.ProcessEvent(ev);

            switch ((SDL.EventType)ev.Type)
            {
                case SDL.EventType.WindowCloseRequested:
                case SDL.EventType.Quit:
                    IsRunning = false;
                    break;
            }
        }
    }

    private void Widgets()
    {
        var open = true;
        ImGui.Begin("Chaos Controller", ref open, ImGuiWindowFlags.NoResize);

        if (_chaosModConfig == null)
        {
            _chaosModConfig = ChaosModConfig.Load(false);
            if (_chaosModConfig is null)
            {
                ImGui.OpenPopup("Config Load Failure");
            }
        }

        var pos = ImGui.GetWindowPos();
        if (pos.X != 0 || pos.Y != 0)
        {
            ImGui.SetWindowPos(new Vector2());
            SDL.GetWindowPosition(_window, out var x, out var y);
            SDL.SetWindowPosition(_window, x + (int)pos.X, y + (int)pos.Y);
        }

        if (ImGui.Button("Save Changes"))
        {
            _chaosModConfig!.Save();
            ImGui.OpenPopup("Config Saved!");
        }

        if (ImGui.BeginTabBar("Tabs"))
        {
            if (ImGui.BeginTabItem("Chaos Settings"))
            {
                Tabs.DisplayChaosTab(_chaosModConfig);
                ImGui.EndTabItem();
            }

            if (ImGui.BeginTabItem("Autosave Settings"))
            {
                Tabs.DisplayAutoSaveTab(_chaosModConfig);
                ImGui.EndTabItem();
            }

            if (ImGui.BeginTabItem("Style Settings"))
            {
                Tabs.DisplayStyleTab(_chaosModConfig);
                ImGui.EndTabItem();
            }

            if (ImGui.BeginTabItem("Patch Notes / Randomizer Settings"))
            {
                Tabs.DisplayPatchNoteTab(_chaosModConfig);
                ImGui.EndTabItem();
            }

            if (ImGui.BeginTabItem("Twitch Settings"))
            {
                Tabs.DisplayTwitchTab(_chaosModConfig);
                ImGui.EndTabItem();
            }

            if (ImGui.BeginTabItem("Effect Toggles"))
            {
                Tabs.DisplayEffectToggleTab(_chaosModConfig);
                ImGui.EndTabItem();
            }

            if (ImGui.BeginTabItem("Discord Settings"))
            {
                Tabs.DisplayDiscordTab(_chaosModConfig);
                ImGui.EndTabItem();
            }

            ImGui.EndTabBar();
        }

        var center = ImGui.GetMainViewport().GetCenter();
        ImGui.SetNextWindowPos(center, ImGuiCond.Appearing, new Vector2(0.5f, 0.5f));
        ImGui.SetNextWindowSize(new Vector2(400, 300));
        if (ImGui.BeginPopupModal("Config Load Failure", ImGuiWindowFlags.NoResize))
        {
            ImGui.TextWrapped(
                "Error while trying to parse chaos config file. Would you like to reset the config to default? If no, the file must be fixed before this tool can be run.");
            ImGui.NewLine();

            if (ImGui.Button("Reset"))
            {
                _chaosModConfig = ChaosModConfig.Load(true);
                ImGui.CloseCurrentPopup();
            }

            ImGui.SameLine();
            if (ImGui.Button("Ignore"))
            {
                ImGui.CloseCurrentPopup();
                open = false;
            }

            ImGui.EndPopup();
        }

        ImGui.SetNextWindowPos(center, ImGuiCond.Appearing, new Vector2(0.5f, 0.5f));
        ImGui.SetNextWindowSize(new Vector2(400, 300));
        if (ImGui.BeginPopupModal("Config Saved!", ImGuiWindowFlags.AlwaysAutoResize))
        {
            ImGui.Text("Config Saved Successfully!");
            ImGui.EndPopup();
        }

        ImGui.End();

        if (!open)
        {
            IsRunning = false;
        }
    }

    private void Update()
    {
        _platform.NewFrame();
        _renderer.NewFrame();
        ImGui.NewFrame();
        ImGui.PushFont(_fonts[Font.Default]);

        ImGui.SetNextWindowSize(new Vector2(1195, 595), ImGuiCond.Always);
        Widgets();

        ImGui.PopFont();
        ImGui.EndFrame();
    }

    private void Render()
    {
        SDL.RenderClear(_device);

        // Reset the clip rect to the screen size
        SDL.SetRenderClipRect(_device, _screenClipRect);

        // Render ImGui
        ImGui.Render();
        _renderer.RenderDrawData(ImGui.GetDrawData());

        SDL.RenderPresent(_device);
    }

    private void SetupScreenClipRect()
    {
        SDL.GetWindowSize(_window, out var w, out var h);
        _screenClipRect = new SDL.Rect
        {
            X = 0,
            Y = 0,
            W = w,
            H = h
        };
    }

    private void SetDefaultStyle()
    {
        var style = ImGui.GetStyle();

        var bgColor = new Vector4(0.0784f, 0.0431f, 0.2235f, 0.7f); // #140B39
        var bgColorChild = new Vector4(0.0784f, 0.0431f, 0.2235f, 0.0f); // #140B39

        var titleBgColor = new Vector4(0.00784314f, 0.105882f, 0.188235f, 1); // #021B30
        var borderColor = new Vector4(0.50f, 0.72f, 0.83f, 1.0f);
        var textColor = new Vector4(0.529f, 0.764f, 0.878f, 1.0f);
        var checkBoxCheckColor = new Vector4(0.16f, 0.59f, 0.16f, 1.0f); // #28962B
        var scrollBarColor = new Vector4(0.411f, 0.596f, 0.678f, 1.0f); // #6998AE
        var disabledColor = new Vector4(0.451f, 0.451f, 0.463f, 1.0f); // #737276
        var borderShadowColor = new Vector4(0.4549f, 0.6235f, 0.7569f, 1.0f); // #749FC1
        var frameBg = new Vector4(0.0f, 0.0f, 0.0f, 1.0f); // Schwarz
        var buttonNormalColor = new Vector4(0.00784314f, 0.105882f, 0.188235f, 1); // #021B30
        var buttonHoverColor = new Vector4(0.52f, 0.45f, 0.0f, 1.0f); // Hex #857400
        var comboHoverColor = new Vector4(0.52f, 0.45f, 0.0f, 1.0f); // Hex #857400

        style.Alpha = 1.0f;
        style.WindowPadding = new Vector2(8, 4.0f);
        style.WindowMinSize = new Vector2(20.0f, 20.0f);
        style.WindowTitleAlign = new Vector2(0.5f, 0.5f);
        style.SeparatorTextAlign = new Vector2(.5f, .5f);
        style.SeparatorTextPadding = new Vector2(20, 3);
        style.WindowMenuButtonPosition = ImGuiDir.None;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.ItemSpacing = new Vector2(20.0f, 4.0f);
        style.ItemInnerSpacing = new Vector2(6.0f, 3.0f);
        style.IndentSpacing = 20.0f;
        style.ColumnsMinSpacing = 6.0f;
        style.ScrollbarSize = 20.0f;
        style.GrabMinSize = 16.0f;
        style.TabBorderSize = 1.0f;
        style.ColorButtonPosition = ImGuiDir.Right;
        style.ButtonTextAlign = new Vector2(0.5f, 0.5f);
        style.SelectableTextAlign = new Vector2(0.0f, 0.0f);
        style.WindowBorderSize = 2.0f;
        style.FramePadding = new Vector2(6, 2.0f);
        style.CellPadding = new Vector2(3, 2.0f);

        style.WindowRounding = 5.0f;
        style.ChildRounding = 5.0f;
        style.FrameRounding = 5.0f;
        style.GrabRounding = 6.0f;
        style.PopupRounding = 5.0f;
        style.ScrollbarRounding = 6.0f;
        style.TabRounding = 5.0f;


        style.Colors[(int)ImGuiCol.Text] = textColor;
        style.Colors[(int)ImGuiCol.TextDisabled] = disabledColor;
        style.Colors[(int)ImGuiCol.WindowBg] = bgColor;
        style.Colors[(int)ImGuiCol.ChildBg] = bgColorChild;
        style.Colors[(int)ImGuiCol.PopupBg] = bgColor;
        style.Colors[(int)ImGuiCol.Border] = borderColor;
        style.Colors[(int)ImGuiCol.BorderShadow] = borderShadowColor;
        style.Colors[(int)ImGuiCol.FrameBg] = frameBg;
        style.Colors[(int)ImGuiCol.FrameBgHovered] = frameBg;
        style.Colors[(int)ImGuiCol.FrameBgActive] = frameBg;
        style.Colors[(int)ImGuiCol.TitleBg] = titleBgColor;
        style.Colors[(int)ImGuiCol.TitleBgActive] = titleBgColor;
        style.Colors[(int)ImGuiCol.TitleBgCollapsed] = titleBgColor;

        style.Colors[(int)ImGuiCol.MenuBarBg] = bgColor;
        style.Colors[(int)ImGuiCol.ScrollbarBg] = bgColor;
        style.Colors[(int)ImGuiCol.ScrollbarGrab] = scrollBarColor;
        style.Colors[(int)ImGuiCol.ScrollbarGrabHovered] = comboHoverColor;
        style.Colors[(int)ImGuiCol.ScrollbarGrabActive] = comboHoverColor;
        style.Colors[(int)ImGuiCol.CheckMark] = checkBoxCheckColor;
        style.Colors[(int)ImGuiCol.SliderGrab] = scrollBarColor;
        style.Colors[(int)ImGuiCol.SliderGrabActive] = comboHoverColor;
        style.Colors[(int)ImGuiCol.Button] = buttonNormalColor;
        style.Colors[(int)ImGuiCol.ButtonHovered] = buttonHoverColor;
        style.Colors[(int)ImGuiCol.ButtonActive] = buttonHoverColor;
        style.Colors[(int)ImGuiCol.Header] = bgColor;
        style.Colors[(int)ImGuiCol.HeaderHovered] = comboHoverColor;
        style.Colors[(int)ImGuiCol.HeaderActive] = comboHoverColor;
        style.Colors[(int)ImGuiCol.Separator] = scrollBarColor;
        style.Colors[(int)ImGuiCol.SeparatorHovered] = scrollBarColor;
        style.Colors[(int)ImGuiCol.SeparatorActive] = scrollBarColor;
        style.Colors[(int)ImGuiCol.ResizeGrip] = bgColor;
        style.Colors[(int)ImGuiCol.ResizeGripHovered] = titleBgColor;
        style.Colors[(int)ImGuiCol.ResizeGripActive] = bgColor;
        style.Colors[(int)ImGuiCol.Tab] = titleBgColor;
        style.Colors[(int)ImGuiCol.TabHovered] = buttonHoverColor;
        style.Colors[(int)ImGuiCol.TabSelected] = titleBgColor with
        {
            W = 1.0f
        };
        style.Colors[(int)ImGuiCol.TabDimmed] = titleBgColor;
        style.Colors[(int)ImGuiCol.TabDimmedSelected] = titleBgColor;

        style.Colors[(int)ImGuiCol.PlotLines] = new Vector4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
        style.Colors[(int)ImGuiCol.PlotLinesHovered] = new Vector4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
        style.Colors[(int)ImGuiCol.PlotHistogram] = new Vector4(1.0f, 0.2901960909366608f, 0.5960784554481506f, 1.0f);
        style.Colors[(int)ImGuiCol.PlotHistogramHovered] = new Vector4(0.9960784316062927f, 0.4745098054409027f, 0.6980392336845398f, 1.0f);
        style.Colors[(int)ImGuiCol.TextSelectedBg] = new Vector4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
        style.Colors[(int)ImGuiCol.DragDropTarget] = titleBgColor;
        style.Colors[(int)ImGuiCol.NavCursor] = new Vector4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
        style.Colors[(int)ImGuiCol.NavWindowingHighlight] = new Vector4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
        style.Colors[(int)ImGuiCol.NavWindowingDimBg] = new Vector4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
        style.Colors[(int)ImGuiCol.ModalWindowDimBg] = new Vector4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0);
    }
}