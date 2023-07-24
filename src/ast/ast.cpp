#include <desyl/ast.hpp>

#include <sstream>
#include <iostream>

namespace desyl
{
    std::string stringify_op(UnaryOperator const &op)
    {
        return op == UnaryOperator::Neg ? std::string{"-"} : std::string{"!"};
    }

    std::string stringify_op(BinaryOperator const &op)
    {
        switch (op)
        {
        case BinaryOperator::Union:
            return std::string{"++"};
        case BinaryOperator::Intersect:
            return std::string{"**"};
        case BinaryOperator::Mul:
            return std::string{"*"};
        case BinaryOperator::Div:
            return std::string{"/"};
        case BinaryOperator::Mod:
            return std::string{"%"};
        case BinaryOperator::Add:
            return std::string{"+"};
        case BinaryOperator::Sub:
            return std::string{"-"};
        case BinaryOperator::PSubs:
            return std::string{"<s"};
        case BinaryOperator::Subs:
            return std::string{"<=s"};
        case BinaryOperator::PSups:
            return std::string{">s"};
        case BinaryOperator::Sups:
            return std::string{">=s"};
        case BinaryOperator::Lt:
            return std::string{"<"};
        case BinaryOperator::Leq:
            return std::string{"<="};
        case BinaryOperator::Gt:
            return std::string{">"};
        case BinaryOperator::Geq:
            return std::string{">="};
        case BinaryOperator::Iso:
            return std::string{"=i"};
        case BinaryOperator::Eq:
            return std::string{"=="};
        case BinaryOperator::Neq:
            return std::string{"!="};
        case BinaryOperator::And:
            return std::string{"&&"};
        case BinaryOperator::Or:
            return std::string{"||"};
        case BinaryOperator::Implies:
            return std::string{"=>"};
        default:
            return std::string{};
        }
    }

    // Standard overload resolution (https://www.modernescpp.com/index.php/visiting-a-std-variant-with-the-overload-pattern)
    // doesn't like recursive lambdas, so we use a plain struct
    struct StringifyVisitor
    {
        const auto operator()(Literal const &literal) const
        {
            return std::to_string(literal);
        }

        const auto operator()(SetLiteral const &set) const
        {
            std::vector<std::string> elements;
            for (auto const &element : set.elements)
            {
                elements.push_back(std::visit(*this, element));
            }
            std::stringstream stream;
            stream << "{";
            for (size_t i = 0; i + 1 < elements.size(); ++i)
            {
                stream << elements[i] << ", ";
            }
            if (!elements.empty())
            {
                stream << elements.back();
            }
            stream << "}";
            return stream.str();
        }

        const auto operator()(Identifier const &identifier) const
        {
            return identifier;
        }

        const auto operator()(UnaryOperatorCall const &call) const
        {
            return std::string{"("} + stringify_op(call.type) + std::visit(*this, *call.operand) + std::string{")"};
        }

        const auto operator()(BinaryOperatorCall const &call) const
        {
            return std::string{"("} + std::visit(*this, *call.lhs) + std::string{" "} + stringify_op(call.type) + std::string{" "} + std::visit(*this, *call.rhs) + std::string{")"};
        }
    };

    constexpr StringifyVisitor stringify;

    std::string stringify_expression(Expression const &expression)
    {
        return std::visit(stringify, expression);
    }

    std::string stringify_heap(Heap const &heap)
    {
        std::vector<std::string> heaplets;
        for (auto const &array : heap.array_declarations)
        {
            heaplets.push_back("[" + array.name + ", " + std::to_string(array.size) + "]");
        }
        for (auto const &pointer : heap.pointer_declarations)
        {
            heaplets.push_back("<" + stringify_expression(*pointer.base) + ", " + std::to_string(pointer.offset) + "> -> " + stringify_expression(*pointer.value));
        }
        for (auto const &predicate : heap.predicate_calls)
        {
            std::string args;
            for (size_t i = 0; i + 1 < predicate.args.size(); ++i)
            {
                args += predicate.args[i] + ", ";
            }
            if (!predicate.args.empty())
            {
                args += predicate.args.back();
            }
            heaplets.push_back(predicate.name + "(" + args + ")");
        }
        std::stringstream stream;
        for (size_t i = 0; i + 1 < heaplets.size(); ++i)
        {
            stream << heaplets[i] << ", ";
        }
        if (!heaplets.empty())
        {
            stream << heaplets.back();
        }
        else
        {
            stream << "emp";
        }
        stream << ";";
        return stream.str();
    }

    std::string stringify_proposition(Proposition const &prop)
    {
        std::stringstream stream;
        for (size_t i = 0; i + 1 < prop.size(); ++i)
        {
            stream << stringify_expression(prop[i]) << " && ";
        }
        if (!prop.empty())
        {
            stream << stringify_expression(prop.back());
        }
        else
        {
            stream << "true";
        }
        stream << ";";
        return stream.str();
    }

    std::string stringify_function_spec(FunctionSpecification const &spec)
    {
        std::stringstream stream;
        stream << "{";
        stream << stringify_proposition(spec.precondition.proposition) << " ";
        stream << stringify_heap(spec.precondition.heap);
        stream << "} -> {";
        stream << stringify_proposition(spec.postcondition.proposition) << " ";
        stream << stringify_heap(spec.postcondition.heap);
        stream << "}";
        return stream.str();
    }

    bool UnaryOperatorCall::operator==(UnaryOperatorCall const &other) const
    {
        return type == other.type && *operand == *other.operand;
    }

    bool BinaryOperatorCall::operator==(BinaryOperatorCall const &other) const
    {
        return type == other.type && *lhs == *other.lhs && *rhs == *other.rhs;
    }

    bool PointerDeclaration::operator==(PointerDeclaration const &other) const
    {
        return *base == *other.base && offset == other.offset && *value == *other.value;
    }

    bool PredicateCall::operator==(PredicateCall const &other) const
    {
        // Ignores label since that is only used for unfolding
        return name == other.name && args == other.args;
    }
}
