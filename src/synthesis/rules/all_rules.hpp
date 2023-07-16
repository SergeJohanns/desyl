#pragma once

#include <rules.hpp>
#include <emp.hpp>
#include <frame.hpp>
#include <write.hpp>
#include <read.hpp>
#include <heap_unify.hpp>
#include <pick.hpp>
#include <pure_unify.hpp>
#include <subst_right.hpp>
#include <branch.hpp>
#include <failures/post_invalid.hpp>
#include <failures/pre_inconsistent.hpp>
#include <pure_rewrites/pure_frame.hpp>
#include <pure_rewrites/reflexivity.hpp>
#include <pure_rewrites/expand_implied.hpp>
#include <memory>

namespace desyl
{
    constexpr size_t RULES = 14;

    std::vector<Expression> learned_clauses;

    // Pointers because the subclasses take up different amounts of memory
    const std::unique_ptr<Rule> all_rules[RULES] = {
        std::make_unique<PreInconsistentRule>(PreInconsistentRule()),
        std::make_unique<EmpRule>(EmpRule()),
        std::make_unique<FrameRule>(FrameRule()),
        std::make_unique<PureFrameRule>(PureFrameRule()),
        std::make_unique<ReflexiveRule>(ReflexiveRule()),
        std::make_unique<HeapUnifyRule>(HeapUnifyRule()),
        std::make_unique<PickRule>(PickRule()),
        std::make_unique<PureUnifyRule>(PureUnifyRule()),
        std::make_unique<SubstRightRule>(SubstRightRule()),
        std::make_unique<ExpandImpliedRule>(ExpandImpliedRule()),
        std::make_unique<PostInvalidRule>(PostInvalidRule(learned_clauses)),
        std::make_unique<WriteRule>(WriteRule()),
        std::make_unique<BranchRule>(BranchRule(learned_clauses)),
        std::make_unique<ReadRule>(ReadRule()),
    };
}
