# JIT CMakeLists.txt

set(elib_JIT_sources
"TJITCache.cp"
"TJITPage.cp"
#"Generic/TJITGenericRetarget.cp"
#"Generic/TJITGenericRetargetMap.cpp"
"Generic/TJITGenericROMPatch.cp"
"Generic/TJITGeneric_BlockDataTransfer.cp"
"Generic/TJITGeneric_DataProcessingPSRTransfer.cp"
"Generic/TJITGeneric_DataProcessingPSRTransfer_MRS.cp"
"Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp.cp"
"Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp.cp"
"Generic/TJITGeneric_DataProcessingPSRTransfer_MRS.cp"
"Generic/TJITGeneric_DataProcessingPSRTransfer_MSR.cp"
"Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp.cp"
"Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp.cp"
"Generic/TJITGeneric_Multiply.cp"
"Generic/TJITGeneric_MultiplyAndAccumulate.cp"
"Generic/TJITGeneric_Other.cp"
"Generic/TJITGeneric_SingleDataSwap.cp"
"Generic/TJITGeneric_SingleDataTransfer.cp"
"Generic/TJITGeneric_Other.cp"
"Generic/TJITGeneric_Test.cp"
)

set(elib_jit_generic_sources
"Generic/TJITGeneric.cp"
"Generic/TJITGenericPage.cp"
)

set(elib_jit_armle_sources
"ARMLE/TJITARMLE.cp"
"ARMLE/TJITARMLEPage.cp"
"ARMLE/JITARMLEGlue.s"
)

if(armlejit)
    set(elib_JIT_sources ${elib_JIT_sources} ${elib_jit_armle_sources})
else()
    set(elib_JIT_sources ${elib_JIT_sources} ${elib_jit_generic_sources})
endif()

set_source_files_properties(${elib_JIT_sources} PROPERTIES LANGUAGE CXX)

add_library(jit STATIC ${elib_JIT_sources})

target_include_directories(jit PUBLIC
    Generic/
    ../
    ../JIT
    ../JIT/Generic
    ../ROM
    ../../
    ../../Monitor
)

target_link_libraries(jit k)
