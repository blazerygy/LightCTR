//
//  matrix.h
//  LightCTR
//
//  Created by SongKuangshi on 2017/10/24.
//  Copyright © 2017年 SongKuangshi. All rights reserved.
//

#ifndef matrix_h
#define matrix_h

#include <cmath>
#include <vector>
#include "random.h"
#include "assert.h"
using namespace std;

struct Matrix_Description {
    size_t x_len, y_len;
};

// 2D Matrix
class Matrix {
public:
    Matrix(size_t _x_len, size_t _y_len, bool alloc = 1): x_len(_x_len), y_len(_y_len) {
        if (alloc) {
            matrix = new vector<double>();
            matrix->resize(x_len * y_len);
        }
    }
    ~Matrix() {
        matrix->clear();
        matrix = NULL;
    }
    inline void loadDataPtr(vector<double>* const dataPtr) {
        assert(dataPtr->size() == size());
        this->matrix = dataPtr;
    }
    
    inline Matrix* copy(Matrix* newM = NULL) const {
        if (newM == NULL) {
            newM = new Matrix(x_len, y_len);
        }
        assert(x_len == newM->x_len);
        assert(y_len == newM->y_len);
        newM->pointer()->assign(matrix->begin(), matrix->end());
        return newM;
    }
    inline Matrix* reshape(size_t new_x, size_t new_y) {
        if (x_len == new_x && y_len == new_y) {
            return this;
        }
        x_len = new_x;
        y_len = new_y;
        matrix->resize(new_x * new_y);
        return this;
    }
    
    inline size_t size() const {
        return x_len * y_len;
    }
    
    inline const vector<double>::iterator getEle(size_t x, size_t y) const {
        assert(x * y_len + y < size());
        return matrix->begin() + x * y_len + y;
    }
    
    inline void debugPrint() const {
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                printf("%lf ", *getEle(i, j));
            }
            puts("");
        }
        puts("");
    }
    
    inline void zeroInit() {
        assert(matrix);
        std::fill(matrix->begin(), matrix->end(), 0.0f);
    }
    inline void randomInit() {
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                *getEle(i, j) = GaussRand();
            }
        }
    }
    
    template<typename Func>
    inline void operate(Func f) {
        f(this->matrix);
    }
    
    inline Matrix* rot180() {
        assert(x_len == y_len);
        if (x_len == 1) {
            return this;
        }
        for (size_t i = 0; i < x_len / 2; i++) {
            for (size_t j = 0; j < y_len; j++) {
                size_t new_i = x_len - 1 - i;
                size_t new_j = y_len - 1 - j;
                swap(*getEle(i, j), *getEle(new_i, new_j));
            }
        }
        return this;
    }
    
    inline Matrix* transpose() {
        assert(x_len == 1 || y_len == 1);
        swap(x_len, y_len);
        return this;
    }
    
    inline Matrix* inverse() {
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                assert(*getEle(i, j) != 0);
                *getEle(i, j) = 1.0 / *getEle(i, j);
            }
        }
        return this;
    }
    
    inline Matrix* clipping(double clip_threshold) {
        assert(clip_threshold > 0);
        for (auto it = matrix->begin(); it != matrix->end(); it++) {
            if (*it < -clip_threshold) {
                *it = -clip_threshold;
            } else if (*it > clip_threshold) {
                *it = clip_threshold;
            }
        }
        return this;
    }
    
    inline Matrix* add(const Matrix* const another, double scale = 1.0) {
        assert(x_len == another->x_len);
        assert(y_len == another->y_len);
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                *getEle(i, j) = *getEle(i, j) + scale * *another->getEle(i, j);
            }
        }
        return this;
    }
    inline Matrix* add(double fac) {
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                *getEle(i, j) += fac;
            }
        }
        return this;
    }
    
    inline Matrix* subtract(const Matrix* const another, double scale = 1.0) {
        assert(x_len == another->x_len);
        assert(y_len == another->y_len);
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                *getEle(i, j) -= scale * *another->getEle(i, j);
            }
        }
        return this;
    }
    inline Matrix* subtract(double delta) {
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                *getEle(i, j) -= delta;
            }
        }
        return this;
    }
    
    inline Matrix* scale(double scale_fac) {
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                *getEle(i, j) *= scale_fac;
            }
        }
        return this;
    }
    
    inline Matrix* pow(double fac) {
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                *getEle(i, j) = ::pow(*getEle(i, j), fac);
            }
        }
        return this;
    }
    
    inline Matrix* dotProduct(const Matrix* const another) {
        assert(x_len == another->x_len);
        assert(y_len == another->y_len);
        for (size_t i = 0; i < x_len; i++) {
            for (size_t j = 0; j < y_len; j++) {
                if (*another->getEle(i, j) != 0) {
                    *getEle(i, j) *= *another->getEle(i, j);
                    assert(!isinf(*getEle(i, j)));
                }
            }
        }
        return this;
    }
    
    inline Matrix* Multiply(Matrix* ansM, const Matrix* const another) {
        assert(another && y_len == another->x_len);
        if (ansM == NULL) {
            ansM = new Matrix(x_len, another->y_len);
        }
        ansM->zeroInit();
        assert(ansM->x_len == x_len);
        assert(ansM->y_len == another->y_len);
        double tmp;
        for (size_t i = 0; i < x_len; i++) {
            for (size_t k = 0; k < y_len; k++) {
                tmp = *getEle(i, k);
                if (tmp == 0)
                    continue;
                for (size_t j = 0; j < another->y_len; j++) {
                    *ansM->getEle(i, j) += tmp * *another->getEle(k, j);
                }
            }
        }
        return ansM;
    }
    
    // TODO support AVX instruction
    inline void deconvolution_Delta(Matrix*& ansM, const Matrix* const filter, size_t padding = 0, size_t stride = 1) {
        size_t recover_x = (x_len - 1) * stride + filter->x_len - 2 * padding;
        size_t recover_y = (x_len - 1) * stride + filter->x_len - 2 * padding;
        
        if (ansM == NULL) {
            ansM = new Matrix(recover_x, recover_y);
        }
        ansM->zeroInit();
        
        for (size_t i = 0; i < recover_x + 2*padding - filter->x_len + 1; i+=stride) {
            for (size_t j = 0; j < recover_y + 2*padding - filter->y_len + 1; j+=stride) {
                // loop filter size
                for (size_t xc = 0; xc < filter->x_len; xc++) {
                    for (size_t yc = 0; yc < filter->y_len; yc++) {
                        if (i + xc < padding || j + yc < padding || i + xc >= padding + recover_x || j + yc >= padding + recover_x) {
                            continue;
                        }
                        *ansM->getEle(i + xc, j + yc) += *getEle(i / stride, j / stride) * *filter->getEle(xc, yc);
                    }
                }
            }
        }
    }
    inline void deconvolution_Filter(const Matrix* const filterDelta, const Matrix* const input, size_t padding = 0, size_t stride = 1) {
        assert(filterDelta && input);
        size_t recover_x = input->x_len;
        size_t recover_y = input->y_len;
        
        for (size_t i = 0; i < recover_x + 2*padding - filterDelta->x_len + 1; i+=stride) {
            for (size_t j = 0; j < recover_y + 2*padding - filterDelta->y_len + 1; j+=stride) {
                // loop filterDelta size
                for (size_t xc = 0; xc < filterDelta->x_len; xc++) {
                    for (size_t yc = 0; yc < filterDelta->y_len; yc++) {
                        if (i + xc < padding || j + yc < padding || i + xc >= padding + recover_x || j + yc >= padding + recover_x) {
                            continue;
                        }
                        // Weight Gradient descent, scale is sensitive
                        *filterDelta->getEle(xc, yc) += *getEle(i / stride, j / stride) * *input->getEle(i + xc, j + yc);
                    }
                }
            }
        }
    }
    // conv conform to commutative principle
    inline void convolution(Matrix*& ansM, const Matrix* const filter, size_t padding = 0, size_t stride = 1) {
        assert(filter && (filter->x_len <= x_len) && (filter->y_len <= y_len));
        size_t new_x_len = (x_len - filter->x_len + 2 * padding) / stride + 1;
        size_t new_y_len = (y_len - filter->y_len + 2 * padding) / stride + 1;
        
        if (ansM == NULL) {
            ansM = new Matrix(new_x_len, new_y_len);
        }
        ansM->zeroInit();
        // left corner (-padding, -padding) to (xlen-1+padding, ylen-1+padding)
        for (size_t i = 0; i < x_len + 2 * padding - filter->x_len + 1; i+=stride) {
            for (size_t j = 0; j < y_len + 2 * padding - filter->y_len + 1; j+=stride) {
                // loop filter size
                double sum = 0.0f;
                for (size_t xc = i; xc < i + filter->x_len; xc++) {
                    for (size_t yc = j; yc < j + filter->y_len; yc++) {
                        if (xc < padding || yc < padding || xc >= padding + x_len || yc >= padding + y_len) {
                            continue;
                        }
                        sum += *getEle(xc - padding, yc - padding) * *filter->getEle(xc - i, yc - j);
                    }
                }
                assert(!isnan(sum));
                *ansM->getEle(i / stride, j / stride) = sum;
            }
        }
    }
    
    inline vector<double>* pointer() const {
        assert(matrix);
        return this->matrix;
    }
    inline vector<double>& reference() {
        assert(matrix);
        return *this->matrix;
    }
    
    size_t x_len, y_len;
    
private:
    vector<double>* matrix;
};

#endif /* matrix_h */
