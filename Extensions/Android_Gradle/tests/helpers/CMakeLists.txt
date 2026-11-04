cmake_minimum_required(VERSION 3.22)
project(delib_helper_tests LANGUAGES CXX)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/DeLibHelpers.cmake")
enable_testing()

# Each public header must compile as the first and only include in its own TU.
set(general_definition_headers
    GeneralDefinition.h GeneralMacros.h PathUtilities.h MemoryConstants.h HashUtilities.h
    StringTraits.h StringConversion.h UnicodeConversion.h StringUtilities.h StringSplit.h
    ArithmeticTraits.h ArithmeticUtilities.h ContainerUtilities.h)
set(header_probes)
foreach(header IN LISTS general_definition_headers)
    set(probe "${CMAKE_CURRENT_BINARY_DIR}/header-${header}.cpp")
    file(GENERATE OUTPUT "${probe}" CONTENT "#include \"${header}\"\n")
    list(APPEND header_probes "${probe}")
endforeach()
add_library(general_definition_headers OBJECT ${header_probes})
target_link_libraries(general_definition_headers PRIVATE DeLib::Helpers)

# The Unicode utility must also work without the umbrella's C++20/iconv requirements.
add_executable(unicode_conversion_test UnicodeConversionTest.cpp)
target_compile_features(unicode_conversion_test PRIVATE cxx_std_17)
set_target_properties(unicode_conversion_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_include_directories(unicode_conversion_test PRIVATE "${FTW_CORE_INCLUDE}")
add_test(NAME unicode_conversion COMMAND unicode_conversion_test)

add_executable(general_definition_test GeneralDefinitionTest.cpp HelperOtherTU.cpp)
target_link_libraries(general_definition_test PRIVATE DeLib::Helpers)
add_test(NAME general_definition COMMAND general_definition_test)
add_executable(acp_override_test ACPOverrideTest.cpp)
# Deliberately test a different macro policy in a separate executable (no mixed-policy TUs).
target_compile_features(acp_override_test PRIVATE cxx_std_20)
target_include_directories(acp_override_test PRIVATE "${FTW_CORE_INCLUDE}")
if(NOT WIN32)
    target_link_libraries(acp_override_test PRIVATE Iconv::Iconv)
    target_compile_definitions(acp_override_test PRIVATE DELIB_ACP_ENCODING="CP936")
    add_test(NAME acp_override COMMAND acp_override_test)
endif()

find_package(Threads REQUIRED)
add_executable(path_owner_thread_test PathOwnerThreadTest.cpp)
target_link_libraries(path_owner_thread_test PRIVATE DeLib::Helpers Threads::Threads)
add_test(NAME path_owner_threads COMMAND path_owner_thread_test)

add_executable(map_data_test MapDataTest.cpp)
target_link_libraries(map_data_test PRIVATE DeLib::Helpers)
add_test(NAME map_data COMMAND map_data_test)

add_executable(event_adapter_test EventAdapterTest.cpp)
target_link_libraries(event_adapter_test PRIVATE DeLib::Helpers)
add_test(NAME event_adapter COMMAND event_adapter_test)
