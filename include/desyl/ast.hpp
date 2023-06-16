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
        std::shared_ptr<Expression> operand;
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
        std::shared_ptr<Expression> lhs;
        std::shared_ptr<Expression> rhs;
    };

    struct ArrayDeclaration
    {
        Identifier name;
        int size;
        bool operator==(ArrayDeclaration const &other) const = default;
    };

    struct PointerDeclaration
    {
        std::shared_ptr<Expression> base;
        int offset;
        std::shared_ptr<Expression> value;
        bool operator==(PointerDeclaration const &other) const = default;
    };

    struct PredicateCall
    {
        Identifier name;
        std::vector<Identifier> args;
        bool operator==(PredicateCall const &other) const = default;
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
        Identifier name;
    };

    struct FunctionSignature
    {
        Identifier name;
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
        Identifier name;
    };

    struct Predicate
    {
        Identifier name;
        std::vector<AlgebraicTypedVariable> args;
    };

    struct Query
    {
        std::vector<Predicate> predicates;
        std::vector<FunctionSpecification> functions;
    };
}
