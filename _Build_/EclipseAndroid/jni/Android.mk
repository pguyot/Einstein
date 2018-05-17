# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)/../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_CPP_EXTENSION := .cp

LOCAL_CFLAGS    := \
	-Wno-multichar \
	-DTARGET_OS_ANDROID=1 \
	-O3 

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/Emulator/ \
	$(LOCAL_PATH)/Emulator/ROM/ \
	$(LOCAL_PATH)/Emulator/Platform/ \
	$(LOCAL_PATH)/Emulator/NativeCalls/ \
	$(LOCAL_PATH)/Emulator/Host/ \
	$(LOCAL_PATH)/Emulator/Log/ \
	$(LOCAL_PATH)/Emulator/JIT/ \
	$(LOCAL_PATH)/Emulator/JIT/Generic/ \
	$(LOCAL_PATH)/Emulator/Network/ \
	$(LOCAL_PATH)/Emulator/PCMCIA/ \
	$(LOCAL_PATH)/Emulator/Serial/ \
	$(LOCAL_PATH)/Emulator/Sound/ \
	$(LOCAL_PATH)/Emulator/Screen/

LOCAL_LDLIBS    := -llog -ljnigraphics

LOCAL_MODULE    := einstein

LOCAL_SRC_FILES := \
	app/AndroidGlue.cp \
	app/TAndroidApp.cp \
	Emulator/Screen/TAndroidScreenManager.cp \
	Emulator/TARMProcessor.cp \
	Emulator/TDMAManager.cp \
	Emulator/TEmulator.cp \
	Emulator/TFlash.cp \
	Emulator/TInterruptManager.cp \
	Emulator/TMemory.cp \
	Emulator/TMMU.cp \
	Emulator/TNativePrimitives.cp \
	Emulator/Host/THostInfo.cp \
	Emulator/JIT/TJIT.cp \
	Emulator/JIT/TJITCache.cp \
	Emulator/JIT/TJITPage.cp \
	Emulator/JIT/TJITPerformance.cp \
	Emulator/Log/TBufferLog.cp \
	Emulator/Log/TFileLog.cp \
	Emulator/Log/TLog.cp \
	Emulator/Log/TStdOutLog.cp \
	Emulator/NativeCalls/TNativeCalls.cp \
	Emulator/NativeCalls/TVirtualizedCalls.cp \
	Emulator/Network/TNetworkManager.cp \
	Emulator/Network/TUsermodeNetwork.cp \
	Emulator/PCMCIA/TATACard.cp \
	Emulator/PCMCIA/TLinearCard.cp \
	Emulator/PCMCIA/TNE2000Card.cp \
	Emulator/PCMCIA/TPCMCIACard.cp \
	Emulator/PCMCIA/TPCMCIAController.cp \
	Emulator/Platform/TPlatformManager.cp \
	Emulator/ROM/TAIFFile.cp \
	Emulator/ROM/TAIFROMImageWithREXes.cp \
	Emulator/ROM/TFlatROMImage.cp \
	Emulator/ROM/TFlatROMImageWithREX.cp \
	Emulator/ROM/TROMImage.cp \
	Emulator/Screen/TScreenManager.cp \
	Emulator/Serial/TSerialPortManager.cp \
	Emulator/Sound/TBufferedSoundManager.cp \
	Emulator/Sound/TAndroidSoundManager.cp \
	Emulator/Sound/TSoundManager.cp \
	Monitor/TSymbolList.cp \
	K/Misc/TCircleBuffer.cp \
	K/Misc/TMappedFile.cp \
	K/Streams/TFileStream.cp \
	K/Streams/TStream.cp \
	K/Threads/TCondVar.cp \
	K/Threads/TMutex.cp \
	K/Threads/TThread.cp \
	K/Unicode/UUTF16Conv.cp \
	K/Unicode/UUTF16CStr.cp \
	Emulator/JIT/Generic/TJITGeneric.cp \
	Emulator/JIT/Generic/TJITGeneric_BlockDataTransfer.cp \
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer.cp \
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp.cp \
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp.cp \
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp.cp \
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MRS.cp \
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MSR.cp \
	Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp.cp \
	Emulator/JIT/Generic/TJITGeneric_Multiply.cp \
	Emulator/JIT/Generic/TJITGeneric_MultiplyAndAccumulate.cp \
	Emulator/JIT/Generic/TJITGeneric_Other.cp \
	Emulator/JIT/Generic/TJITGeneric_SingleDataSwap.cp \
	Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer.cp \
	Emulator/JIT/Generic/TJITGeneric_Test.cp \
	Emulator/JIT/Generic/TJITGenericPage.cp

include $(BUILD_SHARED_LIBRARY)
