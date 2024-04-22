# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     GainMatrixSmoother.cpp
     GainMatrixUpdater.cpp
     DIRECTORY ../..
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fvisibility-inlines-hidden "
     COMPILE_DEFINITIONS "FLATTEN" )
endif()

target_sources(
    ActsCore
    PRIVATE
        KalmanFitterError.cpp
        GainMatrixUpdater.cpp
        GainMatrixSmoother.cpp
        GlobalChiSquareFitterError.cpp
        GsfError.cpp
        GsfUtils.cpp
        BetheHeitlerApprox.cpp
        GsfMixtureReduction.cpp
        GlobalChiSquareFitter.cpp
        MbfSmoother.cpp
)

foreach(DIM RANGE 1 6)
    set(dim_file ${CMAKE_CURRENT_BINARY_DIR}/GainMatrixUpdaterImpl${DIM}.cpp)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/GainMatrixUpdaterImpl.cpp.in
        ${dim_file}
        @ONLY
    )

    set_source_files_properties(
        ${dim_file}
        PROPERTIES
            COMPILE_DEFINITIONS ACTS_GAIN_MATRIX_UPDATER_INSTANTIATE=${DIM}
    )
    target_sources(ActsCore PRIVATE ${dim_file})
endforeach()
