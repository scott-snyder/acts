# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     AnnulusBounds.cpp
     ConeSurface.cpp
     CylinderBounds.cpp
     CylinderSurface.cpp
     DiscSurface.cpp
     LineSurface.cpp
     PlaneSurface.cpp
     RectangleBounds.cpp
     Surface.cpp
     VerticesHelper.cpp
     DIRECTORY ../..
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fvisibility-inlines-hidden "
     COMPILE_DEFINITIONS "FLATTEN" )
endif()

target_sources(
    ActsCore
    PRIVATE
        AnnulusBounds.cpp
        BoundaryTolerance.cpp
        ConeBounds.cpp
        ConeSurface.cpp
        ConvexPolygonBounds.cpp
        CylinderBounds.cpp
        CylinderSurface.cpp
        DiamondBounds.cpp
        DiscSurface.cpp
        DiscTrapezoidBounds.cpp
        EllipseBounds.cpp
        IntersectionHelper2D.cpp
        LineBounds.cpp
        LineSurface.cpp
        PerigeeSurface.cpp
        PlaneSurface.cpp
        RadialBounds.cpp
        RectangleBounds.cpp
        StrawSurface.cpp
        Surface.cpp
        SurfaceArray.cpp
        SurfaceBounds.cpp
        SurfaceError.cpp
        TrapezoidBounds.cpp
        detail/VerticesHelper.cpp
        RegularSurface.cpp
        CurvilinearSurface.cpp
        detail/AlignmentHelper.cpp
        detail/AnnulusBoundsHelper.cpp
        detail/MergeHelper.cpp
)
