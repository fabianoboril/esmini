
include_directories (
  ${ROADMANAGER_INCLUDE_DIR}
  ${OSG_INCLUDE_DIR}
  ${COMMON_MINI_INCLUDE_DIR}  
)

set ( SOURCES
  viewer.cpp
  RubberbandManipulator.cpp
)

set ( INCLUDES
  viewer.hpp
  RubberbandManipulator.hpp
)

add_definitions(-DOSG_LIBRARY_STATIC)

add_library ( ViewerBase STATIC ${SOURCES} ${INCLUDES} )
