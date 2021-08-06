# MatrixBenchmark
Comparison of the performance of some matrix operations for different implementations. 

How to use:

1) It's necessary to configure the dependencies described in the conanfile.txt. The use of Conan is optional.
2) mkdir build && cd build
3) cmake .. -G \<your generator\> -DWITH_VECTORIZE_AVX=ON -DWITH_MATRIX_ADDITION=ON or -DWITH_MKL=ON -DWITH_MATRIX_MULTIPLICATION=ON. Additional options can be found in the CMakeLists.txt.
4) cmake --build . --config Release
  
