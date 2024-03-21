# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     Polyhedron.cpp
     TrackingVolume.cpp
     DIRECTORY ../..
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()

target_sources(
    ActsCore
    PRIVATE
        AlignablePortalVisitor.cpp
        ConeLayer.cpp
        ConeVolumeBounds.cpp
        CuboidVolumeBounds.cpp
        CuboidVolumeBuilder.cpp
        CutoutCylinderVolumeBounds.cpp
        CylinderLayer.cpp
        CylinderVolumeBounds.cpp
        CylinderVolumeBuilder.cpp
        CylinderVolumeHelper.cpp
        Extent.cpp
        DiscLayer.cpp
        GenericApproachDescriptor.cpp
        GenericCuboidVolumeBounds.cpp
        GeometryIdentifier.cpp
        GlueVolumesDescriptor.cpp
        Layer.cpp
        LayerArrayCreator.cpp
        LayerCreator.cpp
        NavigationLayer.cpp
        PassiveLayerBuilder.cpp
        PlaneLayer.cpp
        Polyhedron.cpp
        ProtoLayer.cpp
        ProtoLayerHelper.cpp
        VolumePlacementBase.cpp
        SurfaceArrayCreator.cpp
        TrackingGeometry.cpp
        TrackingGeometryBuilder.cpp
        TrackingVolume.cpp
        TrackingVolumeArrayCreator.cpp
        TrapezoidVolumeBounds.cpp
        DiamondVolumeBounds.cpp
        Volume.cpp
        VolumeBounds.cpp
        CylinderVolumeStack.cpp
        CuboidVolumeStack.cpp
        Portal.cpp
        GridPortalLink.cpp
        GridPortalLinkMerging.cpp
        TrivialPortalLink.cpp
        CompositePortalLink.cpp
        PortalLinkBase.cpp
        PortalError.cpp
        CylinderPortalShell.cpp
        CuboidPortalShell.cpp
        BlueprintNode.cpp
        Blueprint.cpp
        BlueprintOptions.cpp
        StaticBlueprintNode.cpp
        LayerBlueprintNode.cpp
        IndexGrid.cpp
        MaterialDesignatorBlueprintNode.cpp
        VolumeAttachmentStrategy.cpp
        VolumeResizeStrategy.cpp
        GeometryIdentifierBlueprintNode.cpp
        TrackingGeometryVisitor.cpp
        TrapezoidPortalShell.cpp
        DiamondPortalShell.cpp
        VolumeStack.cpp
        ContainerBlueprintNode.cpp
        MultiWireVolumeBuilder.cpp
        ReferenceGenerators.cpp
        TrackingGeometryPrintVisitor.cpp
        BoundDeduplicator.cpp
        PortalPlacement.cpp
)
