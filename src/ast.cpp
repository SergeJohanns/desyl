#include <desyl/ast.hpp>

namespace desyl
{
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
}
