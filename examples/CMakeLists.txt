add_executable(MinimalExample minimal.cpp)

target_link_libraries(MinimalExample PRIVATE Corona::Logger)

target_compile_features(MinimalExample PRIVATE cxx_std_20)
