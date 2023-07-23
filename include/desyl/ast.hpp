#pragma once

#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <unordered_map>

namespace desyl
{
    using Literal = int;
    using Identifier = std::string;
    // Forward declarations because definitions recursively depend on Expression
    struct SetLiteral;
    struct UnaryOperatorCall;
    struct BinaryOperatorCall;

    constexpr int EXPRESSION_IDENTIFIER_INDEX = 1;
    using Expression = std::variant<
        Literal,
        Identifier,
        SetLiteral,
        UnaryOperatorCall,
        BinaryOperatorCall>;

    struct SetLiteral
    {
        std::vector<Expression> elements;
        bool operator==(SetLiteral const &other) const = default;
    };

    enum class UnaryOperator
    {
        Neg,
        Not,
    };

    struct UnaryOperatorCall
    {
        UnaryOperator type;
        std::shared_ptr<Expression> operand;
        bool operator==(UnaryOperatorCall const &other) const;
    };

    enum class BinaryOperator
    {
        Intersect,
        Union,
        Mul,
        Div,
        Mod,
        Add,
        Sub,
        Subs,
        PSubs,
        Sups,
        PSups,
        Lt,
        Leq,
        Gt,
        Geq,
        Iso,
        Niso,
        Eq,
        Neq,
        And,
        Or,
        Implies,
    };

    struct BinaryOperatorCall
    {
        BinaryOperator type;
        std::shared_ptr<Expression> lhs;
        std::shared_ptr<Expression> rhs;
        bool operator==(BinaryOperatorCall const &other) const;
    };

    std::string stringify_expression(Expression const &expression);

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
        bool operator==(PointerDeclaration const &other) const;
    };

    struct PredicateCall
    {
        Identifier name;
        int label;
        std::vector<Identifier> args;
        bool operator==(PredicateCall const &other) const = default;
    };

    struct Heap
    {
        std::vector<ArrayDeclaration> array_declarations;
        std::vector<PointerDeclaration> pointer_declarations;
        std::vector<PredicateCall> predicate_calls;
    };

    using Proposition = std::vector<Expression>;

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

    std::string stringify_function_spec(FunctionSpecification const &spec);

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

    struct Clause
    {
        Expression condition;
        Assertion assertion;
    };

    struct Predicate
    {
        Identifier name;
        std::vector<AlgebraicTypedVariable> args;
        std::vector<Clause> clauses;
    };

    struct Query
    {
        std::unordered_map<Identifier, Predicate> predicates;
        std::unordered_map<Identifier, FunctionSpecification> functions;
    };
}
