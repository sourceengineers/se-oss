/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include <se-oss/log/FunctionRef.h>

#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

using se_oss::FunctionRef;

namespace {

int twice(int value) { return 2 * value; }

std::size_t sizeOnly(const void*, std::size_t size) { return size; }

// A callee taking a FunctionRef by value, the way the buffer and context APIs do.
int invoke(FunctionRef<int(int)> function, int value) { return function(value); }

}  // namespace

// The whole point of the type: two words, trivially copyable, never empty.
static_assert(sizeof(FunctionRef<int(int)>) == 2 * sizeof(void*), "FunctionRef must be two pointers wide");
static_assert(std::is_trivially_copyable<FunctionRef<int(int)>>::value, "FunctionRef must be trivially copyable");
static_assert(!std::is_default_constructible<FunctionRef<int(int)>>::value, "FunctionRef must never be empty");
static_assert(!std::is_constructible<FunctionRef<int(int)>, std::nullptr_t>::value, "FunctionRef must never be empty");

TEST(FunctionRefTest, InvokesACapturelessLambda)
{
    EXPECT_EQ(6, invoke([](int value) { return 3 * value; }, 2));
}

TEST(FunctionRefTest, InvokesALambdaWithCaptures)
{
    const int offset {10};
    const std::string text {"abc"};

    EXPECT_EQ(16, invoke([&](int value) { return offset + value + static_cast<int>(text.size()); }, 3));
}

TEST(FunctionRefTest, RefersToTheCallableInsteadOfCopyingIt)
{
    // A mutable lambda changes its own state; the FunctionRef has to invoke that very object.
    auto counter = [calls = 0](int value) mutable { return ++calls + value; };
    FunctionRef<int(int)> reference = counter;

    EXPECT_EQ(1, reference(0));
    EXPECT_EQ(2, reference(0));
    EXPECT_EQ(3, counter(0));
}

TEST(FunctionRefTest, InvokesAConstCallable)
{
    const auto increment = [](int value) { return value + 1; };
    FunctionRef<int(int)> reference = increment;

    EXPECT_EQ(8, reference(7));
}

TEST(FunctionRefTest, InvokesAStdFunction)
{
    const std::function<int(int)> function = twice;

    EXPECT_EQ(10, invoke(function, 5));
}

TEST(FunctionRefTest, InvokesAPlainFunction)
{
    FunctionRef<int(int)> reference = twice;
    FunctionRef<std::size_t(const void*, std::size_t)> consumer = sizeOnly;

    EXPECT_EQ(8, reference(4));
    EXPECT_EQ(7U, consumer(nullptr, 7U));
}

TEST(FunctionRefTest, ForwardsReferenceArguments)
{
    int target {0};
    FunctionRef<void(int&)> assign = [](int& value) { value = 42; };

    assign(target);

    EXPECT_EQ(42, target);
}

TEST(FunctionRefTest, CopiesReferToTheSameCallable)
{
    auto counter = [calls = 0]() mutable { return ++calls; };
    FunctionRef<int()> first = counter;
    FunctionRef<int()> second = first;

    EXPECT_EQ(1, first());
    EXPECT_EQ(2, second());
}
