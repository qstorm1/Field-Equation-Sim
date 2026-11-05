#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/constants.h>
#include <symengine/functions.h>

#include "Tensors.h"

using expression = SymEngine::RCP<const SymEngine::Basic>;
using variable = SymEngine::RCP<const SymEngine::Symbol>;
namespace tensorMath {
    using matrix = std::vector<std::vector<expression>>;

    // Copy of m with the given row and column removed
    inline matrix minorMatrix(const matrix &m, size_t row, size_t col) {
        const size_t n = m.size();
        matrix out;
        out.reserve(n - 1);
        for (size_t i = 0; i < n; i++) {
            if (i == row) continue;
            std::vector<expression> r;
            r.reserve(n - 1);
            for (size_t j = 0; j < n; j++) {
                if (j == col) continue;
                r.push_back(m[i][j]);
            }
            out.push_back(std::move(r));
        }
        return out;
    }

    // Determinant by Laplace expansion along the first row
    inline expression determinant(const matrix &m) {
        const size_t n = m.size();
        if (n == 1) return m[0][0];
        if (n == 2) {
            return SymEngine::sub(SymEngine::mul(m[0][0], m[1][1]),
                                  SymEngine::mul(m[0][1], m[1][0]));
        }

        expression det = SymEngine::zero;
        for (size_t col = 0; col < n; col++) {
            // Skipping zero entries makes diagonal/sparse metrics much cheaper
            if (SymEngine::eq(*m[0][col], *SymEngine::zero)) continue;

            expression term = SymEngine::mul(m[0][col],
                                             determinant(minorMatrix(m, 0, col)));
            det = (col % 2 == 0) ? SymEngine::add(det, term)
                                 : SymEngine::sub(det, term);
        }
        return SymEngine::expand(det);
    }

    // Inverse = adjugate / determinant, where adj[i][j] = (-1)^(i+j) * minor(j, i)
    inline matrix inverse(const matrix &m, const std::string &label) {
        const size_t n = m.size();
        expression det = determinant(m);

        if (SymEngine::eq(*det, *SymEngine::zero)) {
            throw std::runtime_error("Tensor '" + label + "' is singular (det = 0); cannot invert.");
        }

        matrix inv(n, std::vector<expression>(n));
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                expression cof = determinant(minorMatrix(m, j, i)); // note the (j, i) swap
                if ((i + j) % 2 == 1) cof = SymEngine::neg(cof);
                inv[i][j] = SymEngine::expand(SymEngine::div(cof, det));
            }
        }
        return inv;
    }
}

