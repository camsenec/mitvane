
set(CXX_SOURCES
    btp_decoder.cpp
    geonet_decoder.cpp
)

add_mitvane_component(decoder ${CXX_SOURCES})
target_link_libraries(decoder PRIVATE Vanetza::btp Vanetza::geonet Vanetza::security)
