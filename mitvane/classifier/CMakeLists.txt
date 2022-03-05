
set(CXX_SOURCES
    classifier.cpp
)

add_mitvane_component(classifier ${CXX_SOURCES})
target_link_libraries(classifier PRIVATE Vanetza::btp)
