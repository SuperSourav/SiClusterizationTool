################################################################################
# Package: SiClusterizationTool
################################################################################

# Declare the package name:
atlas_subdir( SiClusterizationTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthenaKernel
   DetectorDescription/GeoPrimitives
   DetectorDescription/Identifier
   Event/EventPrimitives
   GaudiKernel
   InnerDetector/InDetRawEvent/InDetSimData
   InnerDetector/InDetConditions/InDetBeamSpotService
   InnerDetector/InDetConditions/InDetConditionsSummaryService
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetRawEvent/InDetRawData
   InnerDetector/InDetRecEvent/InDetPrepRawData
   InnerDetector/InDetRecTools/InDetRecToolInterfaces
   Tracking/TrkEvent/TrkParameters
   PRIVATE
   Database/AthenaPOOL/AthenaPoolUtilities
   DetectorDescription/AtlasDetDescr
   DetectorDescription/DetDescrCond/DetDescrCondTools
   InnerDetector/InDetConditions/PixelConditionsServices
   InnerDetector/InDetDetDescr/PixelGeoModel
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/VxVertex
   Tracking/TrkUtilityPackages/TrkNeuralNetworkUtils )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_library( SiClusterizationToolLib
   SiClusterizationTool/*.h src/*.cxx
   PUBLIC_HEADERS SiClusterizationTool
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives
   Identifier EventPrimitives GaudiKernel InDetSimData InDetIdentifier
   InDetReadoutGeometry InDetRawData InDetPrepRawData InDetRecToolInterfaces
   TrkParameters TrkNeuralNetworkUtilsLib
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaPoolUtilities AtlasDetDescr
   TrkSurfaces TrkEventPrimitives VxVertex PixelGeoModelLib )

atlas_add_component( SiClusterizationTool
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel SiClusterizationToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
