# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\RawrXD_StubGUI_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\RawrXD_StubGUI_autogen.dir\\ParseCache.txt"
  "RawrXD_StubGUI_autogen"
  )
endif()
