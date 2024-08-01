#ifndef __OPTIONAL__
#define __OPTIONAL__

#include <exception>
#include <initializer_list>
#include <compare>
#include <type_traits>
#include <utility>
#include <memory>
#include <new>

namespace opt
{
  template <typename T>
  class optional
  {
  private:
    struct nullopt_t
    {
    public:
      explicit constexpr nullopt_t(int) noexcept {}
    };

  public:
    class bad_optional_access;

  public:
    using value_type = T;

  public:
    inline static constexpr nullopt_t nullopt{0};

    // ------- default constructor ------------ //
    constexpr optional() noexcept = default;

    // ------ parametr constructor ------------ //
    constexpr optional(nullopt_t nil) noexcept
        : val{nil}
    {
    }

    // ----- copy constructor ------------ //

    constexpr optional(const optional &other) noexcept

    {
      if (other.has_value())
      {
        new (&val) T(other.val);
        this_value = true;
      }
    }

    // ------ move constructor ---------- //
    constexpr optional(optional &&other) noexcept
    {
      if (other.has_value())
      {
        new (&val) T(std::move(other.val));
        this_value = true;
        other.this_value = true;
      }
    }

    // ----- copy assignment ----------- //
    constexpr optional &operator=(const optional &other) noexcept
    {
      if (this != std::addressof(other))
      {
        new (&val) T(other.val);
        this_value = true;
        other.this_value = true;
      }
      return *this;
    }

    template <class U>
    constexpr optional &operator=(const optional<U> &other) noexcept
    {
      if (this != std::addressof(other))
      {
        new (&val) T(std::move(other.val));
        this_value = true;
        other.this_value = true;
      }
      return *this;
    }

    // ----- move assignment ----------- //

    constexpr optional &operator=(optional &&other) noexcept
    {
      if (this != std::addressof(other))
      {
        val = std::move(other.val);
        this_value = true;
        other.this_value = true;
      }
      return *this;
    }

    template <class U>
    constexpr optional &operator=(optional<U> &&other)
    {
      if (this != std::addressof(other))
      {
        val = std::move(other.val);
        this_value = true;
        other.this_value = true;
      }
      return *this;
    }

    template <class U = T>
    constexpr optional &operator=(U &&other)
    {
      if (this != std::addressof(other))
      {
        new (&val) T(std::move(other));
        this_value = true;
      }
      return *this;
    }

    // ------ assigment -------------- //
    constexpr optional &operator=(nullopt_t nil) noexcept
    {
      val = nil;

      return *this;
    }

    template <class... Args>
    constexpr explicit optional(std::in_place_t, Args &&...args)
    {
      new (&val) T(std::forward<Args>(args)...);
      this_value = true;
    }

    template <class U = T>
    constexpr explicit(!std::is_convertible_v<U &&, T>) optional(U &&other)
    {

      new (&val) T(std::move(other));
      this_value = true;
    }

    template <class U>
    constexpr explicit(!std::is_convertible_v<const U &, T>) optional(const optional<U> &other)
    {
      new (&val) T(other.val);
      this_value = true;
      other.this_value = true;
    }

    template <class U>
    constexpr explicit(!std::is_convertible_v<U &&, T>) optional(optional<U> &&other)
    {
      new (&val) T(other.val);
      this_value = true;
      other.this_value = true;
    }

    // -------------  destructor -------------- //
    constexpr ~optional()
    {
      if (has_value())
      {
        reset();
      }
    }

    template <class... Args>
    constexpr T &emplace(Args &&...args)
    {
      if (this_value)
      {
        reset();
      }
      new (&val) T(std::forward<Args>(args)...);
      this_value = true;

      return val;
    }

    // --------------- swap ----------------- //
    constexpr void swap(optional &other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&
        std::is_nothrow_swappable_v<T>)
    {
      std::swap(val, other.val);
      std::swap(this_value, other.this_value);
    }

    // --------------- observers ------------ //
    constexpr const T *operator->() const noexcept
    {
      return &val;
    }
    constexpr T *operator->() noexcept
    {
      return &val;
    }
    constexpr const T &operator*() const & noexcept
    {
      return this->val;
    }
    constexpr T &operator*() & noexcept
    {
      return this->val;
    }
    constexpr T &&operator*() && noexcept
    {
      return this->val;
    }
    constexpr const T &&operator*() const && noexcept
    {
      return this->val;
    }
    constexpr explicit operator bool() const noexcept
    {
      return this_value;
    }
    constexpr bool has_value() const noexcept
    {
      return this_value;
    }
    constexpr const T &value() const &
    {
      if (!has_value())
      {
        throw std::out_of_range("Optional does not contain a value");
      }
      return val;
    }
    constexpr T &value() &
    {
      if (!has_value())
      {
        throw std::out_of_range("Optional does not contain a value");
      }
      return val;
    }
    constexpr T &&value() &&
    {
      if (!has_value())
      {
        throw std::out_of_range("Optional does not contain a value");
      }
      return val;
    }
    constexpr const T &&value() const &&
    {
      if (!has_value())
      {
        throw std::out_of_range("Optional does not contain a value");
      }
      return val;
    }
    template <class U>
    constexpr T value_or(U &&other) const &
    {
      if (this_value)
      {
        return val;
      }
      return static_cast<T>(std::forward<U>(other));
    }
    template <class U>
    constexpr T value_or(U &&other) &&
    {
      if (this_value)
      {
        return val;
      }
      return static_cast<T>(std::forward<U>(other));
    }

    // ------------- monadic operations ------------- //
    template <class F>
    constexpr auto and_then(F &&f) &
    {
      using ResultType = std::invoke_result_t<F, T &>;
      if (this_value)
      {
        return ResultType(std::forward<F>(f)(val));
      }
      return ResultType{};
    }
    template <class F>
    constexpr auto and_then(F &&f) &&
    {
      using ResultType = std::invoke_result_t<F, T &&>;
      if (this_value)
      {
        return ResultType(std::forward<F>(f)(val));
      }
      return ResultType{};
    }
    template <class F>
    constexpr auto and_then(F &&f) const &
    {
      using ResultType = std::invoke_result_t<F, const T &>;
      if (this_value)
      {
        return ResultType(std::forward<F>(f)(val));
      }
      return ResultType{};
    }
    template <class F>
    constexpr auto and_then(F &&f) const &&
    {
      using ResultType = std::invoke_result_t<F, const T &&>;
      if (this_value)
      {
        return ResultType(std::forward<F>(f)(val));
      }
      return ResultType{};
    }
    template <class F>
    constexpr auto transform(F &&f) &
    {
      using ResultType = std::invoke_result_t<F, T &>;
      if (this_value)
      {
        return ResultType(std::forward<F>(f)(val));
      }
      return optional<ResultType>{};
    }
    template <class F>
    constexpr auto transform(F &&f) &&
    {
      using ResultType = std::invoke_result_t<F, T &&>;
      if (this_value)
      {
        return ResultType(std::forward<F>(f)(val));
      }
      return optional<ResultType>{};
    }
    template <class F>
    constexpr auto transform(F &&f) const &
    {
      using ResultType = std::invoke_result_t<F, const T &>;
      if (this_value)
      {
        return ResultType(std::forward<F>(f)(val));
      }
      return optional<ResultType>{};
    }

    template <class F>
    constexpr auto transform(F &&f) const &&
    {
      using ResultType = std::invoke_result_t<F, const T &&>;
      if (this_value)
      {
        return ResultType(std::forward<F>(f)(val));
      }
      return optional<ResultType>{};
    }

    template <class F>
    constexpr optional or_else(F &&f) &&
    {
      if (this_value)
      {
        return std::move(*this);
      }
      return std::forward<F>(f)(val);
    }

    template <class F>
    constexpr optional or_else(F &&f) const &
    {
      if (this_value)
      {
        return *this;
      }
      return {};
    }

    // modifiers
    constexpr void reset() noexcept
    {
      if (this_value)
      {
        val.~T();
      }
    }

    // ----------- compare optionals  --------------- //

    auto operator<=>(const optional &rhs) const noexcept
    {
      if (has_value() && rhs.has_value())
      {
        return val <=> rhs.val;
      }
      else if (!has_value() && !rhs.has_value())
      {
        return std::strong_ordering::equal;
      }
      else if (has_value())
      {
        return std::strong_ordering::greater;
      }
      else
      {
        return std::strong_ordering::less;
      }
    }

  private:
    T val;

    bool this_value;
  };

  template <typename T>
  class optional<T>::bad_optional_access : protected std::exception
  {
  public:
    bad_optional_access() = default;
    bad_optional_access(const bad_optional_access &) = default;
    bad_optional_access(bad_optional_access &&) = default;

  public:
    virtual const char *what() const noexcept override
    {
      return "Optional does not contain a value";
    }
  };

#endif // end  Optional
}
