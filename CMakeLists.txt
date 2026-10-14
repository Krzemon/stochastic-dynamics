cmake_minimum_required(VERSION 3.10)
project(stochastic_dynamics VERSION 0.0.1 LANGUAGES CXX)
set(PROJECT_AUTHOR "Przemysław Ryś")

# --- Ustawienia kompilacji ---
set(CMAKE_BUILD_TYPE Debug) # Debug / Release
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -O3 -march=native")
# -ffast-math  # szybsze obliczenia zmiennoprzecinkowe kosztem ścisłej zgodności z IEEE 754

# --- Katalogi projektu ---
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(DATA_DIR ${PROJECT_SOURCE_DIR}/data)
set(PLOTS_DIR ${PROJECT_SOURCE_DIR}/plots)
set(PYTHON_DIR ${PROJECT_SOURCE_DIR}/python)

# --- Zrodla ---
file(GLOB SOURCES "${SRC_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# --- Linkowanie ---
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE gsl gslcblas m)

# --- Definicje kompilacji ---
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_ROOT_DIR="${PROJECT_SOURCE_DIR}")

# --- Custom target: run_plot (ogólny - uruchamia domyślny tryb) ---
add_custom_target(run_plot
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    COMMAND python3 ${PYTHON_DIR}/plot_sequence_analysis.py
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    DEPENDS ${PROJECT_NAME}
    COMMENT "Uruchamianie domyślnego trybu i generowanie wykresów..."
)