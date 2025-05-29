using System.Numerics;
using ImGuiNET;
using SDL3;

namespace ChaosMod.ChaosController;

public sealed class ImGuiRenderer : IDisposable
{
    private readonly nint _renderer;
    private nint _fontTexture = IntPtr.Zero;

    public SDL.Rect DefaultClipRect = new();

    public ImGuiRenderer(nint renderer)
    {
        _renderer = renderer;

        var io = ImGui.GetIO();
        io.BackendFlags |= ImGuiBackendFlags.RendererHasVtxOffset;
    }

    public void Dispose()
    {
        DestroyDeviceObjects();
    }

    public void NewFrame()
    {
        if (_fontTexture == IntPtr.Zero)
        {
            CreateDeviceObjects();
        }
    }

    public void RenderDrawData(ImDrawDataPtr drawData)
    {
        // Skip if no data to render
        unsafe
        {
            if (drawData.NativePtr == null || drawData.CmdListsCount == 0)
            {
                return;
            }
        }

        // Get display size & framebuffer scale
        Vector2 renderScale = new(
            drawData.FramebufferScale.X,
            drawData.FramebufferScale.Y
        );

        var fbWidth = (int)(drawData.DisplaySize.X * renderScale.X);
        var fbHeight = (int)(drawData.DisplaySize.Y * renderScale.Y);
        if (fbWidth <= 0 || fbHeight <= 0)
        {
            return;
        }

        // Backup SDL renderer state
        var oldState = BackupRendererState();

        // Set up render state
        SetupRenderState();

        // Set render state in platform IO
        var platformIo = ImGui.GetPlatformIO();
        platformIo.Renderer_RenderState = _renderer;

        var clipOffset = drawData.DisplayPos;

        // Render command lists
        for (var n = 0; n < drawData.CmdListsCount; n++)
        {
            var cmdList = drawData.CmdLists[n];

            for (var cmdIndex = 0; cmdIndex < cmdList.CmdBuffer.Size; cmdIndex++)
            {
                var cmd = cmdList.CmdBuffer[cmdIndex];

                if (cmd.UserCallback != IntPtr.Zero)
                {
                    // User callback not implemented
                    continue;
                }

                // Apply clipping rectangle
                var clipRect = cmd.ClipRect;
                var r = CalculateClipRect(clipRect, clipOffset, renderScale, fbWidth, fbHeight);
                SDL.SetRenderClipRect(_renderer, r);

                // Get texture
                var texId = cmd.GetTexID();

                // Convert ImGui vertices to SDL vertices
                if (!RenderDrawCommand(cmdList, cmd, texId, renderScale))
                {
                    Console.WriteLine($"Failed to render ImGui draw command: {SDL.GetError()}");
                }
            }
        }

        // Reset render state
        platformIo.Renderer_RenderState = IntPtr.Zero;

        // Restore renderer state
        RestoreRendererState(oldState);
    }

    private bool RenderDrawCommand(ImDrawListPtr drawList, ImDrawCmdPtr cmd, nint texId, Vector2 scale)
    {
        // Get indices and vertices for this command
        var indexOffset = (int)cmd.IdxOffset;
        var vertexOffset = (int)cmd.VtxOffset;
        var elemCount = (int)cmd.ElemCount;

        // Create SDL vertices just for the vertices used by this command
        // Determine the vertex range by looking at the indices
        var minVertexIdx = ushort.MaxValue;
        ushort maxVertexIdx = 0;

        for (var i = 0; i < elemCount; i++)
        {
            var idx = drawList.IdxBuffer[indexOffset + i];
            minVertexIdx = Math.Min(minVertexIdx, idx);
            maxVertexIdx = Math.Max(maxVertexIdx, idx);
        }

        // Adjust for the vertex offset
        minVertexIdx = (ushort)(minVertexIdx + vertexOffset);
        maxVertexIdx = (ushort)(maxVertexIdx + vertexOffset);

        // Calculate the number of vertices we need
        var numVertices = maxVertexIdx - minVertexIdx + 1;

        // Create arrays for the vertices and indices we're going to use
        var vertices = new SDL.Vertex[numVertices];
        var indices = new int[elemCount];

        // Convert only the vertices we need
        for (var i = 0; i < numVertices; i++)
        {
            var vertIdx = minVertexIdx + i;
            var srcVert = drawList.VtxBuffer[vertIdx];

            // Convert from ImGui ABGR color to SDL RGBA color
            var col = srcVert.col;
            var r = (byte)(col >> 0 & 0xFF); // Extract R from the least significant byte
            var g = (byte)(col >> 8 & 0xFF); // Extract G
            var b = (byte)(col >> 16 & 0xFF); // Extract B
            var a = (byte)(col >> 24 & 0xFF); // Extract A from the most significant byte

            vertices[i] = new SDL.Vertex
            {
                Position = new SDL.FPoint
                {
                    X = srcVert.pos.X,
                    Y = srcVert.pos.Y
                },
                Color = new SDL.FColor
                {
                    R = r / 255f,
                    G = g / 255f,
                    B = b / 255f,
                    A = a / 255f
                },
                TexCoord = new SDL.FPoint
                {
                    X = srcVert.uv.X,
                    Y = srcVert.uv.Y
                }
            };
        }

        // Adjust indices to be relative to our new vertex array
        for (var i = 0; i < elemCount; i++)
        {
            var originalIdx = drawList.IdxBuffer[indexOffset + i];
            indices[i] = (ushort)(originalIdx - (minVertexIdx - vertexOffset));
        }

        // Call your SDL.RenderGeometry wrapper with the managed arrays
        return SDL.RenderGeometry(
            _renderer,
            texId,
            vertices,
            numVertices,
            indices,
            elemCount
        );
    }

    private void SetupRenderState()
    {
        SDL.SetRenderViewport(_renderer, 0);
        SDL.SetRenderClipRect(_renderer, IntPtr.Zero);
        SDL.SetRenderDrawBlendMode(_renderer, SDL.BlendMode.Blend);
    }

    private BackupSdlRendererState BackupRendererState()
    {
        BackupSdlRendererState state = new()
        {
            ViewportEnabled = SDL.RenderViewportSet(_renderer),
            ClipEnabled = SDL.RenderClipEnabled(_renderer)
        };

        SDL.GetRenderViewport(_renderer, out state.Viewport);
        SDL.GetRenderClipRect(_renderer, out state.ClipRect);
        return state;
    }

    private void RestoreRendererState(BackupSdlRendererState state)
    {
        if (state.ViewportEnabled)
        {
            SDL.SetRenderViewport(_renderer, state.Viewport);
        }
        else
        {
            SDL.SetRenderViewport(_renderer, IntPtr.Zero);
        }
        if (state.ClipEnabled)
        {
            SDL.SetRenderClipRect(_renderer, state.ClipRect);
        }
        else
        {
            SDL.SetRenderClipRect(_renderer, IntPtr.Zero);
        }
    }

    private static SDL.Rect CalculateClipRect(Vector4 clipRect, Vector2 clipOffset, Vector2 scale, int fbWidth, int fbHeight)
    {
        Vector2 clipMin = new((clipRect.X - clipOffset.X) * scale.X, (clipRect.Y - clipOffset.Y) * scale.Y);
        Vector2 clipMax = new((clipRect.Z - clipOffset.X) * scale.X, (clipRect.W - clipOffset.Y) * scale.Y);

        // Clamp to framebuffer bounds
        clipMin.X = Math.Max(0, clipMin.X);
        clipMin.Y = Math.Max(0, clipMin.Y);
        clipMax.X = Math.Min(fbWidth, clipMax.X);
        clipMax.Y = Math.Min(fbHeight, clipMax.Y);

        SDL.Rect r = new()
        {
            X = (int)clipMin.X,
            Y = (int)clipMin.Y,
            W = (int)(clipMax.X - clipMin.X),
            H = (int)(clipMax.Y - clipMin.Y)
        };

        return r;
    }

    private bool CreateDeviceObjects()
    {
        return CreateFontsTexture();
    }

    private void DestroyDeviceObjects()
    {
        DestroyFontsTexture();
    }

    private unsafe bool CreateFontsTexture()
    {
        var io = ImGui.GetIO();

        // Build texture atlas
        io.Fonts.GetTexDataAsRGBA32(out byte* pixels, out var width, out var height);

        // Create surface from pixel data
        var surface = SDL.CreateSurfaceFrom(width, height, SDL.PixelFormat.RGBA8888, (IntPtr)pixels, width * 4);
        if (surface == IntPtr.Zero)
        {
            SDL.LogError(SDL.LogCategory.Application, $"Failed to create font surface: {SDL.GetError()}");
            return false;
        }

        // Create texture
        _fontTexture = SDL.CreateTextureFromSurface(_renderer, surface);
        if (_fontTexture == IntPtr.Zero)
        {
            SDL.LogError(SDL.LogCategory.Application, $"Failed to create font texture: {SDL.GetError()}");
            return false;
        }

        // Update texture directly without converting pixel format
        if (!SDL.UpdateTexture(_fontTexture, IntPtr.Zero, (IntPtr)pixels, width * 4))
        {
            SDL.LogError(SDL.LogCategory.Application, $"Failed to update font texture: {SDL.GetError()}");
            return false;
        }

        // Ensure proper blending for font rendering
        SDL.SetTextureBlendMode(_fontTexture, SDL.BlendMode.Blend);

        // Use nearest neighbor filtering for crisp font rendering at small sizes
        SDL.SetTextureScaleMode(_fontTexture, SDL.ScaleMode.Linear);

        // Store our identifier
        io.Fonts.SetTexID(_fontTexture);

        SDL.DestroySurface(surface);
        io.Fonts.ClearTexData();

        return true;
    }

    private void DestroyFontsTexture()
    {
        var io = ImGui.GetIO();
        if (_fontTexture == IntPtr.Zero)
        {
            return;
        }

        io.Fonts.SetTexID(IntPtr.Zero);
        SDL.DestroyTexture(_fontTexture);
        _fontTexture = IntPtr.Zero;
    }

    private struct BackupSdlRendererState
    {
        public SDL.Rect Viewport;
        public bool ViewportEnabled;
        public bool ClipEnabled;
        public SDL.Rect ClipRect;
    }
}