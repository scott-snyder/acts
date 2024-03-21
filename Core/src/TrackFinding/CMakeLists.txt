
# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     MeasurementSelector.cpp
     DIRECTORY ../..
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()

target_sources(
    ActsCore
    PRIVATE
        CombinatorialKalmanFilterError.cpp
        MeasurementSelector.cpp
        AmbiguityTrackClustering.cpp
)
