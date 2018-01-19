
set(ADDON_SRC
  compiler.cpp
  src/addon.cpp
  src/cm_db.cpp
  src/cm_sqlite.cpp
  src/cm_debug.c
  src/sqlite3.c
  )
add_definitions(-DTHREADSAFE=0)
set(CMAKE_CXX_FLAGS "-std=c++11")
include_directories(../includes inc)
add_executable(cm_test ${ADDON_SRC})
target_link_libraries(cm_test dl)
