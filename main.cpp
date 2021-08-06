/* Eigen benchmarks*/

#if defined(DONT_VECTORIZE)
  #define EIGEN_DONT_VECTORIZE
#elif defined(VECTORIZE_SSE) && defined(_MSC_VER)
  #define EIGEN_VECTORIZE_SSE3
  #define EIGEN_VECTORIZE_SSSE3
  #define EIGEN_VECTORIZE_SSE4_1
  #define EIGEN_VECTORIZE_SSE4_2
#elif defined(VECTORIZE_AVX) && defined(_MSC_VER)
  #define EIGEN_VECTORIZE_AVX
  #define EIGEN_VECTORIZE_AVX2
#elif defined(USE_MKL)
  #define EIGEN_USE_MKL_ALL
#endif

#include "simple_matrix.h"
#include "expression_matrix.h"
#include <benchmark/benchmark.h>
#include <Eigen/Core>
#include <mkl.h>

using namespace Eigen;
using namespace std;

#define REPEAT2(x) x x
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)
#define REPEAT(x) REPEAT32(x)

// Matrix size
int rows = 1000, cols = 1000;

template <typename T>
void FillMatriñes(T& mat1, T& mat2, T& mat3, T& result)
{
    for (int i = 0; i < mat1.rows(); ++i) {
        for (int j = 0; j < mat1.cols(); ++j) {
            mat1(i, j) = static_cast<float>(i + j);
            mat2(i, j) = static_cast<float>(i * j);
            mat3(i, j) = static_cast<float>(i - j);
            result(i, j) = 0.0f;
        }
    }
}

void BM_SimpleMatrix(benchmark::State& state) {
    SimpleMatrix<float> mat1(rows, cols), mat2(rows, cols), mat3(rows, cols), result(rows, cols);
    FillMatriñes(mat1, mat2, mat3, result);

    for (auto _ : state) {
        REPEAT({ result = mat1 + mat2 + mat3;
                 benchmark::DoNotOptimize(result);
               })
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_ColumnEigenMatrix(benchmark::State& state) {
    setNbThreads(1);
    MatrixXf mat1(rows, cols), mat2(rows, cols), mat3(rows, cols), result(rows, cols);
    FillMatriñes(mat1, mat2, mat3, result);

    for (auto _ : state) {
        REPEAT({ result = mat1 + mat2 + mat3;
                 benchmark::DoNotOptimize(result);
               })
    }
    state.SetItemsProcessed(32 * state.iterations());
}

using RowMatrixXf = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
void BM_RowEigenMatrix(benchmark::State& state) {
    setNbThreads(1);
    RowMatrixXf mat1(rows, cols), mat2(rows, cols), mat3(rows, cols), result(rows, cols);
    FillMatriñes(mat1, mat2, mat3, result);

    for (auto _ : state) {
        REPEAT({ result = mat1 + mat2 + mat3;
                 benchmark::DoNotOptimize(result);
               })
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_RowMKLMatrix(benchmark::State& state) {
    SimpleMatrix<float> mat1(rows, cols), mat2(rows, cols), mat3(rows, cols), result(rows, cols);
    FillMatriñes(mat1, mat2, mat3, result);

    auto n = static_cast<MKL_INT>(result.size());
    const float alfa = 1.0f;
    const MKL_INT incx = 1;
    const MKL_INT incy = 1;

    for (auto _ : state) {
        REPEAT({ cblas_scopy(n, mat1.data(), incx, result.data(), incy);
                 cblas_saxpy(n, alfa, mat2.data(), incx, result.data(), incy);
                 cblas_saxpy(n, alfa, mat3.data(), incx, result.data(), incy);
                 benchmark::DoNotOptimize(result);
               })
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_FunctionMatrix(benchmark::State& state) {
    SimpleMatrix<float> mat1(rows, cols), mat2(rows, cols), mat3(rows, cols), result(rows, cols);
    FillMatriñes(mat1, mat2, mat3, result);

    for (auto _ : state) {
        REPEAT({ add(result, mat1, mat2, mat3);
                 benchmark::DoNotOptimize(result);
               })
    }
    state.SetItemsProcessed(32 * state.iterations());
}


void BM_ExpressionMatrix(benchmark::State& state) {
    ExpressionTemplates::ExpressionMatrix<float> mat1(rows, cols), mat2(rows, cols), mat3(rows, cols), result(rows, cols);
    FillMatriñes(mat1, mat2, mat3, result);

    for (auto _ : state) {
        REPEAT({ result = mat1 + mat2 + mat3;
                 benchmark::DoNotOptimize(result);
               })
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_RowEigenMatrixInit(benchmark::State& state) {
    RowMatrixXf mat1(rows, cols), mat2(rows, cols), mat3(rows, cols);

    for (auto _ : state) {
        REPEAT({
                for (int i = 0; i < mat1.rows(); ++i) {
                     for (int j = 0; j < mat1.cols(); ++j) {
                         mat1(i, j) = static_cast<float>(i + j);
                         mat2(i, j) = static_cast<float>(i * j);
                         mat3(i, j) = static_cast<float>(i - j);
                     }
                 }
                 benchmark::DoNotOptimize(mat1);
                 benchmark::DoNotOptimize(mat2);
                 benchmark::DoNotOptimize(mat3);
            }
        )
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_ColumnEigenMatrixInit(benchmark::State& state) {
    MatrixXf mat1(rows, cols), mat2(rows, cols), mat3(rows, cols), result(rows, cols);

    for (auto _ : state) {
        REPEAT({
                for (int i = 0; i < mat1.rows(); ++i) {
                     for (int j = 0; j < mat1.cols(); ++j) {
                         mat1(i, j) = static_cast<float>(i + j);
                         mat2(i, j) = static_cast<float>(i * j);
                         mat3(i, j) = static_cast<float>(i - j);
                     }
                 }
                 benchmark::DoNotOptimize(mat1);
                 benchmark::DoNotOptimize(mat2);
                 benchmark::DoNotOptimize(mat3);
            }
        )
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_EigenMatrixMul(benchmark::State& state) {
    MatrixXf mat1(rows, cols), mat2(rows, cols), mat3(rows, cols), result(rows, cols);
    FillMatriñes(mat1, mat2, mat3, result);

    for (auto _ : state) {
        REPEAT({ result.noalias() = mat1 * mat2 * mat3;
                 benchmark::DoNotOptimize(result);
               })
    }
    state.SetItemsProcessed(32 * state.iterations());
}

void BM_MKLMatrixMul(benchmark::State& state) {
    SimpleMatrix<float> mat1(rows, cols), mat2(rows, cols), mat3(rows, cols), temp_result(rows, cols), result(rows, cols);
    FillMatriñes(mat1, mat2, mat3, result);

    mkl_set_num_threads_local(1);
    mkl_set_num_threads(1);
    const auto M = static_cast<MKL_INT>(mat1.rows());
    const auto K = static_cast<MKL_INT>(mat1.cols());
    const auto N = static_cast<MKL_INT>(mat2.cols());

    for (auto _ : state) {
        REPEAT({
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, 1.0f, mat1.data(), K, mat2.data(), N, 0.0f, temp_result.data(), N);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, 1.0f, temp_result.data(), K, mat3.data(), N, 0.0f, result.data(), N);
        benchmark::DoNotOptimize(result);
            })
    }
    state.SetItemsProcessed(32 * state.iterations());
}

#if defined(MATRIX_MULTIPLICATION)
    BENCHMARK(BM_MKLMatrixMul)->Unit(benchmark::kMillisecond)->Iterations(100);
    BENCHMARK(BM_EigenMatrixMul)->Unit(benchmark::kMillisecond)->Iterations(100);
#elif defined(MATRIX_ADDITION)
    BENCHMARK(BM_SimpleMatrix)->Unit(benchmark::kMillisecond)->Iterations(100);
    BENCHMARK(BM_ColumnEigenMatrix)->Unit(benchmark::kMillisecond)->Iterations(100);
    BENCHMARK(BM_FunctionMatrix)->Unit(benchmark::kMillisecond)->Iterations(100);
    BENCHMARK(BM_ExpressionMatrix)->Unit(benchmark::kMillisecond)->Iterations(100);
    BENCHMARK(BM_RowMKLMatrix)->Unit(benchmark::kMillisecond)->Iterations(100);
#else
    BENCHMARK(BM_RowEigenMatrixInit)->Unit(benchmark::kMillisecond)->Iterations(100);
    BENCHMARK(BM_ColumnEigenMatrixInit)->Unit(benchmark::kMillisecond)->Iterations(100);
#endif
