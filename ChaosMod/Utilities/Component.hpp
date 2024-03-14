#pragma once

class Component;
template <class T>
    requires std::is_base_of_v<Component, T>
struct ComponentHolder
{
        class Ptr
        {
                std::shared_ptr<T> ptr = nullptr;

            public:
                void operator=(auto &&args)
                {
                    std::apply(
                        [&](auto &&...innerArgs)
                        {
                            ptr.reset();
                            ptr = std::make_shared<T>(innerArgs...);
                        },
                        args);
                }

                std::shared_ptr<T> operator()() const { return ptr; }

                void Reset(std::shared_ptr<T> replacement = nullptr)
                {
                    if (replacement == nullptr)
                    {
                        ptr.reset();
                    }
                    else
                    {
                        ptr = replacement;
                    }
                }
        };
        static inline Ptr instance;
};

template <class T>
    requires std::is_base_of_v<Component, T>
std::shared_ptr<T> Get()
{
    return ComponentHolder<T>::instance();
}

template <class T>
    requires std::is_base_of_v<Component, T>
void SetComponent(auto &&...args)
{
    ComponentHolder<T>::instance = std::forward_as_tuple(args...);
}

template <class T>
    requires std::is_base_of_v<Component, T>
void ResetComponent(std::shared_ptr<T> component = nullptr)
{
    ComponentHolder<T>::instance.Reset(component);
}

class ChaosMod;
class Component
{
        friend ChaosMod;
        inline static std::set<Component *> components;

    protected:
        Component() { components.insert(this); }
        virtual ~Component() { components.erase(this); }

        // TODO: Hook up these virtual functions
        virtual void OnBaseEnter() {}
        virtual void OnBaseExit() {}

    public:
        Component(const Component &) = delete;
        Component &operator=(const Component &) = delete;

        template <class T>
            requires std::is_base_of_v<Component, T>
        friend struct ComponentHolder;
};
