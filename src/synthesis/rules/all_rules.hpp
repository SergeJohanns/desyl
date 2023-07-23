#pragma once

#include <rules.hpp>
#include <heap_rewrites/emp.hpp>
#include <heap_rewrites/frame.hpp>
#include <statements/write.hpp>
#include <statements/read.hpp>
#include <statements/branch.hpp>
#include <statements/free.hpp>
#include <statements/alloc.hpp>
#include <substitution/heap_unify.hpp>
#include <substitution/pick.hpp>
#include <substitution/pure_unify.hpp>
#include <substitution/subst_right.hpp>
#include <failures/post_invalid.hpp>
#include <failures/post_inconsistent.hpp>
#include <failures/set_size.hpp>
#include <pure_rewrites/pure_frame.hpp>
#include <pure_rewrites/reflexivity.hpp>
#include <pure_rewrites/expand_implied.hpp>
#include <pure_rewrites/true_elision.hpp>
#include <recursion/open.hpp>
#include <recursion/close.hpp>
#include <recursion/call.hpp>
#include <memory>

namespace desyl
{
    constexpr size_t ALL_RULES = 21;

    std::vector<Expression> learned_clauses;

    // Pointers because the subclasses take up different amounts of memory
    const std::unique_ptr<Rule> all_rules[ALL_RULES] = {
        // (Early) termination rules
        std::make_unique<PostInconsistentRule>(PostInconsistentRule()),
        std::make_unique<SetSizeRule>(SetSizeRule()),
        std::make_unique<EmpRule>(EmpRule()),
        // Unfolding phase rules
        std::make_unique<OpenRule>(OpenRule()),
        std::make_unique<CloseRule>(CloseRule()), // Complicates heap and adds backtracking targets
        // Strictly simplifying lossless rules
        std::make_unique<TrueElisionRule>(TrueElisionRule()),
        std::make_unique<FrameRule>(FrameRule()),
        std::make_unique<PureFrameRule>(PureFrameRule()),
        std::make_unique<ReflexiveRule>(ReflexiveRule()),
        std::make_unique<ExpandImpliedRule>(ExpandImpliedRule()),
        // Substitution rules
        std::make_unique<HeapUnifyRule>(HeapUnifyRule()),
        std::make_unique<PickRule>(PickRule()),
        std::make_unique<PureUnifyRule>(PureUnifyRule()),
        std::make_unique<SubstRightRule>(SubstRightRule()),
        // Expensive but nongenerating rules
        std::make_unique<PostInvalidRule>(PostInvalidRule(learned_clauses)), // Not expensive to run but adds branching targets
        // Desctructive generation rules (at the end to avoid generating code that is not used)
        std::make_unique<WriteRule>(WriteRule()),
        std::make_unique<FreeRule>(FreeRule()),
        std::make_unique<AllocRule>(AllocRule()),
        std::make_unique<BranchRule>(BranchRule(learned_clauses)),
        std::make_unique<CallRule>(CallRule()),
        // Invertable generation rules (will *always* generate code that is not used if possible)
        std::make_unique<ReadRule>(ReadRule()),
    };
}
