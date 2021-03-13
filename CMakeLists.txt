#
# Use a single CMake setup to compile Einstein on all supported platforms.
#
# Einstein has evolved over time into a well supported Xcode version,
# a much less supported MSWindows version, and a quite minimal Linux version.
# For the sake of maintainability, this branch of Einstein compiles using
# FLTK as its cross-platform GUI for all three desktop platforms.
#

cmake_minimum_required(VERSION 3.15)

project("Einstein" VERSION "2020.4.12")

set (CMAKE_CXX_STANDARD 17)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

	# MacOS
	set (CMAKE_OSX_ARCHITECTURES=x86_64)
	set (CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
    set (CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
    set (CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
	#set (CMAKE_PREFIX_PATH /usr/local)

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

	# Linux
	#set (CMAKE_PREFIX_PATH /usr/local)
	
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

	# MSWindows
	cmake_policy(SET CMP0091 NEW)
	set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

else ()

	# not supported
	message ("Einstein is not configured to build on this platform: " ${CMAKE_SYSTEM_NAME} ".")

endif ()

#
# ---- Newt/64 library ---------------------------------------------------------
#

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	find_package(Newt64 CONFIG) # -> Newt64_FOUND, Newt64_DIR
endif()
find_library(newt64_lib NAMES newt64 ) 
find_file(newt64_incl NAMES NewtCore.h PATH_SUFFIXES newt64) 

if (newt64_lib MATCHES ".*NOTFOUND" OR newt64_incl MATCHES ".*NOTFOUND")
	message(WARNING "Newt64 not found!")
	set ( USE_NEWT64 false )
else()
	set ( USE_NEWT64 true )
	get_filename_component(newt64_lib_path ${newt64_lib} DIRECTORY)
	get_filename_component(newt64_incl_path ${newt64_incl} DIRECTORY)
	message(STATUS "Newt64 found in " ${newt64_lib_path} )
endif()

#
# ---- Einstein Source Files -------------------------------------------------------------
#

set ( sources
	Emulator/Files/TFileManager.cpp
	Emulator/Files/TFileManager.h
	Emulator/Host/THostInfo.cpp
	Emulator/Host/THostInfo.h
	Emulator/Host/UserInfoDefinitions.h
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
	Emulator/JIT/JIT.h
	Emulator/JIT/TJIT.h
	Emulator/JIT/TJITCache.cpp
	Emulator/JIT/TJITCache.h
	Emulator/JIT/TJITPage.cpp
	Emulator/JIT/TJITPage.h
	Emulator/JIT/TJITPerformance.cpp
	Emulator/JIT/TJITPerformance.h
	Emulator/Log/TBufferLog.cpp
	Emulator/Log/TBufferLog.h
	Emulator/Log/TFileLog.cpp
	Emulator/Log/TFileLog.h
	Emulator/Log/TLog.cpp
	Emulator/Log/TLog.h
	Emulator/Log/TRAMLog.cpp
	Emulator/Log/TRAMLog.h
	Emulator/Log/TStdOutLog.cpp
	Emulator/Log/TStdOutLog.h
	Emulator/NativeCalls/TVirtualizedCalls.cpp
	Emulator/NativeCalls/TVirtualizedCalls.h
	Emulator/NativeCalls/TVirtualizedCallsPatches.cpp
	Emulator/NativeCalls/TVirtualizedCallsPatches.h
	Emulator/Serial/TTcpClientSerialPortManager.cpp
	Emulator/Serial/TTcpClientSerialPortManager.h
	Emulator/Network/TNetworkManager.cpp
	Emulator/Network/TNetworkManager.h
	Emulator/PCMCIA/TATACard.cpp
	Emulator/PCMCIA/TATACard.h
	Emulator/PCMCIA/TLinearCard.cpp
	Emulator/PCMCIA/TLinearCard.h
	Emulator/PCMCIA/TNE2000Card.cpp
	Emulator/PCMCIA/TNE2000Card.h
	Emulator/PCMCIA/TPCMCIACard.cpp
	Emulator/PCMCIA/TPCMCIACard.h
	Emulator/PCMCIA/TPCMCIAController.cpp
	Emulator/PCMCIA/TPCMCIAController.h
	Emulator/Platform/PlatformEvents.h
	Emulator/Platform/PlatformGestalt.h
	Emulator/Platform/TNewt.cpp
	Emulator/Platform/TNewt.h
	Emulator/Platform/TPlatformManager.cpp
	Emulator/Platform/TPlatformManager.h
	Emulator/ROM/TAIFFile.cpp
	Emulator/ROM/TAIFFile.h
	Emulator/ROM/TAIFROMImageWithREXes.cpp
	Emulator/ROM/TAIFROMImageWithREXes.h
	Emulator/ROM/TFlatROMImageWithREX.cpp
	Emulator/ROM/TFlatROMImageWithREX.h
	Emulator/ROM/TROMImage.cpp
	Emulator/ROM/TROMImage.h
	Emulator/Screen/TFLScreenManager.cpp
	Emulator/Screen/TFLScreenManager.h
	Emulator/Screen/TNullScreenManager.cpp
	Emulator/Screen/TNullScreenManager.h
	Emulator/Screen/TScreenManager.cpp
	Emulator/Screen/TScreenManager.h
	Emulator/Serial/TBasicSerialPortManager.cpp
	Emulator/Serial/TBasicSerialPortManager.h
	Emulator/Serial/TSerialPortManager.cpp
	Emulator/Serial/TSerialPortManager.h
	Emulator/Serial/TSerialPorts.cpp
	Emulator/Serial/TSerialPorts.h
	Emulator/Sound/TBufferedSoundManager.cpp
	Emulator/Sound/TBufferedSoundManager.h
	Emulator/Sound/TNullSoundManager.cpp
	Emulator/Sound/TNullSoundManager.h
	Emulator/Sound/TSoundManager.cpp
	Emulator/Sound/TSoundManager.h
	Emulator/TARMProcessor.cpp
	Emulator/TARMProcessor.h
	Emulator/TDMAManager.cpp
	Emulator/TDMAManager.h
	Emulator/TEmulator.cpp
	Emulator/TEmulator.h
	Emulator/TFlash.cpp
	Emulator/TFlash.h
	Emulator/THashMapCache.h
	Emulator/TInterruptManager.cpp
	Emulator/TInterruptManager.h
	Emulator/TMMU.cpp
	Emulator/TMMU.h
	Emulator/TMemory.cpp
	Emulator/TMemory.h
	Emulator/TMemoryConsts.h
	Emulator/TNativePrimitives.cpp
	Emulator/TNativePrimitives.h
	K/Defines/KDefinitions.cpp
	K/Defines/KDefinitions.h
	K/Defines/UByteSex.h
	K/Exceptions/Errors/TError.cpp
	K/Exceptions/Errors/TError.h
	K/Exceptions/Errors/TMemError.cpp
	K/Exceptions/Errors/TMemError.h
	K/Exceptions/IO/TEOFException.cpp
	K/Exceptions/IO/TEOFException.h
	K/Exceptions/IO/TIOException.cpp
	K/Exceptions/IO/TIOException.h
	K/Exceptions/TException.cpp
	K/Exceptions/TException.h
	K/Misc/CRC32.h
	K/Misc/CRC32.cpp
	K/Misc/RelocHack.h
	K/Misc/TCircleBuffer.cpp
	K/Misc/TCircleBuffer.h
	K/Misc/TDoubleLinkedList.h
	K/Misc/TMappedFile.cpp
	K/Misc/TMappedFile.h
	K/Streams/TFileStream.cpp
	K/Streams/TFileStream.h
	K/Streams/TRandomAccessStream.h
	K/Streams/TStream.cpp
	K/Streams/TStream.h
	K/Threads/TCondVar.cpp
	K/Threads/TCondVar.h
	K/Threads/TMutex.cpp
	K/Threads/TMutex.h
	K/Threads/TThread.cpp
	K/Threads/TThread.h
	K/Unicode/UUTF16CStr.cpp
	K/Unicode/UUTF16CStr.h
	K/Unicode/UUTF16Conv.cpp
	K/Unicode/UUTF16Conv.h
	K/Unicode/UnicodeDefinitions.h
	Monitor/TFLMonitor.cpp
	Monitor/TFLMonitor.h
	Monitor/TMonitor.cpp
	Monitor/TMonitor.h
	Monitor/TMonitorCore.cpp
	Monitor/TMonitorCore.h
	Monitor/TSymbolList.cpp
	Monitor/TSymbolList.h
	Monitor/UDisasm.cpp
	Monitor/UDisasm.h
	app/TFLApp.cpp
	app/TFLApp.h
	app/TFLAppUI.fl
	app/TFLAppUI.cpp
	app/TFLAppUI.h
	app/TFLRexImage.fl
	app/TFLRexImage.cpp
	app/TFLRexImage.h
	app/TFLSettingsUI.fl
	app/TFLSettingsUI.cpp
	app/TFLSettingsUI.h
	app/TFLSettings.cpp
	app/TFLSettings.h
	app/TFLAppWindow.cpp
	app/TFLAppWindow.h
	app/TPathHelper.cpp
	app/TPathHelper.h
	app/TFLTabs.cpp
	app/TFLTabs.h
	app/Version.h
	app/Version.h.in
)

#
# ---- Einstein Toolkit Sources ----------------------------------------------------------
#

if ( USE_NEWT64 MATCHES true )
	list (APPEND sources
		Toolkit/TToolkit.cpp
		Toolkit/TToolkit.h
		Toolkit/TTkElement.cpp
		Toolkit/TTkElement.h
		Toolkit/TFLToolkitUI.cpp
		Toolkit/TFLToolkitUI.h
		Toolkit/TFLToolkitUI.fl
		Toolkit/TTkProject.cpp
		Toolkit/TTkProject.h
		Toolkit/TTkScript.cpp
		Toolkit/TTkScript.h
		Toolkit/TFLInspectorPanel.cpp
		Toolkit/TFLInspectorPanel.h
		Toolkit/TFLTerminalPanel.cpp
		Toolkit/TFLTerminalPanel.h
		Toolkit/TFLScriptPanel.cpp
		Toolkit/TFLScriptPanel.h
		Toolkit/TToolkitPrototypes.cpp
		Toolkit/TToolkitPrototypes.h
	)
endif ()

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/app/Version.h PROPERTIES GENERATED TRUE)

string(TIMESTAMP COMPILE_TIME_YYYY "%Y")
string(TIMESTAMP COMPILE_TIME_MM "%m")
string(TIMESTAMP COMPILE_TIME_DD "%d")

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/app/Version.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/app/Version.h
)


macro (build_with_fluid name dir)
	add_custom_command (
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${dir}"
		COMMAND ${FLTK_FLUID_EXECUTABLE} -c ${name}.fl
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.fl"
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/app/Version.h"
		OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.cpp"
		OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.h"
	)
	set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.cpp PROPERTIES GENERATED TRUE)
	set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.h PROPERTIES GENERATED TRUE)
endmacro ()


if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

	# configure to use FLTK as the GUI
	set(FLTK_SKIP_OPENGL true)
	set(FLTK_FLUID_EXECUTABLE /usr/local/bin/fluid)
	find_package(FLTK REQUIRED NO_MODULE)

	build_with_fluid(TFLAppUI app)
	build_with_fluid(TFLRexImage app)
	build_with_fluid(TFLSettingsUI app)
	build_with_fluid(TFLToolkitUI Toolkit)

	# additional source code
	list (APPEND sources
		# calling ObjectiveC native functions (very limited support in this version of EInsten)
		Emulator/NativeCalls/TObjCBridgeCalls.mm
		Emulator/NativeCalls/TObjCBridgeCalls.h
		app/TCocoaPathHelper.mm
		app/TCocoaPathHelper.h
		# macOS sound support
		Emulator/Sound/TCoreAudioSoundManager.cpp
		Emulator/Sound/TCoreAudioSoundManager.h
		# Network card emulations
		Emulator/Network/TTapNetwork.cpp
		Emulator/Network/TTapNetwork.h
		Emulator/Network/TUsermodeNetwork.cpp
		Emulator/Network/TUsermodeNetwork.h
		# Serial port emulation
		Emulator/Serial/TBasiliskIISerialPortManager.cpp
		Emulator/Serial/TBasiliskIISerialPortManager.h
		Emulator/Serial/TPipesSerialPortManager.cpp
		Emulator/Serial/TPipesSerialPortManager.h
		Emulator/Serial/TPtySerialPortManager.cpp
		Emulator/Serial/TPtySerialPortManager.h
		# Apple macOS required resources
		Resources/macOS/Info.plist.in
		Resources/macOS/Einstein.icns
        Resources/macOS/Entitlements.plist      
	)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	source_group(TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES ${sources})

	# take care of the app icon
	set (MACOSX_BUNDLE_ICON_FILE Einstein.icns)
	set_source_files_properties(Resources/macOS/${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

	# create the application
	add_executable(Einstein MACOSX_BUNDLE
		${sources}
	)

	# how to compile and link
	target_compile_options(Einstein PUBLIC -Wall)
	target_compile_definitions(Einstein PRIVATE TARGET_UI_FLTK=1 NO_PORT_AUDIO NO_X11 TARGET_OS_OPENSTEP=1 TARGET_OS_MAC=1 NS_BLOCK_ASSERTIONS=1)
	target_link_libraries(Einstein
		# /usr/local/lib/libfltk.a
		fltk
		fltk_images
		fltk_png
		fltk_z
		pthread
		"-framework AddressBook"
		"-framework AudioUnit"
		"-framework AppKit"
		"-framework CoreAudio"
		"-framework Cocoa"
	)
	set_target_properties(Einstein PROPERTIES
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Resources/macOS/Info.plist.in"
	)
    set_target_properties(Einstein PROPERTIES
        CPACK_BUNDLE_APPLE_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/macOS/EntiXXXtlements.plist"
    )

	if ( USE_NEWT64 MATCHES true )
		target_include_directories(Einstein SYSTEM PUBLIC ${newt64_incl_path} ${newt64_incl_path}/darwin)
		target_link_libraries(Einstein
			${newt64_lib}
		)
		target_compile_definitions(Einstein PRIVATE USE_NEWT64=1)
	endif ()

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

	# configure to use FLTK as the GUI
	set(FLTK_SKIP_OPENGL true)
	set(FLTK_FLUID_EXECUTABLE /usr/local/bin/fluid)
	find_package(FLTK REQUIRED NO_MODULE)

	build_with_fluid(TFLAppUI app)
	build_with_fluid(TFLRexImage app)
	build_with_fluid(TFLSettingsUI app)
	build_with_fluid(TFLToolkitUI Toolkit)

	# additional source code
	list (APPEND sources
			# calling native functions (very limited support in this version of EInsten)
			Emulator/NativeCalls/NativeCallsDefines.h
			Emulator/NativeCalls/TNativeCalls.cpp
			Emulator/NativeCalls/TNativeCalls.h
			# Unix sound support
			Emulator/Sound/TPulseAudioSoundManager.cpp
			Emulator/Sound/TPulseAudioSoundManager.h
			# Network card emulations
			Emulator/Network/TUsermodeNetwork.cpp
			Emulator/Network/TUsermodeNetwork.h
			Emulator/Serial/TBasiliskIISerialPortManager.cpp
			Emulator/Serial/TBasiliskIISerialPortManager.h
			Emulator/Serial/TPipesSerialPortManager.cpp
			Emulator/Serial/TPipesSerialPortManager.h
			Emulator/Serial/TPtySerialPortManager.cpp
			Emulator/Serial/TPtySerialPortManager.h
			)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	source_group(TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES ${sources})

	# create the application
	add_executable(Einstein ${sources})

	# how to compile and link
	target_compile_options(Einstein PUBLIC -Wall -Wno-multichar)
	target_compile_definitions(Einstein PRIVATE TARGET_UI_FLTK=1 TARGET_OS_LINUX=1)
	target_link_libraries(Einstein
		fltk
		fltk_images
		fltk_png
		fltk_z
		pulse # sound
	)

	if ( USE_NEWT64 MATCHES true )
		target_include_directories(Einstein SYSTEM PUBLIC ${newt64_incl_path} ${newt64_incl_path}/darwin)
		target_link_libraries(Einstein
			${newt64_lib}
		)
		target_compile_definitions(Einstein PRIVATE USE_NEWT64=1)
	endif ()

elseif (WIN32)

	# configure to use FLTK as the GUI
	# In later versions of Windows 10, C:\Program Files\ is not writable for the user.
	# I used this command to create a symbolic link from within the Admin Command Prompt:
	#    cd "c:\Program Files"
	#    mklink /D FLTK "C:\Users\micro\dev\fltk-1.4 for Einstein.git\out\install\x64-Release"
	# Update the link to point to your build directory as needed
	set(FLTK_DIR "C:/Program Files/FLTK/CMake")
	# set(FLTK_FLUID_EXECUTABLE "C:/Program Files/FLTK/bin/fluid.exe")
	set(FLTK_SKIP_OPENGL true)
	find_package(FLTK REQUIRED NO_MODULE)

	build_with_fluid(TFLAppUI app)
	build_with_fluid(TFLRexImage app)
	build_with_fluid(TFLSettingsUI app)
	build_with_fluid(TFLToolkitUI Toolkit)

	# additional source code
	list (APPEND sources
		# calling native functions (very limited support in this version of EInsten)
		Emulator/NativeCalls/NativeCallsDefines.h
		Emulator/NativeCalls/TNativeCalls.cpp
		Emulator/NativeCalls/TNativeCalls.h
		# Windows sound support
		Emulator/Sound/TWaveSoundManager.cpp
		Emulator/Sound/TWaveSoundManager.h
		# Network card emulations
		Emulator/Network/TUsermodeNetwork.cpp
		Emulator/Network/TUsermodeNetwork.h
	)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	source_group(TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES ${sources} Resources/MSWindows/Einstein.rc.in)

	# Create a resources file for Windows
	configure_file(
		Resources/MSWindows/Einstein.rc.in
		Einstein.rc
	)

	# create the application
	add_executable(Einstein WIN32
		${sources} ${data}
		${CMAKE_CURRENT_BINARY_DIR}/Einstein.rc
	)

	# how to compile and link
	target_compile_options(Einstein PUBLIC "/bigobj")
	if ($<CONFIG:Debug>) 
		target_compile_options(Einstein PUBLIC "/ZI")
	endif()
	target_compile_definitions(Einstein PRIVATE TARGET_UI_FLTK=1 TARGET_OS_WIN32=1 WIN32_LEAN_AND_MEAN=1 _CRT_SECURE_NO_WARNINGS=1)
	target_link_libraries(Einstein
		# this vvv always includes the debug version, even in release mode
		fltk
		fltk_images
		fltk_png
		fltk_z
		# alternatively, this loads the correct library but needs additional libs that would be included automatically in the line above
		#"${FLTK_BASE_LIBRARY_RELEASE}" # FIXME: or it will select the debug version even in release mode!
		#comctl32
		#ws2_32
		# end of alternative
		winmm
	)

	if ( USE_NEWT64 MATCHES true )
		target_include_directories(Einstein SYSTEM PUBLIC ${newt64_incl_path} ${newt64_incl_path}/win)
		target_link_libraries(Einstein
			${newt64_lib}
			shlwapi
		)
		message(STATUS ${newt64_lib})
		target_compile_definitions(Einstein PRIVATE USE_NEWT64=1)
	endif ()

endif ()


target_include_directories(
	Einstein PUBLIC
	${CMAKE_SOURCE_DIR}
	${FLTK_INCLUDE_DIRS}
)


# nice code to output every know variable in this CMake session
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#	message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()


#
# --- MessagePad.org distribution system ---------------------------------------
#

set (EINSTEIN_DIST_DIR "Sites/messagepad.org/htdocs/Downloads/Einstein")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

	# MacOS
	add_custom_target(
		dist
		COMMAND ${CMAKE_COMMAND} -E tar "cfv" "/Users/matt/${EINSTEIN_DIST_DIR}/MacOS/Einstein.MacOS.${CMAKE_PROJECT_VERSION}.zip" --format=zip -- "Einstein.app"
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
    add_dependencies(dist Einstein)

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

	# Linux
	add_custom_target(
		dist
		COMMAND ${CMAKE_COMMAND} -E tar "cfv" "/media/psf/Home/${EINSTEIN_DIST_DIR}/Linux/Einstein.Linux.${CMAKE_PROJECT_VERSION}.zip" --format=zip -- "Einstein"
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
    add_dependencies(dist Einstein)

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

	# MSWindows
	add_custom_target(
		dist
		COMMAND ${CMAKE_COMMAND} -E tar "cfv" "z:/${EINSTEIN_DIST_DIR}/MSWindows/Einstein.Windows.${CMAKE_PROJECT_VERSION}.zip" --format=zip -- "Einstein.exe"
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
    add_dependencies(dist Einstein)

else ()

	# not supported
	message ("Einstein is not configured to distribute for this platform: " ${CMAKE_SYSTEM_NAME} ".")

endif ()

