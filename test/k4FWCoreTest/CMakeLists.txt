#[[
Copyright (c) 2014-2023 Key4hep-Project.

This file is part of Key4hep.
See https://key4hep.github.io/key4hep-doc/ for further info.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
]]

################################################################################
# Package: k4FWCoreTest
################################################################################

find_package(EDM4HEP)

set(k4fwcoretest_plugin_sources
  src/components/TestDataHandleUniquePtr.cpp
  src/components/TestUniqueIDGenSvc.cpp
  src/components/k4FWCoreTest_AlgorithmWithTFile.cpp
  src/components/k4FWCoreTest_CreateExampleEventData.cpp
  src/components/k4FWCoreTest_HelloWorldAlg.cpp
  src/components/k4FWCoreTest_cellID_reader.cpp
  src/components/k4FWCoreTest_cellID_writer.cpp
  src/components/k4FWCoreTest_CheckExampleEventData.cpp
)

gaudi_add_module(k4FWCoreTestPlugins
                 SOURCES ${k4fwcoretest_plugin_sources}
                 LINK Gaudi::GaudiAlgLib Gaudi::GaudiKernel k4FWCore k4FWCore::k4Interface ROOT::Core ROOT::RIO ROOT::Tree EDM4HEP::edm4hepDict EDM4HEP::edm4hep)

include(CTest)

set(K4RUN ${PROJECT_SOURCE_DIR}/k4FWCore/scripts/k4run)

get_target_property(edm4hep_lib EDM4HEP::edm4hepDict LOCATION)
get_filename_component(edm4hep_loc ${edm4hep_lib} DIRECTORY)

get_target_property(root_lib ROOT::Core LOCATION)
get_filename_component(root_loc ${root_lib} DIRECTORY)

get_target_property(podio_lib podio::podio LOCATION)
get_filename_component(podio_loc ${podio_lib} DIRECTORY)

#--- The genConf directory has been renamed to genConfDir in Gaudi 35r1
#--- See https://gitlab.cern.ch/gaudi/Gaudi/-/merge_requests/1158
set(GAUDI_GENCONF_DIR "genConfDir")
if (${Gaudi_VERSION} VERSION_LESS 35.1)
  set(GAUDI_GENCONF_DIR "genConf")
endif()


function(set_test_env _testname)
  set_property(TEST ${_testname} APPEND PROPERTY ENVIRONMENT "ROOT_INCLUDE_PATH=${podio_loc}/../include/podio:${edm4hep_loc}/../include:$ENV{ROOT_INCLUDE_PATH}")
  set_property(TEST ${_testname} APPEND PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}:${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}:${PROJECT_BINARY_DIR}/test/k4FWCoreTest:${root_loc}:${edm4hep_loc}:${podio_loc}:$ENV{LD_LIBRARY_PATH}")
  set_property(TEST ${_testname} APPEND PROPERTY ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}/${GAUDI_GENCONF_DIR}:${PROJECT_BINARY_DIR}/test/k4FWCoreTest/${GAUDI_GENCONF_DIR}:$ENV{PYTHONPATH}")
endfunction()


add_test(NAME CreateExampleEventData
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/createExampleEventData.py)
set_test_env(CreateExampleEventData)

add_test(NAME CheckExampleEventData
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/checkExampleEventData.py
)
set_test_env(CheckExampleEventData)
set_tests_properties( CheckExampleEventData
         PROPERTIES PASS_REGULAR_EXPRESSION "Application Manager Terminated successfully with a user requested ScheduledStop"
         DEPENDS CreateExampleEventData)

add_test(NAME CreateExampleEventData_cellID
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/createExampleEventData_cellID.py)
set_test_env(CreateExampleEventData_cellID)

add_test(NAME TwoProducers
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/TwoProducers.py --filename output_k4fwcore_test_twoproducer2.root --magicNumberOffset.Producer2 12345 --Producer1.magicNumberOffset 54321 )
set_test_env(TwoProducers)

add_test(NAME ReadExampleEventData
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/readExampleEventData.py)
set_test_env(ReadExampleEventData)
set_property(TEST ReadExampleEventData APPEND PROPERTY DEPENDS CreateExampleEventData)

add_test(NAME ReadExampleDataFromNthEvent
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/readExampleDataFromNthEvent.py)
set_test_env(ReadExampleDataFromNthEvent)
set_property(TEST ReadExampleDataFromNthEvent APPEND PROPERTY DEPENDS CreateExampleEventData)

add_test(NAME AlgorithmWithTFile
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/TestAlgorithmWithTFile.py)
set_test_env(AlgorithmWithTFile)

add_test(NAME AlgorithmWithTFileCheckFrameworkOutput
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND  python scripts/check_TestAlgorithmWithTFile_framework_nonempty.py)
set_property(TEST AlgorithmWithTFileCheckFrameworkOutput APPEND PROPERTY DEPENDS AlgorithmWithTFile)
set_test_env(AlgorithmWithTFileCheckFrameworkOutput)

add_test(NAME AlgorithmWithTFileCheckMyTFileOutput
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND  python scripts/check_TestAlgorithmWithTFile_myTFile_nonempty.py)
set_property(TEST AlgorithmWithTFileCheckMyTFileOutput APPEND PROPERTY DEPENDS AlgorithmWithTFile)
set_test_env(AlgorithmWithTFileCheckMyTFileOutput)

add_test(NAME CheckCommandLineArguments
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/createHelloWorld.py --HelloWorldAlg1.PerEventPrintMessage TwasBrilligAndTheSlithyToves
)
set_tests_properties(CheckCommandLineArguments PROPERTIES
  PASS_REGULAR_EXPRESSION "TwasBrilligAndTheSlithyToves"
)
set_test_env(CheckCommandLineArguments)

add_test(NAME checkKeepDropSwitch
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND python scripts/check_KeepDropSwitch.py)
set_test_env(checkKeepDropSwitch)
set_property(TEST checkKeepDropSwitch APPEND PROPERTY DEPENDS ReadExampleEventData)

add_test(NAME TestDataHandleUniquePtr
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/TestDataHandleUniquePtr.py)
set_test_env(TestDataHandleUniquePtr)

add_test(NAME TestUniqueIDGenSvc
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/TestUniqueIDGenSvc.py)
set_test_env(TestUniqueIDGenSvc)


add_test(NAME CreateLegacyExampleEventData
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} options/createLegacyExampleEventData.py)
set_test_env(CreateLegacyExampleEventData)

add_test(NAME Testk4runNoArguments
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN})
set_test_env(Testk4runNoArguments)
set_tests_properties(Testk4runNoArguments
  PROPERTIES PASS_REGULAR_EXPRESSION "Usage: k4run <options_file.py>, use --help to get a complete list of arguments")

add_test(NAME Testk4runCustomArguments
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} --foo=42 options/TestArgs.py)
set_test_env(Testk4runCustomArguments)
set_tests_properties(Testk4runCustomArguments
  PROPERTIES PASS_REGULAR_EXPRESSION "The answer is 42")

add_test(NAME Testk4runVerboseOutput
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} --verbose options/TestArgs.py)
set_test_env(Testk4runVerboseOutput)
set_tests_properties(Testk4runVerboseOutput
  PROPERTIES PASS_REGULAR_EXPRESSION " VERBOSE ")

add_test(NAME Testk4runHelpOnly
         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
         COMMAND ${K4RUN} --help)
set_test_env(Testk4runHelpOnly)
set_tests_properties(Testk4runHelpOnly
  PROPERTIES PASS_REGULAR_EXPRESSION "show this help message and exit")
