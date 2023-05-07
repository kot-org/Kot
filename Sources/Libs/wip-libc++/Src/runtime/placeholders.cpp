#include <functional>

namespace std
{
namespace placeholders
{

/**
* The std::placeholders namespace contains the placeholder objects [_1, . . . _N]
* where N is an implementation defined maximum number.
*
* When used as an argument in a std::bind expression, the placeholder objects
* are stored in the generated function object, and when that function object is
* invoked with unbound arguments, each placeholder _N is replaced by the
* corresponding Nth unbound argument.
*/

//TODO: should these be inline constexpr? (C++17)
decltype(std::placeholders::_1) _1;
decltype(std::placeholders::_2) _2;
decltype(std::placeholders::_3) _3;
decltype(std::placeholders::_4) _4;

}  // namespace placeholders
}  // namespace std
