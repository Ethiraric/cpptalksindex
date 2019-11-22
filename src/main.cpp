#include <iostream>

#include <cppti/TalksLoader.hh>

int main(int, char const* const* argv)
{
  auto const etalks = cppti::TalksLoader::loadFrom(argv[1]);
  if (etalks)
  {
    auto const& talks = etalks.value();
    for (auto const& talk : talks)
      std::cout << talk.speakers[0] << " - " << talk.title << std::endl;
  }
  return 0;
}
