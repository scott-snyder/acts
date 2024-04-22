# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     BinnedSurfaceMaterial.cpp
     MaterialGridHelper.cpp
     SurfaceMaterialMapper.cpp
     VolumeMaterialMapper.cpp
     DIRECTORY ../..
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fvisibility-inlines-hidden "
     COMPILE_DEFINITIONS "FLATTEN" )
endif()

target_sources(
    ActsCore
    PRIVATE
        AccumulatedMaterialSlab.cpp
        AccumulatedSurfaceMaterial.cpp
        AccumulatedVolumeMaterial.cpp
        AverageMaterials.cpp
        BinnedSurfaceMaterial.cpp
        BinnedSurfaceMaterialAccumulator.cpp
        GridSurfaceMaterialFactory.cpp
        HomogeneousSurfaceMaterial.cpp
        HomogeneousVolumeMaterial.cpp
        Interactions.cpp
        IntersectionMaterialAssigner.cpp
        ISurfaceMaterial.cpp
        Material.cpp
        MaterialGridHelper.cpp
        MaterialInteractionAssignment.cpp
        MaterialMapUtils.cpp
        MaterialMapper.cpp
        MaterialSlab.cpp
        MaterialValidator.cpp
        MergedMaterialMarker.cpp
        ProtoVolumeMaterial.cpp
        SurfaceMaterialMapper.cpp
        VolumeMaterialMapper.cpp
)
