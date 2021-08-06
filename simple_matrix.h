#pragma once

#include <vector>

template <typename T>
class SimpleMatrix
{
public:
    using value_type = T;
    explicit SimpleMatrix(int row, int col) : row_(row), col_(col), size_(row_ * col_) { data_.resize(size_); }
    const T& operator() (int row, int col) const { return data_[col_ * row + col]; }
    T& operator() (int row, int col) { return data_[col_ * row + col]; }
    const T& operator[] (int i) const { return data_[i]; }
    T& operator[] (int i) { return data_[i]; }
    int rows() const { return row_; }
    int cols() const { return col_; }
    int size() const { return size_; }
    T* data() { return data_.data(); }
private:
    int row_, col_, size_;
    std::vector<T> data_;
};
template <typename T>
SimpleMatrix<T> operator+ (const SimpleMatrix<T>& x, const SimpleMatrix<T>& y)
{
    SimpleMatrix<T> sum(x.rows(), x.cols());
    auto size = sum.size();
    for (int i = 0; i < size; ++i)
        sum[i] = x[i] + y[i];
    return sum;
}

template <typename T>
void add3(SimpleMatrix<T>& sum, const SimpleMatrix<T>& x, const SimpleMatrix<T>& y, const SimpleMatrix<T>& z)
{
    auto size = sum.size();
    for (int i = 0; i < size; ++i)
        sum[i] = x[i] + y[i] + z[i];
}

template<typename T, typename... Args>
void add(SimpleMatrix<T>& sum, Args&&... args)
{
    auto size = sum.size();
    for (int i = 0; i < size; ++i)
        sum[i] = (args[i] + ...);
}