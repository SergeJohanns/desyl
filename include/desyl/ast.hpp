#pragma once

#include <string>
#include <vector>
#include <variant>
#include <memory>

namespace desyl
{
    using Literal = int;
    using Identifier = std::string;

    // Forward declarations because definitions recursively depend on Expression
    struct UnaryOperatorCall;
    struct BinaryOperatorCall;

    using Expression = std::variant<
        Literal,
        Identifier,
        UnaryOperatorCall,
        BinaryOperatorCall>;

    enum class UnaryOperator
    {
        Neg,
        Not,
    };

    struct UnaryOperatorCall
    {
        UnaryOperator type;
        std::unique_ptr<Expression> operand;
    };

    enum class BinaryOperator
    {
        Eq,
        Neq,
        Lt,
        Leq,
        Gt,
        Geq,
        And,
        Or,
        Implies,
    };

    struct BinaryOperatorCall
    {
        BinaryOperator type;
        std::unique_ptr<Expression> lhs;
        std::unique_ptr<Expression> rhs;
    };

    struct ArrayDeclaration
    {
        std::string name;
        int size;
    };

    struct PointerDeclaration
    {
        std::unique_ptr<Expression> base;
        int offset;
        std::unique_ptr<Expression> value;
    };

    struct PredicateCall
    {
        std::string name;
        std::vector<std::string> args;
    };

    struct Heap
    {
        std::vector<ArrayDeclaration> array_declarations;
        std::vector<PointerDeclaration> pointer_declarations;
        std::vector<PredicateCall> predicate_calls;
    };

    using Proposition = Expression;

    struct Assertion
    {
        Proposition proposition;
        Heap heap;
    };

    enum class Type
    {
        Int,
        Loc,
    };

    struct TypedVariable
    {
        Type type;
        std::string name;
    };

    struct FunctionSignature
    {
        std::string name;
        std::vector<TypedVariable> args;
    };

    struct FunctionSpecification
    {
        Assertion precondition;
        FunctionSignature signature;
        Assertion postcondition;
    };

    enum class AlgebraicType
    {
        Int,
        Loc,
        Set,
    };

    struct AlgebraicTypedVariable
    {
        AlgebraicType type;
        std::string name;
    };

    struct Predicate
    {
        std::string name;
        std::vector<AlgebraicTypedVariable> args;
    };

    struct Query
    {
        std::vector<Predicate> predicates;
        std::vector<FunctionSpecification> functions;
    };
}
