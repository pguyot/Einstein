#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	Emulator/JIT/Generic/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/JIT/Generic/IncludeMask.h
	Emulator/JIT/Generic/IncludeMaskRm.h
	Emulator/JIT/Generic/IncludeModeRd.h
	Emulator/JIT/Generic/IncludeModeRn.h
	Emulator/JIT/Generic/IncludeModeRnRd.h
	Emulator/JIT/Generic/IncludeRd.h
	Emulator/JIT/Generic/IncludeRdRm.h
	Emulator/JIT/Generic/IncludeRdRnRsRm15.h
	Emulator/JIT/Generic/IncludeRdRsRm15.h
	Emulator/JIT/Generic/IncludeRm.h
	Emulator/JIT/Generic/IncludeRm15.h
	Emulator/JIT/Generic/IncludeRn.h
	Emulator/JIT/Generic/IncludeRn15.h
	Emulator/JIT/Generic/IncludeRnRd.h
	Emulator/JIT/Generic/IncludeRnRdRm.h
	Emulator/JIT/Generic/IncludeRnRsRm15.h
	Emulator/JIT/Generic/IncludeRsRm15.h
	Emulator/JIT/Generic/RepeatMacros.h
	Emulator/JIT/Generic/TJITGeneric.cpp
	Emulator/JIT/Generic/TJITGeneric.h
	Emulator/JIT/Generic/TJITGenericPage.cpp
	Emulator/JIT/Generic/TJITGenericPage.h
	Emulator/JIT/Generic/TJITGenericROMPatch.cpp
	Emulator/JIT/Generic/TJITGenericROMPatch.h
	Emulator/JIT/Generic/TJITGeneric_BlockDataTransfer.cpp
	Emulator/JIT/Generic/TJITGeneric_BlockDataTransfer.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer.cpp
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp.cpp
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp_template.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp.cpp
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp_template.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MRS.cpp
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MRS.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MRS_template.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MSR.cpp
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MSR.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MSR_template.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp.cpp
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp_template.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp.cpp
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp_template.h
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_common.h
	Emulator/JIT/Generic/TJITGeneric_HalfwordAndSignedDataTransfer.h
	Emulator/JIT/Generic/TJITGeneric_HalfwordAndSignedDataTransferImm.cpp
	Emulator/JIT/Generic/TJITGeneric_HalfwordAndSignedDataTransferImm_template.t
	Emulator/JIT/Generic/TJITGeneric_HalfwordAndSignedDataTransferReg.cpp
	Emulator/JIT/Generic/TJITGeneric_HalfwordAndSignedDataTransferReg_template.t
	Emulator/JIT/Generic/TJITGeneric_HalfwordAndSignedDataTransfer_template.h
	Emulator/JIT/Generic/TJITGeneric_LDM1_template.h
	Emulator/JIT/Generic/TJITGeneric_LDM2_template.h
	Emulator/JIT/Generic/TJITGeneric_LDM3_template.h
	Emulator/JIT/Generic/TJITGeneric_Macros.h
	Emulator/JIT/Generic/TJITGeneric_Multiply.cpp
	Emulator/JIT/Generic/TJITGeneric_Multiply.h
	Emulator/JIT/Generic/TJITGeneric_MultiplyAndAccumulate.cpp
	Emulator/JIT/Generic/TJITGeneric_MultiplyAndAccumulate.h
	Emulator/JIT/Generic/TJITGeneric_Multiply_template.h
	Emulator/JIT/Generic/TJITGeneric_Other.cpp
	Emulator/JIT/Generic/TJITGeneric_Other.h
	Emulator/JIT/Generic/TJITGeneric_STM1_template.h
	Emulator/JIT/Generic/TJITGeneric_STM2_template.h
	Emulator/JIT/Generic/TJITGeneric_SingleDataSwap.cpp
	Emulator/JIT/Generic/TJITGeneric_SingleDataSwap.h
	Emulator/JIT/Generic/TJITGeneric_SingleDataSwap_template.h
	Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer.cpp
	Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer.h
	Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer_template.h
	Emulator/JIT/Generic/TJITGeneric_Test.cpp
	Emulator/JIT/Generic/TJITGeneric_Test.h
	Emulator/JIT/Generic/TJITGeneric_Test_template.h
)
