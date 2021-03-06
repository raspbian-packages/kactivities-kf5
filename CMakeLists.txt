# vim:set softtabstop=3 shiftwidth=3 tabstop=3 expandtab:

cmake_minimum_required (VERSION 2.8.12)

project (KActivities)

option (KACTIVITIES_LIBRARY_ONLY "If true, compiles only the KActivities library, without the QML imports." OFF)
option (KACTIVITIES_ENABLE_EXCEPTIONS "If you have Boost 1.53, you need to build KActivities with exceptions enabled. This is UNTESTED and EXPERIMENTAL!" OFF)

message (
   WARNING "
        +----------------------------------------------------------------+
   !!!  |  Only the library (libKF5Activities) and the QML imports       |
   !!!  |  are build from this repository.                               |
   !!!  |                                                                |
   !!!  |  The activities service (kactivitymanagerd) has been moved to  |
   !!!  |  the kde:kactivitymanagerd repository along with its plugins.  |
   !!!  |                                                                |
   !!!  |  The Dolphin plugins (fileitem plugin), the KIO slave and KCM  |
   !!!  |  now live in the kde:kactivities-workspace repository.         |
        +----------------------------------------------------------------+
   "
   )

set (REQUIRED_QT_VERSION 5.3.0)

# We don't build in-source
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message (
      FATAL_ERROR
      "kactivities require an out of source build. Please create a separate build directory and run 'cmake path_to_plasma [options]' there."
   )
endif ()

set (KACTIVITIES_CURRENT_ROOT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Extra CMake stuff
include(FeatureSummary)
find_package(ECM 5.28.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings NO_POLICY_SCOPE)
include (GenerateExportHeader)
include (ECMGenerateHeaders)

# Qt
set (CMAKE_AUTOMOC ON)
find_package (Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS Core DBus)

# KDE Frameworks
set(KF5_VERSION "5.28.0") # handled by release scripts
set(KF5_DEP_VERSION "5.28.0") # handled by release scripts

# Basic includes
include (CPack)

include (ECMPackageConfigHelpers)
include (ECMSetupVersion)

message ("We are using the ${CMAKE_CXX_COMPILER_ID} compiler")
if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (KACTIVITIES_OVERRIDE_VISIBILITY STREQUAL "default"))
   message ("Setting visibility preset to default")
   set(CMAKE_CXX_VISIBILITY_PRESET default)
   set(CMAKE_VISIBILITY_INLINES_HIDDEN 0)
   string (REPLACE "-fvisibility-inlines-hidden" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
   string (REPLACE "-fvisibility=hidden" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif ()

# libKActivities

ecm_setup_version (
   ${KF5_VERSION}
   VARIABLE_PREFIX KACTIVITIES
   VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kactivities_version.h"
   PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5ActivitiesConfigVersion.cmake"
   SOVERSION 5
   )

set (CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5Activities")

install (
   EXPORT KF5ActivitiesLibraryTargets
   DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
   FILE KF5ActivitiesLibraryTargets.cmake
   NAMESPACE KF5::
   )

ecm_configure_package_config_file (
   "${CMAKE_CURRENT_SOURCE_DIR}/KF5ActivitiesConfig.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/KF5ActivitiesConfig.cmake"
   INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
   PATH_VARS  KF5_INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX
   )

install (
   FILES "${CMAKE_CURRENT_BINARY_DIR}/KF5ActivitiesConfig.cmake"
         "${CMAKE_CURRENT_BINARY_DIR}/KF5ActivitiesConfigVersion.cmake"
   DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
   COMPONENT Devel
   )

install (
   FILES ${CMAKE_CURRENT_BINARY_DIR}/kactivities_version.h
   DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
   )

add_subdirectory (src)
add_subdirectory (autotests)
add_subdirectory (tests)

# Write out the features
feature_summary (WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

