cmake_minimum_required(VERSION 3.20)
project(project VERSION 1.0)

# Enable OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "Found OpenMP")
else()
    message(FATAL_ERROR "Could not find OpenMP")
endif()

# Add the executable
add_executable(${PROJECT_NAME}
    main.cpp
    PokemonCard.h
    GameSimulation.h
    GameSimulation.cpp
    FileParser.h
    FileParser.cpp
    Constants.h
)

# Set C++ standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Link OpenMP
target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION .)
