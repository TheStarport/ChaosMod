#pragma once

class ChatConsole final : public Component
{
        enum class Node
        {
            NoNode = 0,
            TRANode,
            TextNode,
            UnknownNode, // causes failure if present
            PositionNode,
            ParagraphNode,
            StyleNode
        };

        union VPtr {
                char* c{};
                ushort* s;
                uint* i;
                wchar_t* w;
        };

        inline static std::vector<char> buffer;

        VPtr data{};

    public:
        // The bits in brackets are the defaults, overridden by the definition in
        // DATA\FONTS\rich_fonts.ini.

        enum class Style
        {
            Sender = 0xd5ec,            // white, font = 2
            Console = 0x73f7,           // white (green, font = 2, iu)
            Universe = 0xd681,          // white
            System = 0xd801,            // aqua
            Local = 0x1d9d,             // green
            Private = 0x8283,           // green (i)
            Info = 0xe784,              // white, b
            Notice = 0x3ea8,            // red, b
            Group = 0xba28,             // #FF7EFD (magenta)
            StyleBoldYellow = 0x66b7,   // yellow, b (unused)
            Invite = 0x2b34,            // yellow, iu
            Player = 0x66ac,            // white
            Button = 0x828b,            // aqua, font = 6 (1)
            Title = 0x1df5,             // aqua, font = 3, center
            Subtitle = 0x43e0,          // aqua, font = 3
            Header = 0xd824,            // aqua, font = 1
            HeaderActive = 0xd190,      // yellow, font = 1
            Table = 0x69f5,             // aqua
            TableSelect = 0x3343,       // yellow
            TABLE_INACTIVE = 0xbb80,    // #B6B6B6 (light grey)
            Loctable = 0x5203,          // aqua (font = 1)
            LoctableSelect = 0x3dbc,    // yellow (font = 1)
            LoctableInactive = 0x867f,  // #B6B6B6 (font = 1)
            NN = 0x9ca4,                // aqua
            Data = 0x76ec,              // aqua
            StyleSmallAqua = 0x686c,    // aqua, font = 5 (unused)
            SytleNnBold = 0x0f64,       // aqua, b
            NnSelected = 0x3d0a,        // yellow
            Dialog = 0x5c12,            // aqua, font = 1
            DialogSmall = 0xc9b7,       // aqua, font = 5
            SmallHeader = 0x3df7,       // aqua, font = 5
            SmallHeaderActive = 0xda80, // yellow, font = 5
            Subsubtitle = 0xfaa9,       // aqua, font = 1
            Err = 0x3363,               // red
            NnAgency = 0x5724,          // aqua, font = 5
            ConsoleSender = 0x6c0e,     // #00FF00, font = 2
            Label = 0x4433              // aqua, font = 1
        };

        enum class TraMask : unsigned int
        {
            TRA_Bold = 1,
            TRA_Italic = 2,
            TRA_Underline = 4,
            TRA_Color = 0xFFFFFF00
        };

        void AddNode(const uint node, const uint len)
        {
            if (const uint newSize = buffer.size() + 8u + len; newSize > buffer.capacity())
            {
                // Double the buffer size if needed
                buffer.reserve(buffer.size() * 2);
                data.c = buffer.data() + buffer.size();
            }

            *data.i++ = node;
            *data.i++ = len;
        }

        void Tra(const UINT tra, const UINT mask)
        {
            AddNode(static_cast<int>(Node::TRANode), 8);
            *data.i++ = tra; // TextRenderAttributes
            *data.i++ = mask;
        }

        void Style(const USHORT style)
        {
            AddNode(static_cast<int>(Node::StyleNode), 2);
            *data.s++ = style;
        }

        void Paragraph()
        {
            AddNode(static_cast<int>(Node::ParagraphNode), 2); // ignores it if it's zero; use 2 to keep
            *data.s++ = 0;                                     //  word (wchar_t) alignment
        }

        void Print(const std::wstring& str, const bool shouldFlush = false)
        {
            const uint len = sizeof(wchar_t) * str.size() + 2;

            AddNode(static_cast<int>(Node::TextNode), len);
            memcpy(data.w, str.c_str(), len);
            data.c += len;

            if (shouldFlush)
            {
                using Flush = void (*)(int, char*, unsigned int);
                static auto flush = reinterpret_cast<Flush>(0x46A150);
                flush(0, buffer.data(), buffer.size());
                data.c = buffer.data();
            }
        }

        void Print(const std::string& str, const bool flush = false)
        {
            const auto newStr = StringUtils::stows(str);
            Print(newStr, flush);
        }
};
