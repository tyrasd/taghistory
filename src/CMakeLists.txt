project(taghistory)

add_executable(osmium
    hist.cpp
    ${OSMIUM_SOURCE_FILES}
)

target_link_libraries(osmium ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})

set_pthread_on_target(osmium)

install(TARGETS osmium DESTINATION bin)
