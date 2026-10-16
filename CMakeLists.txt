
cmake_minimum_required(VERSION 3.28)

project(ColliderToCosmos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(ROOT CONFIG REQUIRED COMPONENTS Core Hist RIO Tree)

add_executable(particle_level analysis/particle_level.cpp)

target_link_libraries(particle_level PRIVATE ROOT::Core ROOT::Hist ROOT::RIO ROOT::Tree)

target_include_directories(particle_level PRIVATE
    "$ENV{CONDA_PREFIX}/include"
)

target_link_libraries(particle_level PRIVATE
    fastjettools
    fastjet
)

target_link_directories(particle_level PRIVATE
    "$ENV{CONDA_PREFIX}/lib"
)

set(HEPMC_DIR "${PROJECT_SOURCE_DIR}/MG5_aMC_v3_8_0/HEPTools")

target_include_directories(particle_level PRIVATE
    "${HEPMC_DIR}/hepmc/include"
)

target_link_libraries(particle_level PRIVATE
    "${HEPMC_DIR}/lib/libHepMC.so"
)

set_target_properties(particle_level PROPERTIES
    BUILD_RPATH "${HEPMC_DIR}/lib"
)