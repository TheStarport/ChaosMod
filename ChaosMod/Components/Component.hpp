#pragma once

class Component;

inline std::set<Component*> Components;

template <class T>
    requires std::is_base_of_v<Component, T>
struct ComponentHolder
{
        class Ptr
        {
                struct Deleter
                {
                        void operator()(T* ptr) { delete ptr; }
                };
                std::unique_ptr<T, Deleter> ptr;

            public:
                void operator=(auto&& args)
                {
                    std::apply(
                        [&](auto&&... args)
                        {
                            ptr.reset();
                            ptr = std::unique_ptr<T, Deleter>(new T(args...));
                        },
                        args);
                }

                T* operator()() const { return ptr.get(); }
        };

        static inline Ptr Instance;
};

template <class T>
    requires std::is_base_of_v<Component, T>
inline T* GetComponent()
{
    return ComponentHolder<T>::Instance();
}

template <class T>
    requires std::is_base_of_v<Component, T>
inline bool ComponentExists()
{
    return ComponentHolder<T>::Instance();
}

template <class T>
    requires std::is_base_of_v<Component, T>
inline void InitComponent(auto&&... args)
{
    // For whatever reason the compiler prepends an additional template param to Args, breaking std::forward
    // We're just going to perfect forward using a tuple instead
    ComponentHolder<T>::Instance = std::forward_as_tuple(args...);
}

class Component
{
    protected:
        Component() { Components.insert(this); }

        virtual ~Component() { Components.erase(this); }

    public:
        Component(const Component&) = delete;

        Component& operator=(const Component&) = delete;

        virtual void OnModPauseCleanup() {}

        virtual void OnRun() {}

        virtual void OnKeyInput(DWORD key, bool repeated, bool isUpNow) {}

        template <class T>
            requires std::is_base_of_v<Component, T>
        friend struct ComponentHolder;
};
