cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_physx_fastxml ../../../../../../../../PhysX/physx/source/fastxml/ )
set( DIR_physx_foundation ../../../../../../../../PhysX/physx/source/foundation/ )
set( DIR_physx_lowleveldynamics ../../../../../../../../PhysX/physx/source/lowleveldynamics/ )
set( DIR_physx_lowlevelaabb ../../../../../../../../PhysX/physx/source/lowlevelaabb/ )
set( DIR_physx_lowlevel_api ../../../../../../../../PhysX/physx/source/lowlevel/api/ )
set( DIR_physx_lowlevel_api_common ../../../../../../../../PhysX/physx/source/lowlevel/common/ )
set( DIR_physx_lowlevel_api_software ../../../../../../../../PhysX/physx/source/lowlevel/software/ )
set( DIR_physx_scenequery ../../../../../../../../PhysX/physx/source/scenequery/ )
set( DIR_physx_common ../../../../../../../../PhysX/physx/source/common/ )
set( DIR_physx_geomutils ../../../../../../../../PhysX/physx/source/geomutils/ )
set( DIR_physx_physxmetadata ../../../../../../../../PhysX/physx/source/physxmetadata/core )
set( DIR_physx_physx ../../../../../../../../PhysX/physx/source/physx/ )
set( DIR_physx_characterkinematic ../../../../../../../../PhysX/physx/source/physxcharacterkinematic/ )
set( DIR_physx_physxextensions ../../../../../../../../PhysX/physx/source/physxextensions/ )
set( DIR_physx_simulationcontroller ../../../../../../../../PhysX/physx/source/simulationcontroller/ )
set( DIR_physx_task ../../../../../../../../PhysX/physx/source/task/ )


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG=1 -Wall -Wextra -Wpedantic -std=c++11 -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/../pxshared/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/foundation/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/fastxml/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxgpu/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/contact")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/common")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/convex")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/distance")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/sweep")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/gjk")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/intersection")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/mesh")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/hf")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/pcm")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/ccd")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/api/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/collision")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/pipeline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/utils")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevelaabb/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/software/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowleveldynamics/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/cudamanager/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/scenequery/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/simulationcontroller/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/simulationcontroller/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physx/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physx/src/buffering")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/pvd/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxcooking/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxcooking/src/mesh")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxcooking/src/convex")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxmetadata/core/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/immediatemode/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxmetadata/core/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxmetadata/extensions/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxextensions/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxextensions/src/serialization/Xml")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxextensions/src/serialization/Binary")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxextensions/src/serialization/File")


add_library(physx SHARED
			${DIR_physx_fastxml}/src/PsFastXml.cpp

			${DIR_physx_foundation}/src/PsAllocator.cpp
			${DIR_physx_foundation}/src/PsAssert.cpp
			${DIR_physx_foundation}/src/PsFoundation.cpp
			${DIR_physx_foundation}/src/PsMathUtils.cpp
			${DIR_physx_foundation}/src/PsString.cpp
			${DIR_physx_foundation}/src/PsTempAllocator.cpp
			${DIR_physx_foundation}/src/PsUtilities.cpp

			${DIR_physx_foundation}/src/unix/PsUnixAtomic.cpp
			${DIR_physx_foundation}/src/unix/PsUnixCpu.cpp
			${DIR_physx_foundation}/src/unix/PsUnixFPU.cpp
			${DIR_physx_foundation}/src/unix/PsUnixMutex.cpp
			${DIR_physx_foundation}/src/unix/PsUnixPrintString.cpp
			${DIR_physx_foundation}/src/unix/PsUnixSList.cpp
			${DIR_physx_foundation}/src/unix/PsUnixSocket.cpp
			${DIR_physx_foundation}/src/unix/PsUnixSync.cpp
			${DIR_physx_foundation}/src/unix/PsUnixThread.cpp
			${DIR_physx_foundation}/src/unix/PsUnixTime.cpp

			${DIR_physx_task}/src/TaskManager.cpp

			${DIR_physx_simulationcontroller}/src/ScActorCore.cpp
			${DIR_physx_simulationcontroller}/src/ScActorSim.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationCore.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationJointCore.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationJointSim.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationSim.cpp
			${DIR_physx_simulationcontroller}/src/ScBodyCore.cpp
			${DIR_physx_simulationcontroller}/src/ScBodySim.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintCore.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintGroupNode.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintInteraction.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintProjectionManager.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintProjectionTree.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintSim.cpp
			${DIR_physx_simulationcontroller}/src/ScElementInteractionMarker.cpp
			${DIR_physx_simulationcontroller}/src/ScElementSim.cpp
			${DIR_physx_simulationcontroller}/src/ScInteraction.cpp
			${DIR_physx_simulationcontroller}/src/ScIterators.cpp
			${DIR_physx_simulationcontroller}/src/ScMetaData.cpp
			${DIR_physx_simulationcontroller}/src/ScNPhaseCore.cpp
			${DIR_physx_simulationcontroller}/src/ScPhysics.cpp
			${DIR_physx_simulationcontroller}/src/ScRigidCore.cpp
			${DIR_physx_simulationcontroller}/src/ScRigidSim.cpp
			${DIR_physx_simulationcontroller}/src/ScScene.cpp

			${DIR_physx_simulationcontroller}/src/ScActorCore.cpp
			${DIR_physx_simulationcontroller}/src/ScActorSim.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationCore.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationJointCore.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationJointSim.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationSim.cpp
			${DIR_physx_simulationcontroller}/src/ScBodyCore.cpp
			${DIR_physx_simulationcontroller}/src/ScBodySim.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintCore.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintGroupNode.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintInteraction.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintProjectionManager.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintProjectionTree.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintSim.cpp
			${DIR_physx_simulationcontroller}/src/ScElementInteractionMarker.cpp
			${DIR_physx_simulationcontroller}/src/ScElementSim.cpp
			${DIR_physx_simulationcontroller}/src/ScInteraction.cpp
			${DIR_physx_simulationcontroller}/src/ScIterators.cpp
			${DIR_physx_simulationcontroller}/src/ScMetaData.cpp
			${DIR_physx_simulationcontroller}/src/ScNPhaseCore.cpp
			${DIR_physx_simulationcontroller}/src/ScPhysics.cpp
			${DIR_physx_simulationcontroller}/src/ScRigidCore.cpp
			${DIR_physx_simulationcontroller}/src/ScRigidSim.cpp
			${DIR_physx_simulationcontroller}/src/ScScene.cpp
			${DIR_physx_simulationcontroller}/src/ScShapeCore.cpp
			${DIR_physx_simulationcontroller}/src/ScShapeInteraction.cpp
			${DIR_physx_simulationcontroller}/src/ScShapeSim.cpp
			${DIR_physx_simulationcontroller}/src/ScSimStats.cpp
			${DIR_physx_simulationcontroller}/src/ScSimulationController.cpp
			${DIR_physx_simulationcontroller}/src/ScSqBoundsManager.cpp
			${DIR_physx_simulationcontroller}/src/ScStaticCore.cpp
			${DIR_physx_simulationcontroller}/src/ScTriggerInteraction.cpp

			${DIR_physx_scenequery}/src/SqAABBPruner.cpp
			${DIR_physx_scenequery}/src/SqAABBTree.cpp
			${DIR_physx_scenequery}/src/SqAABBTreeUpdateMap.cpp
			${DIR_physx_scenequery}/src/SqBounds.cpp
			${DIR_physx_scenequery}/src/SqBucketPruner.cpp
			${DIR_physx_scenequery}/src/SqCompoundPruner.cpp
			${DIR_physx_scenequery}/src/SqCompoundPruningPool.cpp
			${DIR_physx_scenequery}/src/SqExtendedBucketPruner.cpp
			${DIR_physx_scenequery}/src/SqIncrementalAABBPruner.cpp
			${DIR_physx_scenequery}/src/SqIncrementalAABBPrunerCore.cpp
			${DIR_physx_scenequery}/src/SqIncrementalAABBTree.cpp
			${DIR_physx_scenequery}/src/SqMetaData.cpp
			${DIR_physx_scenequery}/src/SqPruningPool.cpp
			${DIR_physx_scenequery}/src/SqPruningStructure.cpp
			${DIR_physx_scenequery}/src/SqSceneQueryManager.cpp

			${DIR_physx_lowleveldynamics}/src/DyArticulation.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationContactPrep.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationContactPrepPF.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationHelper.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationScalar.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationSIMD.cpp
			${DIR_physx_lowleveldynamics}/src/DyConstraintPartition.cpp
			${DIR_physx_lowleveldynamics}/src/DyConstraintSetup.cpp
			${DIR_physx_lowleveldynamics}/src/DyConstraintSetupBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DyContactPrep.cpp
			${DIR_physx_lowleveldynamics}/src/DyContactPrep4.cpp
			${DIR_physx_lowleveldynamics}/src/DyContactPrep4PF.cpp
			${DIR_physx_lowleveldynamics}/src/DyContactPrepPF.cpp
			${DIR_physx_lowleveldynamics}/src/DyDynamics.cpp
			${DIR_physx_lowleveldynamics}/src/DyFeatherstoneArticulation.cpp
			${DIR_physx_lowleveldynamics}/src/DyFeatherstoneForwardDynamic.cpp
			${DIR_physx_lowleveldynamics}/src/DyFeatherstoneInverseDynamic.cpp
			${DIR_physx_lowleveldynamics}/src/DyFrictionCorrelation.cpp
			${DIR_physx_lowleveldynamics}/src/DyRigidBodyToSolverBody.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverConstraintsBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverConstraints.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverConstraintsBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverControl.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverControlPF.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverPFConstraints.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverPFConstraintsBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DyTGSContactPrep.cpp
			${DIR_physx_lowleveldynamics}/src/DyTGSContactPrepBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DyTGSDynamics.cpp
			${DIR_physx_lowleveldynamics}/src/DyThreadContext.cpp
			${DIR_physx_lowleveldynamics}/src/DyThresholdTable.cpp

			${DIR_physx_lowlevelaabb}/src/BpAABBManager.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhase.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseABP.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseMBP.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseSap.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseSapAux.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseShared.cpp
			${DIR_physx_lowlevelaabb}/src/BpMBPTasks.cpp
			${DIR_physx_lowlevelaabb}/src/BpSAPTasks.cpp

			${DIR_physx_lowlevel_api}/src/px_globals.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcContactCache.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcContactMethodImpl.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcMaterialHeightField.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcMaterialMesh.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcMaterialMethodImpl.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcMaterialShape.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpBatch.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpCacheStreamPair.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpContactPrepShared.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpMemBlockPool.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpThreadContext.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsCCD.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsContactManager.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsContext.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsDefaultMemoryManager.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsIslandSim.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsMaterialCombiner.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsNphaseImplementationContext.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsSimpleIslandManager.cpp

			${DIR_physx_common}/src/CmCollection.cpp
			${DIR_physx_common}/src/CmMathUtils.cpp
			${DIR_physx_common}/src/CmPtrTable.cpp
			${DIR_physx_common}/src/CmRadixSort.cpp
			${DIR_physx_common}/src/CmRadixSortBuffered.cpp
			${DIR_physx_common}/src/CmRenderOutput.cpp
			${DIR_physx_common}/src/CmVisualization.cpp
			
			${DIR_physx_geomutils}/src/GuAABBTreeBuild.cpp
			${DIR_physx_geomutils}/src/GuBounds.cpp
			${DIR_physx_geomutils}/src/GuBox.cpp
			${DIR_physx_geomutils}/src/GuBVHStructure.cpp
			${DIR_physx_geomutils}/src/GuCapsule.cpp
			${DIR_physx_geomutils}/src/GuCCTSweepTests.cpp
			${DIR_physx_geomutils}/src/GuGeometryQuery.cpp
			${DIR_physx_geomutils}/src/GuGeometryUnion.cpp
			${DIR_physx_geomutils}/src/GuInternal.cpp
			${DIR_physx_geomutils}/src/GuMeshFactory.cpp
			${DIR_physx_geomutils}/src/GuMetaData.cpp
			${DIR_physx_geomutils}/src/GuMTD.cpp
			${DIR_physx_geomutils}/src/GuOverlapTests.cpp
			${DIR_physx_geomutils}/src/GuRaycastTests.cpp
			${DIR_physx_geomutils}/src/GuSerialize.cpp
			${DIR_physx_geomutils}/src/GuSweepMTD.cpp
			${DIR_physx_geomutils}/src/GuSweepSharedTests.cpp
			${DIR_physx_geomutils}/src/GuSweepTests.cpp

			${DIR_physx_geomutils}/src/ccd/GuCCDSweepConvexMesh.cpp
			${DIR_physx_geomutils}/src/ccd/GuCCDSweepPrimitives.cpp

			${DIR_physx_geomutils}/src/common/GuBarycentricCoordinates.cpp
			${DIR_physx_geomutils}/src/common/GuSeparatingAxes.cpp

			${DIR_physx_geomutils}/src/contact/GuContactBoxBox.cpp
			${DIR_physx_geomutils}/src/contact/GuContactCapsuleBox.cpp
			${DIR_physx_geomutils}/src/contact/GuContactCapsuleCapsule.cpp
			${DIR_physx_geomutils}/src/contact/GuContactCapsuleConvex.cpp
			${DIR_physx_geomutils}/src/contact/GuContactCapsuleMesh.cpp
			${DIR_physx_geomutils}/src/contact/GuContactConvexConvex.cpp
			${DIR_physx_geomutils}/src/contact/GuContactConvexMesh.cpp
			${DIR_physx_geomutils}/src/contact/GuContactPlaneBox.cpp
			${DIR_physx_geomutils}/src/contact/GuContactPlaneCapsule.cpp
			${DIR_physx_geomutils}/src/contact/GuContactPlaneConvex.cpp
			${DIR_physx_geomutils}/src/contact/GuContactPolygonPolygon.cpp
			${DIR_physx_geomutils}/src/contact/GuContactSphereBox.cpp
			${DIR_physx_geomutils}/src/contact/GuContactSphereCapsule.cpp
			${DIR_physx_geomutils}/src/contact/GuContactSphereMesh.cpp
			${DIR_physx_geomutils}/src/contact/GuContactSpherePlane.cpp
			${DIR_physx_geomutils}/src/contact/GuContactSphereSphere.cpp
			${DIR_physx_geomutils}/src/contact/GuFeatureCode.cpp

			${DIR_physx_geomutils}/src/convex/GuBigConvexData.cpp
			${DIR_physx_geomutils}/src/convex/GuBigConvexData.cpp
			${DIR_physx_geomutils}/src/convex/GuConvexHelper.cpp
			${DIR_physx_geomutils}/src/convex/GuConvexMesh.cpp
			${DIR_physx_geomutils}/src/convex/GuConvexSupportTable.cpp
			${DIR_physx_geomutils}/src/convex/GuConvexUtilsInternal.cpp
			${DIR_physx_geomutils}/src/convex/GuHillClimbing.cpp
			${DIR_physx_geomutils}/src/convex/GuShapeConvex.cpp

			${DIR_physx_geomutils}/src/distance/GuDistancePointBox.cpp
			${DIR_physx_geomutils}/src/distance/GuDistancePointBox.cpp
			${DIR_physx_geomutils}/src/distance/GuDistancePointTriangle.cpp
			${DIR_physx_geomutils}/src/distance/GuDistanceSegmentBox.cpp
			${DIR_physx_geomutils}/src/distance/GuDistanceSegmentSegment.cpp
			${DIR_physx_geomutils}/src/distance/GuDistanceSegmentTriangle.cpp

			${DIR_physx_geomutils}/src/gjk/GuEPA.cpp
			${DIR_physx_geomutils}/src/gjk/GuGJKSimplex.cpp
			${DIR_physx_geomutils}/src/gjk/GuGJKTest.cpp

			${DIR_physx_geomutils}/src/hf/GuHeightField.cpp
			${DIR_physx_geomutils}/src/hf/GuHeightFieldUtil.cpp
			${DIR_physx_geomutils}/src/hf/GuOverlapTestsHF.cpp
			${DIR_physx_geomutils}/src/hf/GuSweepsHF.cpp

			${DIR_physx_geomutils}/src/intersection/GuIntersectionBoxBox.cpp
			${DIR_physx_geomutils}/src/intersection/GuIntersectionCapsuleTriangle.cpp
			${DIR_physx_geomutils}/src/intersection/GuIntersectionEdgeEdge.cpp
			${DIR_physx_geomutils}/src/intersection/GuIntersectionRayBox.cpp
			${DIR_physx_geomutils}/src/intersection/GuIntersectionRayCapsule.cpp
			${DIR_physx_geomutils}/src/intersection/GuIntersectionRaySphere.cpp
			${DIR_physx_geomutils}/src/intersection/GuIntersectionSphereBox.cpp
			${DIR_physx_geomutils}/src/intersection/GuIntersectionTriangleBox.cpp

			${DIR_physx_geomutils}/src/mesh/GuBV32.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV32Build.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4Build.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4_AABBSweep.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4_BoxOverlap.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4_CapsuleSweep.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4_CapsuleSweepAA.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4_OBBSweep.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4_Raycast.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4_SphereOverlap.cpp
			${DIR_physx_geomutils}/src/mesh/GuBV4_SphereSweep.cpp
			${DIR_physx_geomutils}/src/mesh/GuMeshQuery.cpp
			${DIR_physx_geomutils}/src/mesh/GuMidphaseBV4.cpp
			${DIR_physx_geomutils}/src/mesh/GuMidphaseRTree.cpp
			${DIR_physx_geomutils}/src/mesh/GuOverlapTestsMesh.cpp
			${DIR_physx_geomutils}/src/mesh/GuRTree.cpp
			${DIR_physx_geomutils}/src/mesh/GuRTreeQueries.cpp
			${DIR_physx_geomutils}/src/mesh/GuSweepsMesh.cpp
			${DIR_physx_geomutils}/src/mesh/GuTriangleMesh.cpp
			${DIR_physx_geomutils}/src/mesh/GuTriangleMeshBV4.cpp
			${DIR_physx_geomutils}/src/mesh/GuTriangleMeshRTree.cpp

			${DIR_physx_geomutils}/src/pcm/GuPCMContactBoxBox.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactBoxBox.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactBoxConvex.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactCapsuleBox.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactCapsuleCapsule.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactCapsuleConvex.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactCapsuleHeightField.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactCapsuleMesh.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactConvexCommon.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactConvexConvex.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactConvexHeightField.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactConvexMesh.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactGenBoxConvex.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactGenSphereCapsule.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactPlaneBox.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactPlaneCapsule.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactPlaneConvex.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactSphereBox.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactSphereCapsule.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactSphereConvex.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactSphereHeightField.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactSphereMesh.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactSpherePlane.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMContactSphereSphere.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMShapeConvex.cpp
			${DIR_physx_geomutils}/src/pcm/GuPCMTriangleContactGen.cpp
			${DIR_physx_geomutils}/src/pcm/GuPersistentContactManifold.cpp

			${DIR_physx_geomutils}/src/sweep/GuSweepBoxBox.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepBoxBox.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepBoxSphere.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepBoxTriangle_FeatureBased.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepBoxTriangle_SAT.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepCapsuleBox.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepCapsuleCapsule.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepCapsuleTriangle.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepSphereCapsule.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepSphereSphere.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepSphereTriangle.cpp
			${DIR_physx_geomutils}/src/sweep/GuSweepTriangleUtils.cpp

			${DIR_physx_physxmetadata}/src/PxAutoGeneratedMetaDataObjects.cpp
			${DIR_physx_physxmetadata}/src/PxMetaDataObjects.cpp

			${DIR_physx_physx}/src/NpActor.cpp
			${DIR_physx_physx}/src/NpAggregate.cpp
			${DIR_physx_physx}/src/NpArticulation.cpp
			${DIR_physx_physx}/src/NpArticulationJoint.cpp
			${DIR_physx_physx}/src/NpArticulationJointReducedCoordinate.cpp
			${DIR_physx_physx}/src/NpArticulationLink.cpp
			${DIR_physx_physx}/src/NpArticulationReducedCoordinate.cpp
			${DIR_physx_physx}/src/NpBatchQuery.cpp
			${DIR_physx_physx}/src/NpConstraint.cpp
			${DIR_physx_physx}/src/NpFactory.cpp
			${DIR_physx_physx}/src/NpMaterial.cpp
			${DIR_physx_physx}/src/NpMetaData.cpp
			${DIR_physx_physx}/src/NpPhysics.cpp
			${DIR_physx_physx}/src/NpPvdSceneQueryCollector.cpp
			${DIR_physx_physx}/src/NpReadCheck.cpp
			${DIR_physx_physx}/src/NpRigidDynamic.cpp
			${DIR_physx_physx}/src/NpRigidStatic.cpp
			${DIR_physx_physx}/src/NpScene.cpp
			${DIR_physx_physx}/src/NpSceneQueries.cpp
			${DIR_physx_physx}/src/NpSerializerAdapter.cpp
			${DIR_physx_physx}/src/NpShape.cpp
			${DIR_physx_physx}/src/NpShapeManager.cpp
			${DIR_physx_physx}/src/NpWriteCheck.cpp
			${DIR_physx_physx}/src/PvdMetaDataPvdBinding.cpp
			${DIR_physx_physx}/src/PvdPhysicsClient.cpp

			${DIR_physx_physx}/src/buffering/ScbActor.cpp
			${DIR_physx_physx}/src/buffering/ScbAggregate.cpp
			${DIR_physx_physx}/src/buffering/ScbBase.cpp
			${DIR_physx_physx}/src/buffering/ScbMetaData.cpp
			${DIR_physx_physx}/src/buffering/ScbScene.cpp
			${DIR_physx_physx}/src/buffering/ScbScenePvdClient.cpp
			${DIR_physx_physx}/src/buffering/ScbShape.cpp

			${DIR_physx_physx}/src/gpu/PxGpu.cpp
			${DIR_physx_physx}/src/gpu/PxPhysXGpuModuleLoader.cpp

			${DIR_physx_characterkinematic}/src/CctBoxController.cpp
			${DIR_physx_characterkinematic}/src/CctCapsuleController.cpp
			${DIR_physx_characterkinematic}/src/CctCharacterController.cpp
			${DIR_physx_characterkinematic}/src/CctCharacterControllerCallbacks.cpp
			${DIR_physx_characterkinematic}/src/CctCharacterControllerManager.cpp
			${DIR_physx_characterkinematic}/src/CctController.cpp
			${DIR_physx_characterkinematic}/src/CctObstacleContext.cpp
			${DIR_physx_characterkinematic}/src/CctSweptBox.cpp
			${DIR_physx_characterkinematic}/src/CctSweptCapsule.cpp
			${DIR_physx_characterkinematic}/src/CctSweptVolume.cpp

			${DIR_physx}/source/physxmetadata/extensions/src/PxExtensionAutoGeneratedMetaDataObjects.cpp

			${DIR_physx_physxextensions}/src/ExtBroadPhase.cpp
			${DIR_physx_physxextensions}/src/ExtCollection.cpp
			${DIR_physx_physxextensions}/src/ExtContactJoint.cpp
			${DIR_physx_physxextensions}/src/ExtConvexMeshExt.cpp
			${DIR_physx_physxextensions}/src/ExtCpuWorkerThread.cpp
			${DIR_physx_physxextensions}/src/ExtD6Joint.cpp
			${DIR_physx_physxextensions}/src/ExtD6JointCreate.cpp
			${DIR_physx_physxextensions}/src/ExtDefaultCpuDispatcher.cpp
			${DIR_physx_physxextensions}/src/ExtDefaultErrorCallback.cpp
			${DIR_physx_physxextensions}/src/ExtDefaultSimulationFilterShader.cpp
			${DIR_physx_physxextensions}/src/ExtDefaultStreams.cpp
			${DIR_physx_physxextensions}/src/ExtDistanceJoint.cpp
			${DIR_physx_physxextensions}/src/ExtExtensions.cpp
			${DIR_physx_physxextensions}/src/ExtFixedJoint.cpp
			${DIR_physx_physxextensions}/src/ExtJoint.cpp
			${DIR_physx_physxextensions}/src/ExtMetaData.cpp
			${DIR_physx_physxextensions}/src/ExtPrismaticJoint.cpp
			${DIR_physx_physxextensions}/src/ExtPvd.cpp
			${DIR_physx_physxextensions}/src/ExtPxStringTable.cpp
			${DIR_physx_physxextensions}/src/ExtRaycastCCD.cpp
			${DIR_physx_physxextensions}/src/ExtRevoluteJoint.cpp
			${DIR_physx_physxextensions}/src/ExtRigidActorExt.cpp
			${DIR_physx_physxextensions}/src/ExtRigidBodyExt.cpp
			${DIR_physx_physxextensions}/src/ExtSceneQueryExt.cpp
			${DIR_physx_physxextensions}/src/ExtSimpleFactory.cpp
			${DIR_physx_physxextensions}/src/ExtSmoothNormals.cpp
			${DIR_physx_physxextensions}/src/ExtSphericalJoint.cpp
			${DIR_physx_physxextensions}/src/ExtTriangleMeshExt.cpp
        
			${DIR_physx_physxextensions}/src/serialization/SnSerialization.cpp
			${DIR_physx_physxextensions}/src/serialization/SnSerializationRegistry.cpp
			${DIR_physx_physxextensions}/src/serialization/SnSerialUtils.cpp

			${DIR_physx_physxextensions}/src/serialization/Binary/SnBinaryDeserialization.cpp
			${DIR_physx_physxextensions}/src/serialization/Binary/SnBinarySerialization.cpp
			${DIR_physx_physxextensions}/src/serialization/Binary/SnConvX.cpp
			${DIR_physx_physxextensions}/src/serialization/Binary/SnConvX_Align.cpp
			${DIR_physx_physxextensions}/src/serialization/Binary/SnConvX_Convert.cpp
			${DIR_physx_physxextensions}/src/serialization/Binary/SnConvX_Error.cpp
			${DIR_physx_physxextensions}/src/serialization/Binary/SnConvX_MetaData.cpp
			${DIR_physx_physxextensions}/src/serialization/Binary/SnConvX_Output.cpp
			${DIR_physx_physxextensions}/src/serialization/Binary/SnConvX_Union.cpp
			${DIR_physx_physxextensions}/src/serialization/Binary/SnSerializationContext.cpp

			${DIR_physx_physxextensions}/src/serialization/Xml/SnJointRepXSerializer.cpp
			${DIR_physx_physxextensions}/src/serialization/Xml/SnRepXCoreSerializer.cpp
			${DIR_physx_physxextensions}/src/serialization/Xml/SnRepXUpgrader.cpp
			${DIR_physx_physxextensions}/src/serialization/Xml/SnXmlSerialization.cpp
			
            )

target_link_libraries(physx
            android
            log
            m)
