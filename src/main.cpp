#include <iostream>

#include <httplib.h>

#include <cppti/HTTPController.hh>
#include <cppti/TalksDB.hh>
#include <cppti/TalksLoader.hh>

namespace
{
template <typename T>
using Expected = cppti::Expected<T>;
using Talk = cppti::Talk;
using TalksDB = cppti::TalksDB;

Expected<TalksDB> buildDB(std::vector<Talk> talks)
{
  auto db = TalksDB{std::move(talks)};
  return db;
}
}

int main(int argc, char const* const* argv)
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << *argv << " <db.json>\n";
    return 1;
  }
  return cppti::TalksLoader::loadFrom(argv[1])
      .and_then(buildDB)
      .and_then([&](auto db) -> Expected<int> {
        auto controller = cppti::HTTPController{db};
        controller.listen("localhost", 8485);
        return 0;
      })
      .value_or(1);
}
