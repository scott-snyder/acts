acts_add_library(
    PluginJson
    src/AlgebraJsonConverter.cpp
    src/ExtentJsonConverter.cpp
    src/GridJsonConverter.cpp
    src/GeometryIdentifierJsonConverter.cpp
    src/IndexGridNavigationJsonConverter.cpp
    src/JsonMaterialDecorator.cpp
    src/MaterialMapJsonConverter.cpp
    src/MaterialJsonConverter.cpp
    src/ProtoAxisJsonConverter.cpp
    src/SurfaceBoundsJsonConverter.cpp
    src/SurfaceJsonConverter.cpp
    src/UtilitiesJsonConverter.cpp
    src/VolumeBoundsJsonConverter.cpp
    src/VolumeJsonConverter.cpp
    src/AmbiguityConfigJsonConverter.cpp
    src/DetrayJsonHelper.cpp
    src/JsonDetectorElement.cpp
    src/JsonSurfacesReader.cpp
    src/DefinitionsJsonConverter.cpp
    src/Seeding2ConfigJsonConverter.cpp
    ACTS_INCLUDE_FOLDER include/ActsPlugins
)

target_include_directories(
    ActsPluginJson
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(
    ActsPluginJson
    PUBLIC Acts::Core nlohmann_json::nlohmann_json
)

target_compile_definitions(
    ActsPluginJson
    PRIVATE JSON_DISABLE_ENUM_SERIALIZATION=1
)

acts_compile_headers(PluginJson GLOB include/**/*.hpp)


# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     src/MaterialJsonConverter.cpp
     src/MaterialMapJsonConverter.cpp
     src/JsonMaterialDecorator.cpp
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fvisibility-inlines-hidden "
     COMPILE_DEFINITIONS "FLATTEN" )
endif()
