


set(CXX_SOURCES
    cam_application_parser.cpp
    denm_application_parser.cpp
    mapem_application_parser.cpp
    spatem_application_parser.cpp
)

add_mitvane_component(app_layer_parser ${CXX_SOURCES})
target_link_libraries(app_layer_parser PUBLIC Vanetza-extension::asn1)
target_link_libraries(app_layer_parser PRIVATE Vanetza::geonet Vanetza::btp)

