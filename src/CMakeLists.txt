set (CMAKE_CXX_FLAGS "-std=c++11")

project(taghistory)

add_executable(taghistory
    hist.cpp
    ${OSMIUM_SOURCE_FILES}
)

target_link_libraries(taghistory ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})

set_pthread_on_target(taghistory)

install(TARGETS taghistory DESTINATION bin)
