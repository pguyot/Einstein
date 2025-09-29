#
# Add source files required to build the FLTK variant of Einstein.
#

list ( APPEND cmake_sources
	app/SDL/CMakeLists.txt
)

list ( APPEND common_sources
)

list ( APPEND app_sources
	app/SDL/TSDLApp.cpp
	app/SDL/TSDLApp.h
	app/SDL/TSDLRexImage.cpp
)

if ( ANDROID )
list ( APPEND app_sources
	app/SDL/TSDLAndroid.cpp
	app/SDL/TSDLAndroid.h
)
endif ()


function(generate_embedded_file resource_file_name source_file_name variable_name)
    file(READ ${resource_file_name} hex_content HEX)
    string(REPEAT "[0-9a-f]" 32 column_pattern)
    string(REGEX REPLACE "(${column_pattern})" "\\1\n" content "${hex_content}")
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " content "${content}")
    string(REGEX REPLACE ", $" "" content "${content}")
    set(array_extern "extern const unsigned char ${variable_name}[];\n")
    set(array_definition "const unsigned char ${variable_name}[] =\n{\n${content}\n};")
    set(source "// Auto generated file.\n${array_extern}${array_definition}\n\nunsigned int ${variable_name}Size = sizeof(${variable_name});\n\n")
    file(WRITE "${source_file_name}.cpp" "${source}")
endfunction()

function ( embed_resource resource_file_name source_file_name variable_name )
	set_source_files_properties (
		${CMAKE_SOURCE_DIR}/${source_file_name}.cpp
		PROPERTIES GENERATED TRUE
	)
    if (    NOT EXISTS ${CMAKE_SOURCE_DIR}/${source_file_name}
         OR ${CMAKE_SOURCE_DIR}/${resource_file_name} IS_NEWER_THAN ${CMAKE_SOURCE_DIR}/${source_file_name}
       )
		generate_embedded_file ( ${resource_file_name} ${source_file_name} ${variable_name} )
	endif ()
endfunction ()


embed_resource ( _Data_/Einstein.rex app/SDL/TSDLRexImage gEinsteinRexImage )
