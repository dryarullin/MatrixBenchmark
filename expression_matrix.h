#pragma once

#include <type_traits>
#include <vector>

namespace ExpressionTemplates {

template <typename T, typename U>
class MatrixSum
{
public:
    using value_type = std::common_type_t<typename T::value_type, typename U::value_type>;

    MatrixSum(const T& mat1, const U& mat2)
        : mat1_(mat1), mat2_(mat2) {}
    value_type operator[] (int i) const
    {
        return mat1_[i] + mat2_[i];
    }

private:
    const T& mat1_;
    const U& mat2_;
};

    template <typename T>
    class ExpressionMatrix
    {
    public:
        using value_type = T;

        explicit ExpressionMatrix(int row, int col) : row_(row), col_(col), size_(row_ * col_)
        {
            data_.resize(size_);
        }

        const T& operator() (int row, int col) const
        {
            return data_[col_ * row + col];
        }
        T& operator() (int row, int col)
        {
            return data_[col_ * row + col];
        }
        const T& operator[] (int i) const
        {
            return data_[i];
        }
        T& operator[] (int i)
        {
            return data_[i];
        }

        template <typename K>
        ExpressionMatrix& operator= (const K& other)
        {
            const int size = size_;
            //#pragma loop(no_vector)
            for (int i = 0; i < size; ++i) {
                data_[i] = other[i];
            }
            //for (int i = 0; i < size_; i+=8) {
            //    data_[i] = other[i];
            //    data_[i + 1] = other[i + 1];
            //    data_[i + 2] = other[i + 2];
            //    data_[i + 3] = other[i + 3];
            //    data_[i + 4] = other[i + 4];
            //    data_[i + 5] = other[i + 5];
            //    data_[i + 6] = other[i + 6];
            //    data_[i + 7] = other[i + 7];
            //}
            return *this;
    }

        int rows() const { return row_; }
        int cols() const { return col_; }
        int size() const { return size_; }

    private:
        int row_;
        int col_;
        int size_;
        std::vector<T> data_;
    };

template <typename T, typename U>
MatrixSum<T, U> operator+ (const T& mat1, const U& mat2)
{
    return { mat1, mat2 };
}

}