cmake_minimum_required(VERSION 2.8.2)

set(CMAKE_COLOR_MAKEFILE ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Flags for basic C files
set(basic_c_compiler_flags_list
  -pedantic-errors
  -Wextra
  -Wall
  -Wfloat-equal
  -fdiagnostics-show-option
  -march=native
)

# Additional flags for basic C++ files
set(basic_cxx_compiler_add_flags_list
  -std=c++11
  -Wctor-dtor-privacy
  -Weffc++
  -Woverloaded-virtual
)

# Additional flags for VATA C++ files
set(vata_cxx_compiler_add_flags_list
  -Wold-style-cast
)

# Setting GCC flags for compilation of VATA C files
set(vata_c_compiler_flags "")
foreach(param ${basic_c_compiler_flags_list})
  set(vata_c_compiler_flags "${vata_c_compiler_flags} ${param}")
endforeach(param)


# Setting GCC flags for compilation of VATA C++ files
set(vata_cxx_compiler_flags ${vata_c_compiler_flags})
foreach(param ${basic_cxx_compiler_add_flags_list})
  set(vata_cxx_compiler_flags "${vata_cxx_compiler_flags} ${param}")
endforeach(param)
foreach(param ${vata_cxx_compiler_add_flags_list})
  set(vata_cxx_compiler_flags "${vata_cxx_compiler_flags} ${param}")
endforeach(param)

include_directories(../../include)

add_library(bddTDSimCore bdd_td_tree_aut_sim_expl.cc)

set_source_files_properties(bdd_td_tree_aut_sim_expl.cc PROPERTIES COMPILE_FLAGS ${vata_cxx_compiler_flags})
