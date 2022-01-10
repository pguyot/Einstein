#
# Add optional source files required to buid the Toolkit for Einstein.
#

target_sources ( Einstein PRIVATE
	TToolkit.cpp
	TToolkit.h
	TTkElement.cpp
	TTkElement.h
	TFLToolkitUI.cpp
	TFLToolkitUI.h
	TFLToolkitUI.fl
	TTkProject.cpp
	TTkProject.h
	TTkScript.cpp
	TTkScript.h
	TFLInspectorPanel.cpp
	TFLInspectorPanel.h
	TFLTerminalPanel.cpp
	TFLTerminalPanel.h
	TFLScriptPanel.cpp
	TFLScriptPanel.h
	TToolkitPrototypes.cpp
	TToolkitPrototypes.h
	CMakeLists.txt
)

target_include_directories(Einstein SYSTEM PUBLIC 
	${NEWT64_INCLUDE_PATH}
)

target_link_libraries(Einstein
	${newt64_lib}
)

target_compile_definitions(Einstein PRIVATE USE_NEWT64=1)

