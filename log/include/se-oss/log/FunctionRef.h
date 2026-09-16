/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace se_oss {

template<typename Signature>
class FunctionRef;

/**
 * Non-owning reference to a callable; the C++14 stand-in for std::function_ref.
 *
 * A FunctionRef is two words wide: a pointer to the callable and a pointer to a trampoline that
 * invokes it. Binding a lambda to one therefore never allocates, whatever the lambda captures.
 * That is what makes it the right parameter type for the buffer and context hot paths, where a
 * `const std::function&` parameter silently copies any closure larger than a few pointers onto
 * the heap on every single log call.
 *
 * Rules:
 * - A FunctionRef does not own the callable and does not extend its lifetime. Use it as a
 *   function parameter type only. Never store it in a member, and never let it outlive a
 *   temporary it was created from.
 * - A FunctionRef is never empty. It has no default constructor and cannot be created from
 *   nullptr, so a callee may invoke it without checking. (A null pointer *variable* of function
 *   pointer type is the caller's responsibility, as it is with std::function.)
 *
 * @tparam R The return type.
 * @tparam Args The parameter types.
 */
template<typename R, typename... Args>
class FunctionRef<R(Args...)> final
{
public:
    /**
     * References a callable object: a lambda, a functor, a std::function, ...
     *
     * Implicit on purpose, so that a lambda can be passed straight to a FunctionRef parameter,
     * the same way it can be passed to a std::function parameter.
     *
     * @tparam F The callable type. Constrained to types that are actually callable with Args and
     *           yield something convertible to R (anything, if R is void), so that this constructor
     *           neither hijacks copying a FunctionRef nor accepts a non-callable.
     * @param callable The callable to reference. It has to outlive the FunctionRef.
     */
    template<
        typename F,
        typename = std::enable_if_t<
            !std::is_same<std::decay_t<F>, FunctionRef>::value
            && !std::is_function<std::remove_reference_t<F>>::value
            && (std::is_void<R>::value
                || std::is_convertible<decltype(std::declval<F&>()(std::declval<Args>()...)), R>::value)>>
    // cppcheck-suppress noExplicitConstructor
    FunctionRef(F&& callable) noexcept :  // NOLINT(google-explicit-constructor)
        _target {const_cast<void*>(static_cast<const void*>(std::addressof(callable)))},
        _invoke {&invokeObject<std::remove_reference_t<F>>}
    {
    }

    /**
     * References a plain function.
     *
     * @param function The function to reference.
     */
    // cppcheck-suppress noExplicitConstructor
    FunctionRef(R (*function)(Args...)) noexcept :  // NOLINT(google-explicit-constructor)
        _target {function},
        _invoke {&invokeFunction}
    {
    }

    /**
     * Not empty, ever: nullptr would otherwise convert to a null function pointer and be accepted by
     * the constructor above. Deliberately not explicit, so that copy-initialisation from nullptr is
     * rejected as well.
     */
    FunctionRef(std::nullptr_t) = delete;  // NOLINT(google-explicit-constructor)

    /**
     * Invokes the referenced callable.
     */
    R operator()(Args... args) const { return _invoke(_target, std::forward<Args>(args)...); }

private:
    /**
     * Object pointers and function pointers may differ in representation, so they are kept in
     * separate union members instead of being cast into one another. Each trampoline reads the
     * member the constructor that selected it has written.
     */
    union Target
    {
        constexpr explicit Target(void* objectPointer) noexcept : object {objectPointer} { }
        constexpr explicit Target(R (*functionPointer)(Args...)) noexcept : function {functionPointer} { }

        void* object;
        R (*function)(Args...);
    };

    template<typename T>
    static R invokeObject(Target target, Args... args)
    {
        return (*static_cast<std::add_pointer_t<T>>(target.object))(std::forward<Args>(args)...);
    }

    static R invokeFunction(Target target, Args... args) { return (*target.function)(std::forward<Args>(args)...); }

    Target _target;
    R (*_invoke)(Target, Args...);
};

}  // namespace se_oss
