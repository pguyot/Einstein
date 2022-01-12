#
# Add optional source files required to build the Toolkit for Einstein.
#

list ( APPEND cmake_sources
  Toolkit/CMakeLists.txt
)

list (APPEND app_sources
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
