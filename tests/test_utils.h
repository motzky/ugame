#pragma once

#include <print>

#include <gtest/gtest.h>

#include "utils/exception.h"
#include "utils/formatter.h"

#define TEST_IMPL(imple)             \
    try                              \
    {                                \
        imple                        \
    }                                \
    catch (const game::Exception &e) \
    {                                \
        std::println("{}", e);       \
        throw;                       \
    }
