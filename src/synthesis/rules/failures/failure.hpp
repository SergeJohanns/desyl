#include <exception>

namespace desyl
{
    struct Failure : public std::exception
    {
        virtual const char *what() const throw()
        {
            return "My exception happened";
        }
    };
}
