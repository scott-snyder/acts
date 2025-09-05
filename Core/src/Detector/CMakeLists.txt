# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     DetectorVolume.cpp
     CuboidVolumeBuilder.cpp
     DIRECTORY ../..
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fvisibility-inlines-hidden "
     COMPILE_DEFINITIONS "FLATTEN" )
endif()


target_sources(
    ActsCore
    PRIVATE
        detail/BlueprintHelper.cpp
        detail/BlueprintDrawer.cpp
        detail/CuboidalDetectorHelper.cpp
        detail/CylindricalDetectorHelper.cpp
        detail/DetectorVolumeConsistency.cpp
        detail/PortalHelper.cpp
        detail/ProtoMaterialHelper.cpp
        detail/SupportSurfacesHelper.cpp
        detail/IndexedGridFiller.cpp
        CylindricalContainerBuilder.cpp
        CuboidalContainerBuilder.cpp
        Detector.cpp
        DetectorBuilder.cpp
        DetectorVolume.cpp
        DetectorVolumeBuilder.cpp
        IndexedRootVolumeFinderBuilder.cpp
        LayerStructureBuilder.cpp
        Portal.cpp
        PortalGenerators.cpp
        ProtoDetector.cpp
        GeometryIdGenerator.cpp
        VolumeStructureBuilder.cpp
        MultiWireStructureBuilder.cpp
)
