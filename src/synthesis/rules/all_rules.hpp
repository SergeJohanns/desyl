#pragma once

#include <rules.hpp>
#include <emp.hpp>
#include <frame.hpp>
#include <write.hpp>
#include <read.hpp>
#include <heap_unify.hpp>
#include <memory>

namespace desyl
{
    constexpr size_t RULES = 5;

    // Pointers because the subclasses take up different amounts of memory
    const std::unique_ptr<Rule> all_rules[RULES] = {
        std::make_unique<EmpRule>(EmpRule()),
        std::make_unique<FrameRule>(FrameRule()),
        std::make_unique<WriteRule>(WriteRule()),
        std::make_unique<ReadRule>(ReadRule()),
        std::make_unique<HeapUnifyRule>(HeapUnifyRule()),
    };
}
