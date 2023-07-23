#include <context.hpp>

namespace desyl
{
    void vars(Literal const &, Vars &) {}

    void vars(Identifier const &element, Vars &target)
    {
        target.insert(element);
    }

    void vars(UnaryOperatorCall const &element, Vars &target)
    {
        vars(*element.operand, target);
    }

    void vars(SetLiteral const &element, Vars &target)
    {
        for (auto const &element : element.elements)
        {
            vars(element, target);
        }
    }

    void vars(BinaryOperatorCall const &element, Vars &target)
    {
        vars(*element.lhs, target);
        vars(*element.rhs, target);
    }

    void vars(Expression const &element, Vars &target)
    {
        std::visit(
            [&target](auto const &e)
            { vars(e, target); },
            element);
    }

    void vars(Proposition const &element, Vars &target)
    {
        for (auto const &expression : element)
        {
            vars(expression, target);
        }
    }

    void vars(ArrayDeclaration const &element, Vars &target)
    {
        target.insert(element.name);
    }

    void vars(PointerDeclaration const &element, Vars &target)
    {
        vars(*element.base, target);
        vars(*element.value, target);
    }

    void vars(PredicateCall const &element, Vars &target)
    {
        for (auto const &arg : element.args)
        {
            vars(arg, target);
        }
    }

    void vars(Heap const &element, Vars &target)
    {
        for (auto const &array : element.array_declarations)
        {
            vars(array, target);
        }
        for (auto const &pointer : element.pointer_declarations)
        {
            vars(pointer, target);
        }
        for (auto const &predicate : element.predicate_calls)
        {
            vars(predicate, target);
        }
    }

    void vars(Assertion const &element, Vars &target)
    {
        vars(element.heap, target);
        vars(element.proposition, target);
    }

    void vars(TypedVariable const &element, Vars &target)
    {
        target.insert(element.name);
    }

    void vars(FunctionSignature const &element, Vars &target)
    {
        for (auto const &arg : element.args)
        {
            vars(arg, target);
        }
    }
}
