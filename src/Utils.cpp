#include <cppti/Utils.hh>

namespace cppti
{
void toSnakeCase(std::string& str)
{
  for (auto& c : str)
  {
    if (c == ' ')
      c = '_';
    else
      c = static_cast<char>(std::tolower(c));
  }
}
}
