#include <kot/heap.h>
#include <kot/types.h>
#include <kot/memory.h>


namespace std{

    template <typename T>
    class unique_ptr
    {
    public:
        unique_ptr() noexcept : ptr_(nullptr) {}

        unique_ptr(unique_ptr&& other) noexcept : ptr_(other.release()) {}

        explicit unique_ptr(T* ptr) noexcept : ptr_(ptr) {}

        ~unique_ptr() { delete ptr_; }

        unique_ptr& operator=(unique_ptr&& other) noexcept
        {
            reset(other.release());
            return *this;
        }

        T* get() const noexcept { return ptr_; }

        T* release() noexcept
        {
            T* ptr = ptr_;
            ptr_ = nullptr;
            return ptr;
        }

        void reset(T* ptr = nullptr) noexcept
        {
            if (ptr_ != ptr)
            {
                delete ptr_;
                ptr_ = ptr;
            }
        }

        explicit operator bool() const noexcept { return ptr_ != nullptr; }

        T& operator*() const { return *ptr_; }

        T* operator->() const noexcept { return ptr_; }

    private:
        T* ptr_;
    };


    template <typename T>
    struct remove_reference
    {
        typedef T type;
    };

    template <typename T>
    struct remove_reference<T&>
    {
        typedef T type;
    };

    template <typename T>
    struct remove_reference<T&&>
    {
        typedef T type;
    };


    template <typename T>
    typename std::remove_reference<T>::type&& move(T&& arg) noexcept
    {
        return static_cast<typename std::remove_reference<T>::type&&>(arg);
    }

    template <typename T>
    T&& forward(typename std::remove_reference<T>::type& arg) noexcept
    {
        return static_cast<T&&>(arg);
    }

    template <typename T>
    T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
    {
        return static_cast<T&&>(arg);
    }


    template <typename T>
    class function;

    template <typename R, typename... Args>
    class function<R(Args...)>
    {
    public:

        function() noexcept : callable(nullptr) {}
        
        template <typename F>
        function(F&& f) : callable(new CallableImpl<F>(std::forward<F>(f))) {}
        
        function(const function& other) : callable(other.callable ? other.callable->clone() : nullptr) {}
        function(function&& other) noexcept : callable(std::move(other.callable)) {}
        
        function& operator=(const function& other)
        {
            function tmp(other);
            swap(tmp);
            return *this;
        }
        
        function& operator=(function&& other) noexcept
        {
            callable = std::move(other.callable);
            return *this;
        }
        
        R operator()(Args... args) const
        {        
            return callable->invoke(std::forward<Args>(args)...);
        }
        
        void swap(function& other) noexcept
        {
            swap(callable, other.callable);
        }
        
        explicit operator bool() const noexcept
        {
            return callable != nullptr;
        }
        
    private:
        class CallableBase
        {
        public:
            virtual ~CallableBase() = default;
            virtual R invoke(Args&&... args) const = 0;
            virtual std::unique_ptr<CallableBase> clone() const = 0;
        };
        
        template <typename F>
        class CallableImpl : public CallableBase
        {
        public:
            CallableImpl(F&& f) : func(std::forward<F>(f)) {}
            
            R invoke(Args&&... args) const override
            {
                return func(std::forward<Args>(args)...);
            }
            
            std::unique_ptr<CallableBase> clone() const override
            {
                return std::unique_ptr<CallableBase>(new CallableImpl<F>(static_cast<F>(func)));
            }
            
        private:
            F func;
        };
        
        std::unique_ptr<CallableBase> callable;
    };

    template <typename R, typename... Args>
    void swap(function<R(Args...)>& f1, function<R(Args...)>& f2) noexcept{
        f1.swap(f2);
    }
}
