# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     TransformationFreeToBound.cpp
     DIRECTORY ../..
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()

target_sources(
    ActsCore
    PRIVATE
        PrintParameters.cpp
        TransformationHelpers.cpp
        CorrectedTransformationFreeToBound.cpp
        TrackStatePropMask.cpp
        VectorMultiTrajectory.cpp
        VectorTrackContainer.cpp
        TrackParameterHelpers.cpp
        SeedContainer.cpp
        SpacePointContainer.cpp
        MultiComponentTrackParameters.cpp
)
