
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

# Keep the validated Pythia version ahead of the Conda installation.
set(LIFETIME_PYTHIA_DIR "${HEPMC_DIR}/pythia8")
target_include_directories(particle_level BEFORE PRIVATE "${LIFETIME_PYTHIA_DIR}/include")
target_link_libraries(particle_level PRIVATE "${LIFETIME_PYTHIA_DIR}/lib/libpythia8.so")
target_compile_definitions(particle_level PRIVATE
    LIFETIME_PYTHIA_XML="${LIFETIME_PYTHIA_DIR}/share/Pythia8/xmldoc"
)

set_target_properties(particle_level PROPERTIES
    BUILD_RPATH "${LIFETIME_PYTHIA_DIR}/lib;${HEPMC_DIR}/lib"
)