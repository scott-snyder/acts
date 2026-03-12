# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     AdaptiveGridTrackDensity.cpp
     AdaptiveMultiVertexFinder.cpp
     AdaptiveMultiVertexFitter.cpp
     FullBilloirVertexFitter.cpp
     ImpactPointEstimator.cpp
     KalmanVertexUpdater.cpp
     DIRECTORY ../..
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fvisibility-inlines-hidden "
     COMPILE_DEFINITIONS "FLATTEN" )
endif()

target_sources(
    ActsCore
    PRIVATE
        AdaptiveGridTrackDensity.cpp
        KalmanVertexUpdater.cpp
        KalmanVertexUpdaterImpl3.cpp
        KalmanVertexUpdaterImpl4.cpp
        FsmwMode1dFinder.cpp
        VertexingError.cpp
        IterativeVertexFinder.cpp
        AdaptiveMultiVertexFitter.cpp
        AdaptiveGridDensityVertexFinder.cpp
        ZScanVertexFinder.cpp
        HelicalTrackLinearizer.cpp
        FullBilloirVertexFitter.cpp
        AdaptiveMultiVertexFinder.cpp
        Vertex.cpp
        NumericalTrackLinearizer.cpp
        TrackDensityVertexFinder.cpp
        GaussianTrackDensity.cpp
        ImpactPointEstimator.cpp
        GaussianGridTrackDensity.cpp
        GridDensityVertexFinder.cpp
        HoughVertexFinder2.cpp
)

