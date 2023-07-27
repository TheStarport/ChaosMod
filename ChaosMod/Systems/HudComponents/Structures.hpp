#pragma once
#pragma warning(push)
#pragma warning(disable : 4554)

typedef unsigned char byte;

class TControl
{
    public:
        virtual void u00();
        virtual void u04();
        virtual void IO_streaminit();
        virtual void u0C();
        virtual void u10();
        virtual void u14();
        virtual void u18();
        virtual void u1C();
        virtual void u20();
        virtual void u24();
        virtual void u28();
        virtual void u2C();
        virtual void u30();
        virtual void Draw();
        virtual void u38();
        virtual void u3C();
        virtual void u40();
        virtual void u44();
        virtual void u48();
        virtual void u4C();
        virtual void u50();
        virtual void u54();
        virtual void u58();
        virtual void u5C();
        virtual void u60();
        virtual void u64();
        virtual void u68();
        virtual void u6C();
        virtual void u70();
        virtual void u74();
        virtual void u78();
        virtual void u7C();
        virtual void u80();
        virtual void u84();
        virtual void u88();
        virtual void u8C();
        virtual void u90();
        virtual void u94();
        virtual void u98();
        virtual void u9C();
        virtual void uA0();
        virtual void uA4();
        virtual int Perform(int Message, uint LPARAM, uint WPARAM);
        // byte dunno[0xAC];

    public:
        [[nodiscard]]
        std::string GetName() const
        {
            try
            {
                const uint data = reinterpret_cast<uint>(this) + 0x18;
                return std::string((char*)data);
            }
            catch (...)
            {
                return "";
            }
        }

        [[nodiscard]]
        bool GetEnabled() const
        {
            const uint data = reinterpret_cast<uint>(this) + 0x6C;
            const byte value = *reinterpret_cast<byte*>(data);
            return (value & 0x08) == 0x08;
        }

        void SetEnabled(bool Value) const
        {
            const uint data = reinterpret_cast<uint>(this) + 0x6C;
            byte value = *reinterpret_cast<byte*>(data);
            const byte part1 = value & 0x07;

            byte part2;
            if (Value)
            {
                part2 = 0x08;
            }
            else
            {
                part2 = 0x00;
            }
            value = part1 | part2;
            *reinterpret_cast<byte*>(data) = value;
        }

        [[nodiscard]]
        bool GetVisible() const
        {
            const uint data = reinterpret_cast<uint>(this) + 0x6C;
            const byte value = *reinterpret_cast<byte*>(data);
            // ReSharper disable once CppIdenticalOperandsInBinaryExpression
            return (value & 0x03) == 0x03;
        }

        void SetVisible(const bool newValue) const
        {
            const uint data = reinterpret_cast<uint>(this) + 0x6C;
            byte currentValue = *reinterpret_cast<byte*>(data);
            const byte part1 = currentValue & 0xFD;

            byte part2;
            if (newValue != 0)
            {
                part2 = 0x03;
            }
            else
            {
                part2 = 0x00;
            }
            currentValue = part1 | part2;
            *reinterpret_cast<byte*>(data) = currentValue;
        }

        [[nodiscard]]
        Vector GetPosition() const
        {
            const uint data = reinterpret_cast<uint>(this) + 0xA8;
            return *reinterpret_cast<Vector*>(data);
        }

        void SetPosition(const Vector p) const
        {
            const uint data = reinterpret_cast<uint>(this) + 0xA8;
            *reinterpret_cast<Vector*>(data) = p;
        }

        [[nodiscard]]
        TControl* GetParentControl() const
        {
            return reinterpret_cast<TControl*>(*reinterpret_cast<PDWORD>(reinterpret_cast<DWORD>(this) + 0x04));
        }

        [[nodiscard]]
        TControl* GetNextControl() const
        {
            return reinterpret_cast<TControl*>(*reinterpret_cast<PDWORD>(reinterpret_cast<DWORD>(this) + 0x08));
        }

        [[nodiscard]]
        TControl* GetChildControl() const
        {
            return reinterpret_cast<TControl*>(*reinterpret_cast<PDWORD>(reinterpret_cast<DWORD>(this) + 0x0C));
        }

        bool ControlExists(TControl* control)
        {
            const auto address = reinterpret_cast<DWORD>(control);
            if (reinterpret_cast<DWORD>(this) == address)
            {
                return true;
            }

            TControl* child = this->GetChildControl();
            while (child)
            {
                if (reinterpret_cast<DWORD>(child) == address)
                {
                    return child;
                }

                TControl* newChild = child->GetChildControl();
                while (newChild)
                {
                    if (reinterpret_cast<DWORD>(newChild) == address)
                    {
                        return newChild;
                    }

                    newChild = newChild->GetNextControl();
                }

                child = child->GetNextControl();
            }

            TControl* next;
            while ((next = this->GetNextControl()))
            {
                if (reinterpret_cast<DWORD>(next) == address)
                {
                    return next;
                }

                TControl* newChild = child->GetChildControl();
                while (newChild)
                {
                    if (reinterpret_cast<DWORD>(newChild) == address)
                    {
                        return newChild;
                    }

                    newChild = newChild->GetNextControl();
                }
            }

            // Not found
            return false;
        }

        TControl* FindControlByName(const std::string& name)
        {
            if (this->GetName() == name)
            {
                return this;
            }

            TControl* child = FindChildControl(name);
            if (child)
            {
                return child;
            }

            TControl* next;
            while ((next = this->GetNextControl()))
            {
                if (next->GetName() == name)
                {
                    return next;
                }

                child = FindChildControl(name);
                if (child)
                {
                    return child;
                }
            }

            // Not found
            return nullptr;
        }

        [[nodiscard]]
        TControl* FindChildControl(const std::string& name) const
        {
            TControl* child = this->GetChildControl();
            while (child)
            {
                if (child->GetName() == name)
                {
                    return child;
                }

                if (child->GetChildControl() != nullptr)
                {
                    TControl* newChild = child->FindChildControl(name);
                    if (newChild != nullptr)
                    {
                        return newChild;
                    }
                }

                child = child->GetNextControl();
            }

            // Not found
            return nullptr;
        }

        void SetParentControl(TControl* control)
        {
            const uint data = reinterpret_cast<uint>(this) + 0x04;
            *reinterpret_cast<TControl**>(data) = control;
        }

        void SetNextControl(TControl* control)
        {
            const uint data = reinterpret_cast<uint>(this) + 0x08;
            *reinterpret_cast<TControl**>(data) = control;
        }

        void SetChildControl(TControl* control)
        {
            const uint data = reinterpret_cast<uint>(this) + 0x0C;
            *reinterpret_cast<TControl**>(data) = control;
        }
};

struct Color
{
        byte R;
        byte G;
        byte B;
        byte A;
};

inline Color CreateColor(byte r, byte g, byte b, byte a)
{
    Color color; // NOLINT
    color.R = r;
    color.G = g;
    color.B = b;
    color.A = a;
    return color;
}

enum Location
{
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
};

inline Vector CreateVector(float x, float y, float z)
{
    Vector vec; // NOLINT
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}
