# cpptalksindex
An index for C++ talks

## Aim
The aim of this is to provide a community-managed tags for C++ conferences.
Both experienced developers seeking to improve on a particular area or beginners looking for new areas to explore may use it to find material to watch.

## Adding a talk
Talks are gathered from `db.json`. Copy-pasting should get you the structure:
```json
  {
    "speakers": ["sample_speaker", "sample_speaker2"],
    "title": "",
    "conference": "CppCon",
    "year": 2020,
    "link": "https://www.youtube.com/watch?v=",
    "tags": [
      "no-tags-yet"
    ]
  },
```

## Building the server
Requirements:

  - `cmake` (tested with cmake 3.18.4)
  - A recent C++ compiler, supporting the C++17 standard (tested with gcc 10.2.0)

Steps:

```sh
mkdir build && cd build
cmake .. # -GNinja
cmake --build . # Or make / ninja
ln -s ../www . # The server will run fine but 404 if you forget this step
```

There should be a `bin/cpptalksindex` binary in `build`.
You can start the server with `./bin/cpptalksindex ../db.json`, which will run the server.
The server listens on port 8485 (hardcoded in `main.cpp`, should you need to change it).
