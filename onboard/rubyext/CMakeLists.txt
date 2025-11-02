########################################################
set(TARGET_EXT_LIBRARY Balloon)
set(SWIG_IF_FILE ${TARGET_EXT_LIBRARY}.i)
set(CLASS_LIST_FILE class_list_${TARGET_EXT_LIBRARY}.hh)
set(RUBY_EXT_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/include
  ${ANLNEXT_INC_DIR}
  )
set(RUBY_EXT_LIBRARY_DIRS
  ${ANLNEXT_LIB_DIR}
  )
set(RUBY_EXT_LIBRARIES
  ${ANLNEXT_LIB}
  ProjectXBalloon
  )

include_directories(${Boost_INCLUDE_DIRS}) #追加した
########################################################
set(cxx_definitions )
set(SWIG_FLAGS ${cxx_definitions})
add_definitions(${cxx_definitions})

########################################################
include(CreateSwigRuby)