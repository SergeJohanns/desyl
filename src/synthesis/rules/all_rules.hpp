#pragma once

#include <rules.hpp>
#include <emp.hpp>
#include <frame.hpp>
#include <write.hpp>
#include <read.hpp>
#include <memory>

namespace desyl
{
    constexpr size_t RULES = 4;

    // Pointers because the subclasses take up different amounts of memory
    const std::unique_ptr<Rule> all_rules[RULES] = {
        std::make_unique<EmpRule>(EmpRule()),
        std::make_unique<FrameRule>(FrameRule()),
        std::make_unique<WriteRule>(WriteRule()),
        std::make_unique<ReadRule>(ReadRule()),
    };
}
