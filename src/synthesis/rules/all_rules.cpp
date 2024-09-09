#include <all_rules.hpp>
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
#include <pure_rewrites/null_not_lval.hpp>
#include <recursion/open.hpp>
#include <recursion/close.hpp>
#include <recursion/call.hpp>

namespace desyl
{
    std::vector<std::shared_ptr<Rule>> all_rules()
    {
        // WARNING: MEMORY DOES NOT GET FREED, DO NOT USE THIS IN A LOOP
        std::vector<Expression> *learned_clauses = new std::vector<Expression>();

        // Pointers because the subclasses take up different amounts of memory
        const std::vector<std::shared_ptr<Rule>> all_rules_vector = {
            // (Early) termination rules
            // std::make_shared<PostInconsistentRule>(PostInconsistentRule()),
            // std::make_shared<SetSizeRule>(SetSizeRule()),
            std::make_shared<EmpRule>(EmpRule()),
            // Strictly simplifying lossless rules
            std::make_shared<NullNotLValRule>(NullNotLValRule()),
            std::make_shared<TrueElisionRule>(TrueElisionRule()),
            std::make_shared<FrameRule>(FrameRule()),
            std::make_shared<PureFrameRule>(PureFrameRule()),
            std::make_shared<ReflexiveRule>(ReflexiveRule()),
            std::make_shared<ExpandImpliedRule>(ExpandImpliedRule()),
            // Unfolding phase rules
            std::make_shared<OpenRule>(OpenRule()),
            std::make_shared<CloseRule>(CloseRule()), // Complicates heap and adds backtracking targets
            // Substitution rules
            std::make_shared<HeapUnifyRule>(HeapUnifyRule()),
            std::make_shared<PickRule>(PickRule()),
            std::make_shared<PureUnifyRule>(PureUnifyRule()),
            std::make_shared<SubstRightRule>(SubstRightRule()),
            // Expensive but nongenerating rules
            // std::make_shared<PostInvalidRule>(PostInvalidRule(*learned_clauses)), // Not expensive to run but adds branching targets
            // Desctructive generation rules (at the end to avoid generating code that is not used)
            std::make_shared<WriteRule>(WriteRule()),
            std::make_shared<FreeRule>(FreeRule()),
            std::make_shared<AllocRule>(AllocRule()),
            std::make_shared<BranchRule>(BranchRule(*learned_clauses)),
            std::make_shared<CallRule>(CallRule()),
            // Invertable generation rules (will *always* generate code that is not used if possible)
            std::make_shared<ReadRule>(ReadRule()),
        };
        return all_rules_vector;
    }
}
