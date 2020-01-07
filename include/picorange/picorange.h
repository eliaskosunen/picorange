// Copyright 2017-2019 Elias Kosunen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file is a part of picorange:
//     https://github.com/eliaskosunen/picorange
//
// The contents of this file are based on NanoRange
//     https://github.com/tcbrindle/NanoRange
//     Copyright (c) 2018 Tristan Brindle
//     Distributed under the Boost Software License, Version 1.0

#ifndef PICORANGE_H
#define PICORANGE_H

#include <cassert>
#include <iterator>
#include <tuple>

#define PICORANGE_STD_11 201103L
#define PICORANGE_STD_14 201402L
#define PICORANGE_STD_17 201703L

#define PICORANGE_COMPILER(major, minor, patch) \
    ((major)*10000000 /* 10,000,000 */ + (minor)*10000 /* 10,000 */ + (patch))
#define PICORANGE_VERSION PICORANGE_COMPILER(0, 2, 0)

#ifdef __INTEL_COMPILER
// Intel
#define PICORANGE_INTEL                                                      \
    PICORANGE_COMPILER(__INTEL_COMPILER / 100, (__INTEL_COMPILER / 10) % 10, \
                       __INTEL_COMPILER % 10)
#elif defined(_MSC_VER) && defined(_MSC_FULL_VER)
// MSVC
#if _MSC_VER == _MSC_FULL_VER / 10000
#define PICORANGE_MSVC \
    PICORANGE_COMPILER(_MSC_VER / 100, _MSC_VER % 100, _MSC_FULL_VER % 10000)
#else
#define PICORANGE_MSVC                                                \
    PICORANGE_COMPILER(_MSC_VER / 100, (_MSC_FULL_VER / 100000) % 10, \
                       _MSC_FULL_VER % 100000)
#endif  // _MSC_VER == _MSC_FULL_VER / 10000
#elif defined(__clang__) && defined(__clang_minor__) && \
    defined(__clang_patchlevel__)
// Clang
#define PICORANGE_CLANG \
    PICORANGE_COMPILER(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) && \
    defined(__GNUC_PATCHLEVEL__)
// GCC
#define PICORANGE_GCC \
    PICORANGE_COMPILER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#endif

#ifndef PICORANGE_INTEL
#define PICORANGE_INTEL 0
#endif
#ifndef PICORANGE_MSVC
#define PICORANGE_MSVC 0
#endif
#ifndef PICORANGE_CLANG
#define PICORANGE_CLANG 0
#endif
#ifndef PICORANGE_GCC
#define PICORANGE_GCC 0
#endif

// Pretending to be gcc (clang, icc, etc.)
#ifdef __GNUC__

#ifdef __GNUC_MINOR__
#define PICORANGE_GCC_COMPAT_MINOR __GNUC_MINOR__
#else
#define PICORANGE_GCC_COMPAT_MINOR 0
#endif

#ifdef __GNUC_PATCHLEVEL__
#define PICORANGE_GCC_COMPAT_PATCHLEVEL __GNUC_PATCHLEVEL__
#else
#define PICORANGE_GCC_COMPAT_PATCHLEVEL 0
#endif

#define PICORANGE_GCC_COMPAT                                 \
    PICORANGE_COMPILER(__GNUC__, PICORANGE_GCC_COMPAT_MINOR, \
                       PICORANGE_GCC_COMPAT_PATCHLEVEL)
#else
#define PICORANGE_GCC_COMPAT 0
#endif  // #ifdef __GNUC__

#define PICORANGE_STRINGIFY_APPLY(x) #x
#define PICORANGE_STRINGIFY(x) PICORANGE_STRINGIFY_APPLY(x)

// POSIX
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define PICORANGE_POSIX 1
#else
#define PICORANGE_POSIX 0
#endif

// Windows
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32)) && \
    !defined(__CYGWIN__)
#define PICORANGE_WINDOWS 1
#else
#define PICORANGE_WINDOWS 0
#endif

#ifdef _MSVC_LANG
#define PICORANGE_MSVC_LANG _MSVC_LANG
#else
#define PICORANGE_MSVC_LANG 0
#endif

// Standard version
#if PICORANGE_MSVC
#define PICORANGE_STD PICORANGE_MSVC_LANG
#else
#define PICORANGE_STD __cplusplus
#endif

// Warning control
#if PICORANGE_GCC
#define PICORANGE_PRAGMA_APPLY(x) _Pragma(#x)

#define PICORANGE_GCC_PUSH _Pragma("GCC diagnostic push")
#define PICORANGE_GCC_POP _Pragma("GCC diagnostic pop")

#define PICORANGE_GCC_IGNORE(x) PICORANGE_PRAGMA_APPLY(GCC diagnostic ignored x)
#else
#define PICORANGE_GCC_PUSH
#define PICORANGE_GCC_POP
#define PICORANGE_GCC_IGNORE(x)
#endif

#if PICORANGE_CLANG
#define PICORANGE_PRAGMA_APPLY(x) _Pragma(#x)

#define PICORANGE_CLANG_PUSH _Pragma("clang diagnostic push")
#define PICORANGE_CLANG_POP _Pragma("clang diagnostic pop")

#define PICORANGE_CLANG_IGNORE(x) \
    PICORANGE_PRAGMA_APPLY(clang diagnostic ignored x)

#if PICORANGE_CLANG >= PICORANGE_COMPILER(3, 9, 0)
#define PICORANGE_CLANG_PUSH_IGNORE_UNDEFINED_TEMPLATE \
    PICORANGE_CLANG_PUSH PICORANGE_CLANG_IGNORE("-Wundefined-func-template")
#define PICORANGE_CLANG_POP_IGNORE_UNDEFINED_TEMPLATE PICORANGE_CLANG_POP
#else
#define PICORANGE_CLANG_PUSH_IGNORE_UNDEFINED_TEMPLATE
#define PICORANGE_CLANG_POP_IGNORE_UNDEFINED_TEMPLATE
#endif

#else
#define PICORANGE_CLANG_PUSH
#define PICORANGE_CLANG_POP
#define PICORANGE_CLANG_IGNORE(x)
#define PICORANGE_CLANG_PUSH_IGNORE_UNDEFINED_TEMPLATE
#define PICORANGE_CLANG_POP_IGNORE_UNDEFINED_TEMPLATE
#endif

#if PICORANGE_MSVC
#define PICORANGE_MSVC_PUSH __pragma(warning(push))
#define PICORANGE_MSVC_POP __pragma(warning(pop))

#define PICORANGE_MSVC_IGNORE(x) __pragma(warning(disable : x))
#else
#define PICORANGE_MSVC_PUSH
#define PICORANGE_MSVC_POP
#define PICORANGE_MSVC_IGNORE(x)
#endif

#ifndef PICORANGE_PREDEFINE_VSCAN_OVERLOADS
#define PICORANGE_PREDEFINE_VSCAN_OVERLOADS 0
#endif

#ifdef __cpp_exceptions
#define PICORANGE_HAS_EXCEPTIONS 1
#endif
#if !defined(PICORANGE_HAS_EXCEPTIONS) && defined(__EXCEPTIONS)
#define PICORANGE_HAS_EXCEPTIONS 1
#endif
#if !defined(PICORANGE_HAS_EXCEPTIONS) && defined(_HAS_EXCEPTIONS)
#define PICORANGE_HAS_EXCEPTIONS 1
#endif
#if !defined(PICORANGE_HAS_EXCEPTIONS) && !defined(_CPPUNWIND)
#define PICORANGE_HAS_EXCEPTIONS 0
#endif
#ifndef PICORANGE_HAS_EXCEPTIONS
#define PICORANGE_HAS_EXCEPTIONS 0
#endif

#ifdef __has_include
#define PICORANGE_HAS_INCLUDE(x) __has_include(x)
#else
#define PICORANGE_HAS_INCLUDE(x) 0
#endif

#ifdef __has_cpp_attribute
#define PICORANGE_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define PICORANGE_HAS_CPP_ATTRIBUTE(x) 0
#endif

#ifdef __has_feature
#define PICORANGE_HAS_FEATURE(x) __has_feature(x)
#else
#define PICORANGE_HAS_FEATURE(x) 0
#endif

#ifdef __has_builtin
#define PICORANGE_HAS_BUILTIN(x) __has_builtin(x)
#else
#define PICORANGE_HAS_BUILTIN(x) 0
#endif

// Detect constexpr
#if defined(__cpp_constexpr)
#if __cpp_constexpr >= 201304
#define PICORANGE_HAS_RELAXED_CONSTEXPR 1
#else
#define PICORANGE_HAS_RELAXED_CONSTEXPR 0
#endif
#endif

#ifndef PICORANGE_HAS_RELAXED_CONSTEXPR
#if PICORANGE_HAS_FEATURE(cxx_relaxed_constexpr) ||       \
    PICORANGE_MSVC >= PICORANGE_COMPILER(19, 10, 0) ||    \
    ((PICORANGE_GCC >= PICORANGE_COMPILER(6, 0, 0) ||     \
      PICORANGE_INTEL >= PICORANGE_COMPILER(17, 0, 0)) && \
     PICORANGE_STD >= PICORANGE_STD_14)
#define PICORANGE_HAS_RELAXED_CONSTEXPR 1
#else
#define PICORANGE_HAS_RELAXED_CONSTEXPR 0
#endif
#endif

#if PICORANGE_HAS_RELAXED_CONSTEXPR
#define PICORANGE_CONSTEXPR14 constexpr
#else
#define PICORANGE_CONSTEXPR14 inline
#endif

// Detect [[nodiscard]]
#if (PICORANGE_HAS_CPP_ATTRIBUTE(nodiscard) &&            \
     __cplusplus >= PICORANGE_STD_17) ||                  \
    (PICORANGE_MSVC >= PICORANGE_COMPILER(19, 11, 0) &&   \
     PICORANGE_MSVC_LANG >= PICORANGE_STD_17) ||          \
    ((PICORANGE_GCC >= PICORANGE_COMPILER(7, 0, 0) ||     \
      PICORANGE_INTEL >= PICORANGE_COMPILER(18, 0, 0)) && \
     __cplusplus >= PICORANGE_STD_17)
#define PICORANGE_NODISCARD [[nodiscard]]
#else
#define PICORANGE_NODISCARD /*nodiscard*/
#endif

// Detect [[clang::trivial_abi]]
#if PICORANGE_HAS_CPP_ATTRIBUTE(clang::trivial_abi)
#define PICORANGE_TRIVIAL_ABI [[clang::trivial_abi]]
#else
#define PICORANGE_TRIVIAL_ABI /*trivial_abi*/
#endif

#if defined(PICORANGE_HEADER_ONLY) && PICORANGE_HEADER_ONLY
#define PICORANGE_FUNC inline
#else
#define PICORANGE_FUNC
#endif

// Detect __assume
#if PICORANGE_INTEL || PICORANGE_MSVC
#define PICORANGE_HAS_ASSUME 1
#else
#define PICORANGE_HAS_ASSUME 0
#endif

// Detect __builtin_assume
#if PICORANGE_HAS_BUILTIN(__builtin_assume)
#define PICORANGE_HAS_BUILTIN_ASSUME 1
#else
#define PICORANGE_HAS_BUILTIN_ASSUME 0
#endif

// Detect __builtin_unreachable
#if PICORANGE_HAS_BUILTIN(__builtin_unreachable) || PICORANGE_GCC_COMPAT
#define PICORANGE_HAS_BUILTIN_UNREACHABLE 1
#else
#define PICORANGE_HAS_BUILTIN_UNREACHABLE 0
#endif

#if PICORANGE_HAS_ASSUME
#define PICORANGE_ASSUME(x) __assume(x)
#elif PICORANGE_HAS_BUILTIN_ASSUME
#define PICORANGE_ASSUME(x) __builtin_assume(x)
#elif PICORANGE_HAS_BUILTIN_UNREACHABLE
#define PICORANGE_ASSUME(x) \
    ((x) ? static_cast<void>(0) : __builtin_unreachable())
#else
#define PICORANGE_ASSUME(x) static_cast<void>((x) ? 0 : 0)
#endif

#if PICORANGE_HAS_BUILTIN_UNREACHABLE
#define PICORANGE_UNREACHABLE __builtin_unreachable()
#else
#define PICORANGE_UNREACHABLE PICORANGE_ASSUME(0)
#endif

// Detect __builtin_expect
#if PICORANGE_HAS_BUILTIN(__builtin_expect) || PICORANGE_GCC_COMPAT
#define PICORANGE_HAS_BUILTIN_EXPECT 1
#else
#define PICORANGE_HAS_BUILTIN_EXPECT 0
#endif

#if PICORANGE_HAS_BUILTIN_EXPECT
#define PICORANGE_LIKELY(x) __builtin_expect(!!(x), 1)
#define PICORANGE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define PICORANGE_LIKELY(x) (x)
#define PICORANGE_UNLIKELY(x) (x)
#endif

#ifndef PICORANGE_DEPRECATED

#if (PICORANGE_HAS_CPP_ATTRIBUTE(deprecated) && PICORANGE_STD >= 201402L) || \
    PICORANGE_MSVC >= PICORANGE_COMPILER(19, 0, 0)
#define PICORANGE_DEPRECATED [[deprecated]]
#else

#if PICORANGE_GCC_COMPAT
#define PICORANGE_DEPRECATED __attribute__((deprecated))
#elif PICORANGE_MSVC
#define PICORANGE_DEPRECATED __declspec(deprecated)
#else
#define PICORANGE_DEPRECATED /* deprecated */
#endif

#endif

#endif  // !defined(PICORANGE_DEPRECATED)

#define PICORANGE_UNUSED(x) static_cast<void>(sizeof(x))

#if PICORANGE_HAS_RELAXED_CONSTEXPR
#define PICORANGE_ASSERT(cond, msg)                \
    do {                                           \
        static_cast<void>(PICORANGE_LIKELY(cond)); \
        assert((cond) && msg);                     \
    } while (false)
#define PICORANGE_EXPECT(cond) PICORANGE_ASSERT(cond, "Precondition violation")
#define PICORANGE_ENSURE(cond) PICORANGE_ASSERT(cond, "Postcondition violation")
#else
#define PICORANGE_ASSERT(cond, msg)
#define PICORANGE_EXPECT(cond)
#define PICORANGE_ENSURE(cond)
#endif

#define PICORANGE_BEGIN_NAMESPACE inline namespace v0 {
#define PICORANGE_END_NAMESPACE }

// end config

namespace picorange {
    PICORANGE_BEGIN_NAMESPACE

    template <typename... Ts>
    struct make_void {
        using type = void;
    };
    template <typename... Ts>
    using void_t = typename make_void<Ts...>::type;

    template <typename... T>
    void valid_expr(T&&...);

    template <typename T>
    struct remove_cvref {
        using type = typename std::remove_cv<
            typename std::remove_reference<T>::type>::type;
    };
    template <typename T>
    using remove_cvref_t = typename remove_cvref<T>::type;

    // Stolen from range-v3
    template <typename T>
    struct static_const {
        static constexpr T value{};
    };
    template <typename T>
    constexpr T static_const<T>::value;

    template <size_t I>
    struct priority_tag : priority_tag<I - 1> {
    };
    template <>
    struct priority_tag<0> {
    };

    template <typename T>
    struct dependent_false : std::false_type {
    };

    struct nonesuch {
        nonesuch() = delete;
        nonesuch(nonesuch const&) = delete;
        nonesuch& operator=(const nonesuch&) = delete;
        ~nonesuch() = delete;
    };

    namespace detail {
        template <typename Void,
                  template <class...>
                  class Trait,
                  typename... Args>
        struct test {
            using type = nonesuch;
        };

        template <template <class...> class Trait, typename... Args>
        struct test<void_t<Trait<Args...>>, Trait, Args...> {
            using type = Trait<Args...>;
        };

        template <template <class...> class Trait, typename... Args>
        using test_t = typename test<void, Trait, Args...>::type;

        template <typename Void,
                  template <class...>
                  class AliasT,
                  typename... Args>
        struct exists_helper : std::false_type {
        };

        template <template <class...> class AliasT, typename... Args>
        struct exists_helper<void_t<AliasT<Args...>>, AliasT, Args...>
            : std::true_type {
        };
    }  // namespace detail

    template <template <class...> class AliasT, typename... Args>
    struct exists : detail::exists_helper<void, AliasT, Args...> {
    };

    template <typename R,
              typename... Args,
              typename = decltype(&R::template _test_requires<Args...>)>
    auto test_requires(R&) -> void;

    template <typename R, typename... Args>
    using test_requires_t =
        decltype(test_requires<R, Args...>(std::declval<R&>()));

    template <typename R, typename... Args>
    struct _requires : exists<test_requires_t, R, Args...> {
    };

    template <bool Expr>
    using requires_expr = typename std::enable_if<Expr, int>::type;

    template <typename...>
    struct get_common_type;

    namespace detail {
        template <typename T, typename U>
        struct copy_cv {
            using type = U;
        };
        template <typename T, typename U>
        struct copy_cv<const T, U> {
            using type = typename std::add_const<U>::type;
        };
        template <typename T, typename U>
        struct copy_cv<volatile T, U> {
            using type = typename std::add_volatile<U>::type;
        };
        template <typename T, typename U>
        struct copy_cv<const volatile T, U> {
            using type = typename std::add_cv<U>::type;
        };
        template <typename T, typename U>
        using copy_cv_t = typename copy_cv<T, U>::type;

        template <typename T>
        using cref_t = typename std::add_lvalue_reference<
            const typename std::remove_reference<T>::type>::type;

        template <typename T>
        struct rref_res {
            using type = T;
        };
        template <typename T>
        struct rref_res<T&> {
            using type = typename std::remove_reference<T>::type&&;
        };
        template <typename T>
        using rref_res_t = typename rref_res<T>::type;

        template <typename T, typename U>
        using cond_res_t =
            decltype(std::declval<bool>() ? std::declval<T (&)()>()()
                                          : std::declval<U (&)()>()());

        template <typename T, typename U>
        struct simple_common_reference {
        };

        template <typename T,
                  typename U,
                  typename C = detail::test_t<detail::cond_res_t,
                                              detail::copy_cv_t<T, U>&,
                                              detail::copy_cv_t<U, T>&>>
        struct lvalue_simple_common_reference
            : std::enable_if<std::is_reference<C>::value, C> {
        };
        template <typename T, typename U>
        using lvalue_scr_t =
            typename lvalue_simple_common_reference<T, U>::type;
        template <typename T, typename U>
        struct simple_common_reference<T&, U&>
            : lvalue_simple_common_reference<T, U> {
        };

        template <typename T,
                  typename U,
                  typename LCR = detail::test_t<lvalue_scr_t, T, U>,
                  typename C = detail::rref_res_t<LCR>>
        struct rvalue_simple_common_reference
            : std::enable_if<std::is_convertible<T&&, C>::value &&
                             std::is_convertible<U&&, C>::value>::type {
        };
        template <typename T, typename U>
        struct simple_common_reference<T&&, U&&>
            : rvalue_simple_common_reference<T, U> {
        };

        template <typename A,
                  typename B,
                  typename C = detail::test_t<lvalue_scr_t, A, const B>>
        struct mixed_simple_common_reference
            : std::enable_if<std::is_convertible<B&&, C>::value, C>::type {
        };

        template <typename A, typename B>
        struct simple_common_reference<A&, B&&>
            : mixed_simple_common_reference<A, B> {
        };
        template <typename A, typename B>
        struct simple_common_reference<A&&, B&>
            : simple_common_reference<B&&, A&> {
        };
        template <typename T, typename U>
        using simple_common_reference_t =
            typename simple_common_reference<T, U>::type;

        template <typename>
        struct xref {
            template <typename U>
            using type = U;
        };

        template <typename A>
        struct xref<A&> {
            template <typename U>
            using type = typename std::add_lvalue_reference<
                typename xref<A>::template type<U>>::type;
        };

        template <typename A>
        struct xref<A&&> {
            template <typename U>
            using type = typename std::add_rvalue_reference<
                typename xref<A>::template type<U>>::type;
        };

        template <typename A>
        struct xref<const A> {
            template <typename U>
            using type = typename std::add_const<
                typename xref<A>::template type<U>>::type;
        };

        template <typename A>
        struct xref<volatile A> {
            template <typename U>
            using type = typename std::add_volatile<
                typename xref<A>::template type<U>>::type;
        };

        template <typename A>
        struct xref<const volatile A> {
            template <typename U>
            using type =
                typename std::add_cv<typename xref<A>::template type<U>>::type;
        };

        template <typename T,
                  typename U,
                  template <class>
                  class TQual,
                  template <class>
                  class UQual>
        struct basic_common_reference {
        };
    }  // namespace detail

    template <typename...>
    struct get_common_reference;
    template <typename... Ts>
    using get_common_reference_t = typename get_common_reference<Ts...>::type;

    template <>
    struct get_common_reference<> {
    };
    template <typename T0>
    struct get_common_reference<T0> {
        using type = T0;
    };

    namespace detail {
        template <typename T, typename U>
        struct has_simple_common_ref : exists<simple_common_reference_t, T, U> {
        };
        template <typename T, typename U>
        using basic_common_ref_t =
            typename basic_common_reference<remove_cvref_t<T>,
                                            remove_cvref_t<U>,
                                            xref<T>::template type,
                                            xref<U>::template type>::type;

        template <typename T, typename U>
        struct has_basic_common_ref : exists<basic_common_ref_t, T, U> {
        };
        template <typename T, typename U>
        struct has_cond_res : exists<detail::cond_res_t, T, U> {
        };

        template <typename T, typename U, typename = void>
        struct binary_common_ref : get_common_type<T, U> {
        };
        template <typename T, typename U>
        struct binary_common_ref<
            T,
            U,
            typename std::enable_if<has_simple_common_ref<T, U>::value>::type>
            : simple_common_reference<T, U> {
        };
        template <typename T, typename U>
        struct binary_common_ref<
            T,
            U,
            typename std::enable_if<
                has_basic_common_ref<T, U>::value &&
                !has_simple_common_ref<T, U>::value>::type> {
            using type = basic_common_ref_t<T, U>;
        };
        template <typename T, typename U>
        struct binary_common_ref<
            T,
            U,
            typename std::enable_if<
                has_cond_res<T, U>::value &&
                !has_basic_common_ref<T, U>::value &&
                !has_simple_common_ref<T, U>::value>::type> {
            using type = detail::cond_res_t<T, U>;
        };
    }  // namespace detail
    template <typename T1, typename T2>
    struct get_common_reference<T1, T2> : detail::binary_common_ref<T1, T2> {
    };

    namespace detail {
        template <typename Void, typename T1, typename T2, typename... Rest>
        struct multiple_common_reference {
        };
        template <typename T1, typename T2, typename... Rest>
        struct multiple_common_reference<void_t<get_common_reference_t<T1, T2>>,
                                         T1,
                                         T2,
                                         Rest...>
            : get_common_reference<get_common_reference_t<T1, T2>, Rest...> {
        };
    }  // namespace detail
    template <typename T1, typename T2, typename... Rest>
    struct get_common_reference<T1, T2, Rest...>
        : detail::multiple_common_reference<void, T1, T2, Rest...> {
    };

    template <typename... Ts>
    using get_common_type_t = typename get_common_type<Ts...>::type;

    namespace detail {
        template <typename T, typename U>
        struct same_decayed
            : std::integral_constant<
                  bool,
                  std::is_same<T, typename std::decay<T>::type>::value &&
                      std::is_same<U, typename std::decay<U>::type>::value> {
        };
    }  // namespace detail

    template <typename T, typename U>
    using ternary_return_t = typename std::decay<decltype(
        false ? std::declval<T>() : std::declval<U>())>::type;

    template <typename, typename, typename = void>
    struct binary_common_type {
    };

    template <typename T, typename U>
    struct binary_common_type<
        T,
        U,
        typename std::enable_if<!detail::same_decayed<T, U>::value>::type>
        : get_common_type<typename std::decay<T>::type,
                          typename std::decay<U>::type> {
    };

    template <typename T, typename U>
    struct binary_common_type<
        T,
        U,
        typename std::enable_if<detail::same_decayed<T, U>::value &&
                                exists<ternary_return_t, T, U>::value>::type> {
        using type = ternary_return_t<T, U>;
    };

    template <typename T, typename U>
    struct binary_common_type<
        T,
        U,
        typename std::enable_if<detail::same_decayed<T, U>::value &&
                                !exists<ternary_return_t, T, U>::value &&
                                exists<detail::cond_res_t,
                                       detail::cref_t<T>,
                                       detail::cref_t<U>>::value>::type> {
        using type = typename std::decay<
            detail::cond_res_t<detail::cref_t<T>, detail::cref_t<U>>>::type;
    };

    template <>
    struct get_common_type<> {
    };

    template <typename T>
    struct get_common_type<T> : get_common_type<T, T> {
    };

    template <typename T, typename U>
    struct get_common_type<T, U> : binary_common_type<T, U> {
    };

    template <typename Void, typename...>
    struct multiple_common_type {
    };

    template <typename T1, typename T2, typename... R>
    struct multiple_common_type<void_t<get_common_type_t<T1, T2>>, T1, T2, R...>
        : get_common_type<get_common_type_t<T1, T2>, R...> {
    };

    template <typename T1, typename T2, typename... R>
    struct get_common_type<T1, T2, R...>
        : multiple_common_type<void, T1, T2, R...> {
    };

    template <typename T>
    constexpr typename std::decay<T>::type decay_copy(T&& t) noexcept(
        noexcept(static_cast<typename std::decay<T>::type>(std::forward<T>(t))))
    {
        return std::forward<T>(t);
    }

    using std::bidirectional_iterator_tag;
    using std::forward_iterator_tag;
    using std::input_iterator_tag;
    using std::output_iterator_tag;
    using std::random_access_iterator_tag;
    struct contiguous_iterator_tag : random_access_iterator_tag {
    };

    template <typename T>
    struct iterator_category;

    namespace detail {
        template <typename T, typename = void>
        struct iterator_category {
        };
        template <typename T>
        struct iterator_category<T*> : std::enable_if<std::is_object<T>::value,
                                                      contiguous_iterator_tag> {
        };
        template <typename T>
        struct iterator_category<const T> : iterator_category<T> {
        };
        template <typename T>
        struct iterator_category<T, void_t<typename T::iterator_category>> {
            using type = typename T::iterator_category;
        };
    }  // namespace detail

    template <typename T>
    struct iterator_category : detail::iterator_category<T> {
    };
    template <typename T>
    using iterator_category_t = typename iterator_category<T>::type;

    template <typename T>
    using iter_reference_t = decltype(*std::declval<T&>());

    // iter_difference_t
    template <typename>
    struct incrementable_traits;

    namespace detail {
        struct empty {
        };

        template <typename T>
        struct with_difference_type {
            using difference_type = T;
        };
        template <typename, typename = void>
        struct incrementable_traits_helper {
        };

        template <>
        struct incrementable_traits_helper<void*> {
        };
        template <typename T>
        struct incrementable_traits_helper<T*>
            : std::conditional<std::is_object<T>::value,
                               with_difference_type<std::ptrdiff_t>,
                               empty>::type {
        };
        template <typename I>
        struct incrementable_traits_helper<const I>
            : incrementable_traits<typename std::decay<I>::type> {
        };

        template <typename, typename = void>
        struct has_member_difference_type : std::false_type {
        };
        template <typename T>
        struct has_member_difference_type<T,
                                          void_t<typename T::difference_type>>
            : std::true_type {
        };

        template <typename T>
        struct incrementable_traits_helper<
            T,
            typename std::enable_if<
                has_member_difference_type<T>::value>::type> {
            using difference_type = typename T::difference_type;
        };
        template <typename T>
        struct incrementable_traits_helper<
            T,
            typename std::enable_if<
                !std::is_pointer<T>::value &&
                !has_member_difference_type<T>::value &&
                std::is_integral<decltype(std::declval<const T&>() -
                                          std::declval<const T&>())>::value>::
                type>
            : with_difference_type<typename std::make_signed<decltype(
                  std::declval<T>() - std::declval<T>())>::type> {
        };
    }  // namespace detail
    template <typename T>
    struct incrementable_traits : detail::incrementable_traits_helper<T> {
    };

    template <typename T>
    using iter_difference_t = typename incrementable_traits<T>::difference_type;

    // iter_value_t
    template <typename>
    struct readable_traits;

    namespace detail {
        template <typename T>
        struct with_value_type {
            using value_type = T;
        };
        template <typename, typename = void>
        struct readable_traits_helper {
        };

        template <typename T>
        struct readable_traits_helper<T*>
            : std::conditional<
                  std::is_object<T>::value,
                  with_value_type<typename std::remove_cv<T>::type>,
                  empty>::type {
        };

        template <typename I>
        struct readable_traits_helper<
            I,
            typename std::enable_if<std::is_array<I>::value>::type>
            : readable_traits<typename std::decay<I>::type> {
        };

        template <typename I>
        struct readable_traits_helper<
            const I,
            typename std::enable_if<!std::is_array<I>::value>::type>
            : readable_traits<typename std::decay<I>::type> {
        };

        template <typename T, typename V = typename T::value_type>
        struct member_value_type : std::conditional<std::is_object<V>::value,
                                                    with_value_type<V>,
                                                    empty>::type {
        };

        template <typename T, typename E = typename T::element_type>
        struct member_element_type
            : std::conditional<
                  std::is_object<E>::value,
                  with_value_type<typename std::remove_cv<E>::type>,
                  empty>::type {
        };

        template <typename T>
        using member_value_type_t = typename T::value_type;

        template <typename T>
        struct has_member_value_type : exists<member_value_type_t, T> {
        };

        template <typename T>
        using member_element_type_t = typename T::element_type;

        template <typename T>
        struct has_member_element_type : exists<member_element_type_t, T> {
        };

        template <typename T>
        struct readable_traits_helper<
            T,
            typename std::enable_if<has_member_value_type<T>::value &&
                                    !has_member_element_type<T>::value>::type>
            : member_value_type<T> {
        };

        template <typename T>
        struct readable_traits_helper<
            T,
            typename std::enable_if<has_member_element_type<T>::value &&
                                    !has_member_value_type<T>::value>::type>
            : member_element_type<T> {
        };

        template <typename T>
        struct readable_traits_helper<
            T,
            typename std::enable_if<has_member_element_type<T>::value &&
                                    has_member_value_type<T>::value>::type> {
        };
    }  // namespace detail

    template <typename T>
    struct readable_traits : detail::readable_traits_helper<T> {
    };

    template <typename T>
    using iter_value_t = typename readable_traits<T>::value_type;

    // sentinel_for
    struct sentinel_for_concept {
        template <typename S, typename I>
        auto _test_requires(S s, I i)
            -> decltype(::picorange::valid_expr(*i, i == s, i != s));
    };
    template <typename S, typename I>
    struct sentinel_for
        : std::integral_constant<
              bool,
              std::is_default_constructible<S>::value &&
                  std::is_copy_constructible<S>::value &&
                  _requires<sentinel_for_concept, S, I>::value> {
    };

    // sized_sentinel_for
    struct sized_sentinel_for_concept {
        template <typename S, typename I>
        auto _test_requires(const S& s, const I& i) -> decltype(
            requires_expr<
                std::is_same<decltype(s - i), iter_difference_t<I>>::value>{},
            requires_expr<
                std::is_same<decltype(i - s), iter_difference_t<I>>::value>{});
    };
    template <typename S, typename I>
    struct sized_sentinel_for
        : std::integral_constant<
              bool,
              _requires<sized_sentinel_for_concept, S, I>::value &&
                  sentinel_for<S, I>::value> {
    };
    template <typename S>
    struct sized_sentinel_for<S, void*> : std::false_type {
    };
    template <typename I>
    struct sized_sentinel_for<void*, I> : std::false_type {
    };
    template <>
    struct sized_sentinel_for<void*, void*> : std::false_type {
    };

    // begin
    namespace _begin {
        template <typename T>
        void begin(T&&) = delete;
        template <typename T>
        void begin(std::initializer_list<T>&&) = delete;

        struct fn {
        private:
            template <typename T, std::size_t N>
            static PICORANGE_CONSTEXPR14 void impl(T(&&)[N],
                                                   priority_tag<2>) = delete;

            template <typename T, std::size_t N>
            static PICORANGE_CONSTEXPR14 auto impl(T (&t)[N],
                                                   priority_tag<2>) noexcept
                -> decltype((t) + 0)
            {
                return (t) + 0;
            }

            template <typename T>
            static PICORANGE_CONSTEXPR14 auto impl(
                T& t,
                priority_tag<1>) noexcept(noexcept(decay_copy(t.begin())))
                -> decltype(decay_copy(t.begin()))
            {
                return decay_copy(t.begin());
            }

            template <typename T>
            static PICORANGE_CONSTEXPR14 auto
            impl(T&& t, priority_tag<0>) noexcept(
                noexcept(decay_copy(begin(std::forward<T>(t)))))
                -> decltype(decay_copy(begin(std::forward<T>(t))))
            {
                return decay_copy(begin(std::forward<T>(t)));
            }

        public:
            template <typename T>
            PICORANGE_CONSTEXPR14 auto operator()(T&& t) const
                noexcept(noexcept(fn::impl(std::forward<T>(t),
                                           priority_tag<3>{})))
                    -> decltype(fn::impl(std::forward<T>(t), priority_tag<3>{}))
            {
                return fn::impl(std::forward<T>(t), priority_tag<3>{});
            }
        };
    }  // namespace _begin
    namespace {
        constexpr auto& begin = static_const<_begin::fn>::value;
    }

    // end
    namespace _end {
        template <typename T>
        void end(T&&) = delete;
        template <typename T>
        void end(std::initializer_list<T>&&) = delete;

        struct fn {
        private:
            template <typename T, std::size_t N>
            static constexpr void impl(T(&&)[N], priority_tag<2>) = delete;

            template <typename T, std::size_t N>
            static constexpr auto impl(T (&t)[N], priority_tag<2>) noexcept
                -> decltype((t) + N)
            {
                return (t) + N;
            }

            template <
                typename T,
                typename S = decltype(decay_copy(std::declval<T&>().end())),
                typename I = decltype(::picorange::begin(std::declval<T&>()))>
            static constexpr auto impl(T& t, priority_tag<1>) noexcept(
                noexcept(decay_copy(t.end()))) -> decltype(decay_copy(t.end()))
            {
                return decay_copy(t.end());
            }

            template <
                typename T,
                typename S = decltype(decay_copy(end(std::declval<T>()))),
                typename I = decltype(::picorange::begin(std::declval<T>()))>
            static constexpr auto impl(T& t, priority_tag<0>) noexcept(
                noexcept(decay_copy(end(std::forward<T>(t))))) -> S
            {
                return decay_copy(end(std::forward<T>(t)));
            }

        public:
            template <typename T>
            constexpr auto operator()(T&& t) const
                noexcept(noexcept(fn::impl(std::forward<T>(t),
                                           priority_tag<2>{})))
                    -> decltype(fn::impl(std::forward<T>(t), priority_tag<2>{}))
            {
                return fn::impl(std::forward<T>(t), priority_tag<2>{});
            }
        };
    }  // namespace _end
    namespace {
        constexpr auto& end = static_const<_end::fn>::value;
    }

    // cbegin
    namespace _cbegin {
        struct fn {
            template <typename T>
            constexpr auto operator()(const T& t) const
                noexcept(noexcept(::picorange::begin(t)))
                    -> decltype(::picorange::begin(t))
            {
                return ::picorange::begin(t);
            }

            template <typename T>
            constexpr auto operator()(const T&& t) const noexcept(
                noexcept(::picorange::begin(static_cast<const T&&>(t))))
                -> decltype(::picorange::begin(static_cast<const T&&>(t)))
            {
                return ::picorange::begin(static_cast<const T&&>(t));
            }
        };
    }  // namespace _cbegin
    namespace {
        constexpr auto& cbegin = static_const<_cbegin::fn>::value;
    }

    // cend
    namespace _cend {
        struct fn {
            template <typename T>
            constexpr auto operator()(const T& t) const
                noexcept(noexcept(::picorange::end(t)))
                    -> decltype(::picorange::end(t))
            {
                return ::picorange::end(t);
            }

            template <typename T>
            constexpr auto operator()(const T&& t) const
                noexcept(noexcept(::picorange::end(static_cast<const T&&>(t))))
                    -> decltype(::picorange::end(static_cast<const T&&>(t)))
            {
                return ::picorange::end(static_cast<const T&&>(t));
            }
        };
    }  // namespace _cend
    namespace {
        constexpr auto& cend = static_const<_cend::fn>::value;
    }

    // range
    namespace detail {
        struct range_impl_concept {
            template <typename T>
            auto _test_requires(T&& t)
                -> decltype(::picorange::begin(std::forward<T>(t)),
                            ::picorange::end(std::forward<T>(t)));
        };
        template <typename T>
        struct range_impl : _requires<range_impl_concept, T> {
        };
    }  // namespace detail
    struct range_concept {
        template <typename>
        static auto test(long) -> std::false_type;
        template <typename T>
        static auto test(int) ->
            typename std::enable_if<detail::range_impl<T&>::value,
                                    std::true_type>::type;
    };
    template <typename T>
    struct range : decltype(range_concept::test<T>(0)) {
    };

    template <typename T>
    struct forwarding_range
        : std::integral_constant<bool,
                                 range<T>::value &&
                                     detail::range_impl<T>::value> {
    };

    // typedefs
    template <typename R>
    using iterator_t = typename std::enable_if<range<R>::value,
                                               decltype(::picorange::begin(
                                                   std::declval<R&>()))>::type;
    template <typename R>
    using sentinel_t = typename std::enable_if<range<R>::value,
                                               decltype(::picorange::end(
                                                   std::declval<R&>()))>::type;
    template <typename R>
    using range_difference_t =
        typename std::enable_if<range<R>::value,
                                iter_difference_t<iterator_t<R>>>::type;
    template <typename R>
    using range_value_t =
        typename std::enable_if<range<R>::value,
                                iter_value_t<iterator_t<R>>>::type;
    template <typename R>
    using range_reference_t =
        typename std::enable_if<range<R>::value,
                                iter_reference_t<iterator_t<R>>>::type;

    // view
    struct view_base {
    };
    namespace detail {
        template <typename>
        struct is_std_non_view : std::false_type {
        };
        template <typename T>
        struct is_std_non_view<std::initializer_list<T>> : std::true_type {
        };
    }  // namespace detail
    template <typename T>
    struct enable_view_helper
        : std::conditional<std::is_base_of<view_base, T>::value,
                           std::true_type,
                           typename std::conditional<
                               detail::is_std_non_view<T>::value,
                               std::false_type,
                               typename std::conditional<
                                   range<T>::value && range<const T>::value,
                                   std::is_same<range_reference_t<T>,
                                                range_reference_t<const T>>,
                                   std::true_type>::type>::type>::type {
    };
    template <typename T>
    struct view
        : std::integral_constant<bool,
                                 range<T>::value &&
                                     std::is_copy_constructible<T>::value &&
                                     std::is_default_constructible<T>::value &&
                                     enable_view_helper<T>::value> {
    };

    // data
    namespace detail {
        template <typename P>
        struct is_object_pointer
            : std::integral_constant<
                  bool,
                  std::is_pointer<P>::value &&
                      std::is_object<test_t<iter_value_t, P>>::value> {
        };
    }  // namespace detail

    namespace _data {
        struct fn {
        private:
            template <typename CharT, typename Traits, typename Allocator>
            static constexpr auto impl(
                std::basic_string<CharT, Traits, Allocator>& str,
                priority_tag<2>) noexcept ->
                typename std::basic_string<CharT, Traits, Allocator>::pointer
            {
                return std::addressof(*str.begin());
            }
            template <typename CharT, typename Traits, typename Allocator>
            static constexpr auto impl(
                const std::basic_string<CharT, Traits, Allocator>& str,
                priority_tag<2>) noexcept -> typename std::
                basic_string<CharT, Traits, Allocator>::const_pointer
            {
                return std::addressof(*str.begin());
            }
            template <typename CharT, typename Traits, typename Allocator>
            static constexpr auto impl(
                std::basic_string<CharT, Traits, Allocator>&& str,
                priority_tag<2>) noexcept ->
                typename std::basic_string<CharT, Traits, Allocator>::pointer
            {
                return std::addressof(*str.begin());
            }

            template <
                typename T,
                typename D = decltype(decay_copy(std::declval<T&>().data()))>
            static constexpr auto impl(T& t, priority_tag<1>) noexcept(
                noexcept(decay_copy(t.data()))) ->
                typename std::enable_if<detail::is_object_pointer<D>::value,
                                        D>::type
            {
                return decay_copy(t.data());
            }

            template <typename T>
            static constexpr auto impl(T&& t, priority_tag<0>) noexcept(
                noexcept(::picorange::begin(std::forward<T>(t)))) ->
                typename std::enable_if<
                    detail::is_object_pointer<decltype(
                        ::picorange::begin(std::forward<T>(t)))>::value,
                    decltype(::picorange::begin(std::forward<T>(t)))>::type
            {
                return ::picorange::begin(std::forward<T>(t));
            }

        public:
            template <typename T>
            constexpr auto operator()(T&& t) const
                noexcept(noexcept(fn::impl(std::forward<T>(t),
                                           priority_tag<2>{})))
                    -> decltype(fn::impl(std::forward<T>(t), priority_tag<2>{}))
            {
                return fn::impl(std::forward<T>(t), priority_tag<2>{});
            }
        };
    }  // namespace _data
    namespace {
        constexpr auto& data = static_const<_data::fn>::value;
    }

    // size
    template <typename>
    struct disable_sized_range : std::false_type {
    };

    namespace _size {
        template <typename T>
        void size(T&&) = delete;
        template <typename T>
        void size(T&) = delete;

        struct fn {
        private:
            template <typename T, std::size_t N>
            static constexpr std::size_t impl(const T(&&)[N],
                                              priority_tag<3>) noexcept
            {
                return N;
            }

            template <typename T, std::size_t N>
            static constexpr std::size_t impl(const T (&)[N],
                                              priority_tag<3>) noexcept
            {
                return N;
            }

            template <
                typename T,
                typename I = decltype(decay_copy(std::declval<T>().size()))>
            static constexpr auto impl(T&& t, priority_tag<2>) noexcept(
                noexcept(decay_copy(std::forward<T>(t).size()))) ->
                typename std::enable_if<
                    std::is_integral<I>::value &&
                        !disable_sized_range<remove_cvref_t<T>>::value,
                    I>::type
            {
                return decay_copy(std::forward<T>(t).size());
            }

            template <
                typename T,
                typename I = decltype(decay_copy(size(std::declval<T>())))>
            static constexpr auto impl(T&& t, priority_tag<1>) noexcept(
                noexcept(decay_copy(size(std::forward<T>(t))))) ->
                typename std::enable_if<
                    std::is_integral<I>::value &&
                        !disable_sized_range<remove_cvref_t<T>>::value,
                    I>::type
            {
                return decay_copy(size(std::forward<T>(t)));
            }

            template <
                typename T,
                typename I = decltype(::picorange::begin(std::declval<T>())),
                typename S = decltype(::picorange::end(std::declval<T>())),
                typename D = decltype(decay_copy(std::declval<S>() -
                                                 std::declval<I>()))>
            static constexpr auto impl(T&& t, priority_tag<0>) noexcept(
                noexcept(decay_copy(::picorange::end(t) -
                                    ::picorange::begin(t)))) -> typename std::
                enable_if<!std::is_array<remove_cvref_t<T>>::value, D>::type
            {
                return decay_copy(::picorange::end(t) - ::picorange::begin(t));
            }

        public:
            template <typename T>
            constexpr auto operator()(T&& t) const
                noexcept(noexcept(fn::impl(std::forward<T>(t),
                                           priority_tag<3>{})))
                    -> decltype(fn::impl(std::forward<T>(t), priority_tag<3>{}))
            {
                return fn::impl(std::forward<T>(t), priority_tag<3>{});
            }
        };
    }  // namespace _size
    namespace {
        constexpr auto& size = static_const<_size::fn>::value;
    }

    // empty
    namespace _empty_ns {
        struct fn {
        private:
            template <typename T>
            static constexpr auto impl(T&& t, priority_tag<2>) noexcept(
                noexcept((bool(std::forward<T>(t).empty()))))
                -> decltype((bool(std::forward<T>(t).empty())))
            {
                return bool((std::forward<T>(t).empty()));
            }
            template <typename T>
            static constexpr auto impl(T&& t, priority_tag<1>) noexcept(
                noexcept(::picorange::size(std::forward<T>(t)) == 0))
                -> decltype(::picorange::size(std::forward<T>(t)) == 0)
            {
                return ::picorange::size(std::forward<T>(t)) == 0;
            }

            template <
                typename T,
                typename I = decltype(::picorange::begin(std::declval<T>()))>
            static constexpr auto impl(T&& t, priority_tag<0>) noexcept(
                noexcept(::picorange::begin(t) == ::picorange::end(t)))
                -> decltype(::picorange::begin(t) == ::picorange::end(t))
            {
                return ::picorange::begin(t) == ::picorange::end(t);
            }

        public:
            template <typename T>
            constexpr auto operator()(T&& t) const
                noexcept(noexcept(fn::impl(std::forward<T>(t),
                                           priority_tag<2>{})))
                    -> decltype(fn::impl(std::forward<T>(t), priority_tag<2>{}))
            {
                return fn::impl(std::forward<T>(t), priority_tag<2>{});
            }
        };
    }  // namespace _empty_ns
    namespace {
        constexpr auto& empty = static_const<_empty_ns::fn>::value;
    }

    // sized_range
    struct sized_range_concept {
        template <typename T>
        auto _test_requires(T& t) -> decltype(::picorange::size(t));
    };
    template <typename T>
    struct sized_range
        : std::integral_constant<
              bool,
              range<T>::value &&
                  !disable_sized_range<remove_cvref_t<T>>::value &&
                  _requires<sized_range_concept, T>::value> {
    };

    // contiguous_range
    struct contiguous_range_concept {
        template <typename>
        static auto test(long) -> std::false_type;
        template <typename T>
        static auto test(int) -> typename std::enable_if<
            _requires<contiguous_range_concept, T>::value,
            std::true_type>::type;

        template <typename T>
        auto _test_requires(T& t) -> decltype(
            requires_expr<
                std::is_same<decltype(::picorange::data(t)),
                             typename std::add_pointer<
                                 range_reference_t<T>>::type>::value>{});
    };
    template <typename T>
    struct contiguous_range : decltype(contiguous_range_concept::test<T>(0)) {
    };

    // subrange
    template <typename D>
    class view_interface : public view_base {
        static_assert(std::is_class<D>::value, "");
        static_assert(std::is_same<D, typename std::remove_cv<D>::type>::value,
                      "");

    private:
        PICORANGE_CONSTEXPR14 D& derived() noexcept
        {
            return static_cast<D&>(*this);
        }
        constexpr D& derived() const noexcept
        {
            return static_cast<const D&>(*this);
        }

    public:
        PICORANGE_NODISCARD PICORANGE_CONSTEXPR14 bool empty()
        {
            return ::picorange::begin(derived()) == ::picorange::end(derived());
        }
        PICORANGE_NODISCARD constexpr bool empty() const
        {
            return ::picorange::begin(derived()) == ::picorange::end(derived());
        }

        template <typename R = D,
                  typename = decltype(::picorange::empty(std::declval<R&>()))>
        PICORANGE_CONSTEXPR14 explicit operator bool()
        {
            return !::picorange::empty(derived());
        }
        template <
            typename R = D,
            typename = decltype(::picorange::empty(std::declval<const R&>()))>
        constexpr explicit operator bool() const
        {
            return !::picorange::empty(derived());
        }

        template <typename R = D,
                  typename std::enable_if<contiguous_range<R>::value>::type* =
                      nullptr>
        auto data() -> decltype(
            std::addressof(*::picorange::begin(static_cast<R&>(*this))))
        {
            return ::picorange::empty(derived())
                       ? nullptr
                       : std::addressof(*::picorange::begin(derived()));
        }
        template <typename R = D,
                  typename std::enable_if<
                      contiguous_range<const R>::value>::type* = nullptr>
        auto data() const -> decltype(
            std::addressof(*::picorange::begin(static_cast<const R&>(*this))))
        {
            return ::picorange::empty(derived())
                       ? nullptr
                       : std::addressof(*::picorange::begin(derived()));
        }

        template <typename R = D,
                  typename std::enable_if<
                      range<R>::value &&
                      sized_sentinel_for<sentinel_t<R>, iterator_t<R>>::value>::
                      type* = nullptr>
        PICORANGE_CONSTEXPR14 auto size()
            -> decltype(::picorange::end(static_cast<R&>(*this)) -
                        ::picorange::begin(static_cast<R&>(*this)))
        {
            return ::picorange::end(derived()) - ::picorange::begin(derived());
        }

        template <typename R = D,
                  typename std::enable_if<
                      range<const R>::value &&
                      sized_sentinel_for<sentinel_t<const R>,
                                         iterator_t<const R>>::value>::type* =
                      nullptr>
        constexpr auto size() const
            -> decltype(::picorange::end(static_cast<const R&>(*this)) -
                        ::picorange::begin(static_cast<const R&>(*this)))
        {
            return ::picorange::end(derived()) - ::picorange::begin(derived());
        }
    };

    enum class subrange_kind : bool { unsized, sized };

    namespace detail {
        template <typename I, typename S>
        struct default_subrange_kind
            : std::integral_constant<subrange_kind,
                                     sized_sentinel_for<S, I>::value
                                         ? subrange_kind::sized
                                         : subrange_kind::unsized> {
        };
    }  // namespace detail

    namespace _subrange {
        template <typename I,
                  typename S = I,
                  subrange_kind = detail::default_subrange_kind<I, S>::value>
        class subrange;
    }  // namespace _subrange

    using _subrange::subrange;

    namespace detail {
        struct pair_like_concept {
            template <typename>
            static auto test(long) -> std::false_type;
            template <typename T, typename = typename std::tuple_size<T>::type>
            static auto test(int) ->
                typename std::enable_if<_requires<pair_like_concept, T>::value,
                                        std::true_type>::type;

            template <typename T>
            auto _test_requires(T t) -> decltype(
                requires_expr<
                    std::is_base_of<std::integral_constant<std::size_t, 2>,
                                    std::tuple_size<T>>::value>{},
                std::declval<
                    std::tuple_element<0,
                                       typename std::remove_const<T>::type>>(),
                std::declval<
                    std::tuple_element<1,
                                       typename std::remove_const<T>::type>>(),
                requires_expr<std::is_convertible<
                    decltype(std::get<0>(t)),
                    const std::tuple_element<0, T>&>::value>{},
                requires_expr<std::is_convertible<
                    decltype(std::get<1>(t)),
                    const std::tuple_element<1, T>&>::value>{});
        };
        template <typename T>
        struct pair_like
            : std::integral_constant<bool,
                                     !std::is_reference<T>::value &&
                                         decltype(pair_like_concept::test<T>(
                                             0))::value> {
        };

        struct pair_like_convertible_to_concept {
            template <typename T, typename U, typename V>
            auto _test_requires(T&& t)
                -> decltype(requires_expr<std::is_convertible<
                                decltype(std::get<0>(std::forward<T>(t))),
                                U>::value>{},
                            requires_expr<std::is_convertible<
                                decltype(std::get<1>(std::forward<T>(t))),
                                V>::value>{});
        };
        template <typename T, typename U, typename V>
        struct pair_like_convertible_to
            : std::integral_constant<
                  bool,
                  !range<T>::value &&
                      pair_like<
                          typename std::remove_reference<T>::type>::value &&
                      _requires<pair_like_convertible_to_concept, T, U, V>::
                          value> {
        };
        template <typename T, typename U, typename V>
        struct pair_like_convertible_from
            : std::integral_constant<
                  bool,
                  !range<T>::value &&
                      pair_like<
                          typename std::remove_reference<T>::type>::value &&
                      std::is_constructible<T, U, V>::value> {
        };

        struct iterator_sentinel_pair_concept {
            template <typename>
            static auto test(long) -> std::false_type;
            template <typename T>
            static auto test(int) -> typename std::enable_if<
                !range<T>::value && pair_like<T>::value &&
                    sentinel_for<
                        typename std::tuple_element<1, T>::type,
                        typename std::tuple_element<0, T>::type>::value,
                std::true_type>::type;
        };
        template <typename T>
        struct iterator_sentinel_pair
            : decltype(iterator_sentinel_pair_concept::test<T>(0)) {
        };

        template <typename I, typename S, bool StoreSize = false>
        struct subrange_data {
            constexpr subrange_data() = default;
            constexpr subrange_data(I&& b, S&& e)
                : begin(std::move(b)), end(std::move(e))
            {
            }
            template <bool Dependent = true>
            constexpr subrange_data(
                I&& b,
                S&& e,
                typename std::enable_if<Dependent, iter_difference_t<I>>::type)
                : begin(std::move(b)), end(std::move(e))
            {
            }

            constexpr iter_difference_t<I> get_size() const
            {
                return distance(begin, end);
            }

            I begin{};
            S end{};
        };

        template <typename I, typename S>
        struct subrange_data<I, S, true> {
            constexpr subrange_data() = default;
            constexpr subrange_data(I&& b, S&& e, iter_difference_t<I> s)
                : begin(std::move(b)), end(std::move(e)), size(s)
            {
            }

            constexpr iter_difference_t<I> get_size() const
            {
                return size;
            }

            I begin{};
            S end{};
            iter_difference_t<I> size{0};
        };

        template <typename R, typename I, typename S, subrange_kind K>
        auto _subrange_range_constructor_constraint_helper_fn(long)
            -> std::false_type;

        template <typename R, typename I, typename S, subrange_kind K>
        auto _subrange_range_constructor_constraint_helper_fn(int) ->
            typename std::enable_if<
                forwarding_range<R>::value &&
                    std::is_convertible<iterator_t<R>, I>::value &&
                    std::is_convertible<sentinel_t<R>, S>::value,
                std::true_type>::type;

        template <typename R, typename I, typename S, subrange_kind K>
        struct subrange_range_constructor_constraint_helper
            : decltype(
                  _subrange_range_constructor_constraint_helper_fn<R, I, S, K>(
                      0)) {
        };

        template <typename R>
        constexpr subrange_kind subrange_deduction_guide_helper()
        {
            return (sized_range<R>::value ||
                    sized_sentinel_for<sentinel_t<R>, iterator_t<R>>::value)
                       ? subrange_kind::sized
                       : subrange_kind::unsized;
        }

        template <typename T, typename U>
        struct not_same_as
            : std::integral_constant<
                  bool,
                  !std::is_same<remove_cvref_t<T>, remove_cvref_t<U>>::value> {
        };
    }  // namespace detail

    namespace _subrange {
        template <typename I, typename S, subrange_kind K>
        class subrange : public view_interface<subrange<I, S, K>> {
            static_assert(sentinel_for<S, I>::value, "");
            static_assert(K == subrange_kind::sized ||
                              !sized_sentinel_for<S, I>::value,
                          "");

            static constexpr bool _store_size =
                K == subrange_kind::sized && !sized_sentinel_for<S, I>::value;

        public:
            using iterator = I;
            using sentinel = S;

            subrange() = default;

            template <bool SS = _store_size,
                      typename std::enable_if<!SS>::type* = nullptr>
            PICORANGE_CONSTEXPR14 subrange(I i, S s)
                : m_data{std::move(i), std::move(s)}
            {
            }
            template <bool Dependent = true,
                      subrange_kind KK = K,
                      typename std::enable_if<
                          KK == subrange_kind::sized>::type* = nullptr>
            PICORANGE_CONSTEXPR14 subrange(
                I i,
                S s,
                typename std::enable_if<Dependent, iter_difference_t<I>>::type
                    n)
                : m_data{std::move(i), std::move(s), n}
            {
            }

            constexpr I begin() const noexcept
            {
                return m_data.begin;
            }

            constexpr S end() const noexcept
            {
                return m_data.end;
            }

            PICORANGE_NODISCARD constexpr bool empty() const noexcept
            {
                return m_data.begin == m_data.end;
            }

            template <subrange_kind KK = K,
                      typename std::enable_if<
                          KK == subrange_kind::sized>::type* = nullptr>
            constexpr iter_difference_t<I> size() const noexcept
            {
                return m_data.get_size();
            }

        private:
            detail::subrange_data<I, S, _store_size> m_data{};
        };

        template <typename I, typename S, subrange_kind K>
        I begin(subrange<I, S, K>&& r) noexcept
        {
            return r.begin();
        }
        template <typename I, typename S, subrange_kind K>
        S end(subrange<I, S, K>&& r) noexcept
        {
            return r.end();
        }
    }  // namespace _subrange

    namespace detail {
        template <std::size_t N>
        struct subrange_get_impl;
        template <>
        struct subrange_get_impl<0> {
            template <typename I, typename S, subrange_kind K>
            static auto get(const subrange<I, S, K>& s) -> decltype(s.begin())
            {
                return s.begin();
            }
        };
        template <>
        struct subrange_get_impl<1> {
            template <typename I, typename S, subrange_kind K>
            static auto get(const subrange<I, S, K>& s) -> decltype(s.end())
            {
                return s.end();
            }
        };
    }  // namespace detail

    template <std::size_t N,
              typename I,
              typename S,
              subrange_kind K,
              typename std::enable_if<(N < 2)>::type* = nullptr>
    auto get(const subrange<I, S, K>& s)
        -> decltype(detail::subrange_get_impl<N>::get(s))
    {
        return detail::subrange_get_impl<N>::get(s);
    }

    // reconstructible_range
    template <typename R>
    struct pair_reconstructible_range
        : std::integral_constant<
              bool,
              range<R>::value &&
                  forwarding_range<
                      typename std::remove_reference<R>::type>::value &&
                  std::is_constructible<R, iterator_t<R>, sentinel_t<R>>::
                      value> {
    };
    template <typename R>
    struct reconstructible_range
        : std::integral_constant<
              bool,
              range<R>::value &&
                  forwarding_range<
                      typename std::remove_reference<R>::type>::value &&
                  std::is_constructible<
                      R,
                      subrange<iterator_t<R>, sentinel_t<R>>>::value> {
    };

    // bidir iterator
    struct bidirectional_iterator_concept {
        template <typename I>
        auto _test_requires(I i)
            -> decltype(requires_expr<std::is_same<decltype(i--), I>::value>{});
        template <typename>
        static auto test(long) -> std::false_type;
        template <typename I>
        static auto test(int) -> typename std::enable_if<
            std::is_base_of<bidirectional_iterator_tag,
                            iterator_category_t<I>>::value &&
                _requires<bidirectional_iterator_concept, I>::value,
            std::true_type>::type;
    };
    template <typename I>
    struct bidirectional_iterator
        : decltype(bidirectional_iterator_concept::test<I>(0)) {
    };

    // random access iterator
    struct random_access_iterator_concept {
        template <typename I>
        auto _test_requires(I i, const I j, const iter_difference_t<I> n)
            -> decltype(valid_expr(
                j + n,
                requires_expr<std::is_same<decltype(j + n), I>::value>{},
                n + j,
#ifndef _MSC_VER
                requires_expr<std::is_same<decltype(n + j), I>::value>{},
#endif
                j - n,
                requires_expr<std::is_same<decltype(j - n), I>::value>{},
                j[n],
                requires_expr<
                    std::is_same<decltype(j[n]), iter_reference_t<I>>::value>{},
                requires_expr<
                    std::is_convertible<decltype(i < j), bool>::value>{}));
        template <typename>
        static auto test(long) -> std::false_type;
        template <typename I>
        static auto test(int) -> typename std::enable_if<
            bidirectional_iterator<I>::value &&
                std::is_base_of<random_access_iterator_tag,
                                iterator_category_t<I>>::value &&
                sized_sentinel_for<I, I>::value &&
                _requires<random_access_iterator_concept, I>::value,
            std::true_type>::type;
    };
    template <typename I>
    struct random_access_iterator
        : decltype(random_access_iterator_concept::test<I>(0)) {
    };

    // advance
    namespace _advance {
        struct fn {
        private:
            template <typename T>
            static constexpr T abs(T t)
            {
                return t < T{0} ? -t : t;
            }

            template <typename R,
                      typename std::enable_if<
                          random_access_iterator<R>::value>::type* = nullptr>
            static PICORANGE_CONSTEXPR14 void impl(R& r, iter_difference_t<R> n)
            {
                r += n;
            }

            template <typename I,
                      typename std::enable_if<
                          bidirectional_iterator<I>::value &&
                          !random_access_iterator<I>::value>::type* = nullptr>
            static PICORANGE_CONSTEXPR14 void impl(I& i, iter_difference_t<I> n)
            {
                constexpr auto zero = iter_difference_t<I>{0};

                if (n > zero) {
                    while (n-- > zero) {
                        ++i;
                    }
                }
                else {
                    while (n++ < zero) {
                        --i;
                    }
                }
            }

            template <typename I,
                      typename std::enable_if<
                          !bidirectional_iterator<I>::value>::type* = nullptr>
            static PICORANGE_CONSTEXPR14 void impl(I& i, iter_difference_t<I> n)
            {
                while (n-- > iter_difference_t<I>{0}) {
                    ++i;
                }
            }

            template <typename I,
                      typename S,
                      typename std::enable_if<
                          std::is_assignable<I&, S>::value>::type* = nullptr>
            static PICORANGE_CONSTEXPR14 void impl(I& i,
                                                   S bound,
                                                   priority_tag<2>)
            {
                i = std::move(bound);
            }

            template <typename I,
                      typename S,
                      typename std::enable_if<
                          sized_sentinel_for<S, I>::value>::type* = nullptr>
            static PICORANGE_CONSTEXPR14 void impl(I& i,
                                                   S bound,
                                                   priority_tag<1>)
            {
                fn::impl(i, bound - i);
            }

            template <typename I, typename S>
            static PICORANGE_CONSTEXPR14 void impl(I& i,
                                                   S bound,
                                                   priority_tag<0>)
            {
                while (i != bound) {
                    ++i;
                }
            }

            template <typename I,
                      typename S,
                      typename std::enable_if<
                          sized_sentinel_for<S, I>::value>::type* = nullptr>
            static PICORANGE_CONSTEXPR14 auto impl(I& i,
                                                   iter_difference_t<I> n,
                                                   S bound)
                -> iter_difference_t<I>
            {
                if (fn::abs(n) >= fn::abs(bound - i)) {
                    auto dist = bound - i;
                    fn::impl(i, bound, priority_tag<2>{});
                    return dist;
                }
                else {
                    fn::impl(i, n);
                    return n;
                }
            }

            template <typename I,
                      typename S,
                      typename std::enable_if<
                          bidirectional_iterator<I>::value &&
                          !sized_sentinel_for<S, I>::value>::type* = nullptr>
            static PICORANGE_CONSTEXPR14 auto impl(I& i,
                                                   iter_difference_t<I> n,
                                                   S bound)
                -> iter_difference_t<I>
            {
                constexpr iter_difference_t<I> zero{0};
                iter_difference_t<I> counter{0};

                if (n < zero) {
                    do {
                        --i;
                        --counter;
                    } while (++n < zero && i != bound);
                }
                else {
                    while (n-- > zero && i != bound) {
                        ++i;
                        ++counter;
                    }
                }

                return counter;
            }

            template <typename I,
                      typename S,
                      typename std::enable_if<
                          !bidirectional_iterator<I>::value &&
                          !sized_sentinel_for<S, I>::value>::type* = nullptr>
            static PICORANGE_CONSTEXPR14 auto impl(I& i,
                                                   iter_difference_t<I> n,
                                                   S bound)
                -> iter_difference_t<I>
            {
                constexpr iter_difference_t<I> zero{0};
                iter_difference_t<I> counter{0};

                while (n-- > zero && i != bound) {
                    ++i;
                    ++counter;
                }

                return counter;
            }

        public:
            template <typename I>
            PICORANGE_CONSTEXPR14 void operator()(I& i,
                                                  iter_difference_t<I> n) const
            {
                fn::impl(i, n);
            }

            template <typename I,
                      typename S,
                      typename std::enable_if<
                          sentinel_for<S, I>::value>::type* = nullptr>
            PICORANGE_CONSTEXPR14 void operator()(I& i, S bound) const
            {
                fn::impl(i, bound, priority_tag<2>{});
            }

            template <typename I,
                      typename S,
                      typename std::enable_if<
                          sentinel_for<S, I>::value>::type* = nullptr>
            PICORANGE_CONSTEXPR14 iter_difference_t<I>
            operator()(I& i, iter_difference_t<I> n, S bound) const
            {
                return n - fn::impl(i, n, bound);
            }
        };
    }  // namespace _advance
    namespace {
        constexpr auto& advance = static_const<_advance::fn>::value;
    }

    // distance
    namespace _distance {
        struct fn {
        private:
            template <typename I, typename S>
            static PICORANGE_CONSTEXPR14 auto impl(I i, S s) ->
                typename std::enable_if<sized_sentinel_for<S, I>::value,
                                        iter_difference_t<I>>::type
            {
                return s - i;
            }

            template <typename I, typename S>
            static PICORANGE_CONSTEXPR14 auto impl(I i, S s) ->
                typename std::enable_if<!sized_sentinel_for<S, I>::value,
                                        iter_difference_t<I>>::type
            {
                iter_difference_t<I> counter{0};
                while (i != s) {
                    ++i;
                    ++counter;
                }
                return counter;
            }

            template <typename R>
            static PICORANGE_CONSTEXPR14 auto impl(R&& r) ->
                typename std::enable_if<sized_range<R>::value,
                                        iter_difference_t<iterator_t<R>>>::type
            {
                return static_cast<iter_difference_t<iterator_t<R>>>(
                    ::picorange::size(r));
            }

            template <typename R>
            static PICORANGE_CONSTEXPR14 auto impl(R&& r) ->
                typename std::enable_if<!sized_range<R>::value,
                                        iter_difference_t<iterator_t<R>>>::type
            {
                return fn::impl(::picorange::begin(r), ::picorange::end(r));
            }

        public:
            template <typename I, typename S>
            PICORANGE_CONSTEXPR14 auto operator()(I first, S last) const ->
                typename std::enable_if<sentinel_for<S, I>::value,
                                        iter_difference_t<I>>::type
            {
                return fn::impl(std::move(first), std::move(last));
            }

            template <typename R>
            PICORANGE_CONSTEXPR14 auto operator()(R&& r) const ->
                typename std::enable_if<range<R>::value,
                                        iter_difference_t<iterator_t<R>>>::type
            {
                return fn::impl(std::forward<R>(r));
            }
        };
    }  // namespace _distance
    namespace {
        constexpr auto& distance = static_const<_distance::fn>::value;
    }
    PICORANGE_END_NAMESPACE
}  // namespace picorange

namespace std {
    template <typename I, typename S, ::picorange::subrange_kind K>
    class tuple_size<::picorange::subrange<I, S, K>>
        : public integral_constant<size_t, 2> {
    };

    template <typename I, typename S, ::picorange::subrange_kind K>
    class tuple_element<0, ::picorange::subrange<I, S, K>> {
    public:
        using type = I;
    };
    template <typename I, typename S, ::picorange::subrange_kind K>
    class tuple_element<1, ::picorange::subrange<I, S, K>> {
    public:
        using type = S;
    };

    using ::picorange::get;
}  // namespace std

#endif  // PICORANGE_H
