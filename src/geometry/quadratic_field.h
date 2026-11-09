#pragma once

#include "geometry/rational.h"

namespace zidian{
    class QuadraticField {
    public:
        explicit QuadraticField(const Rational& d);

        const Rational& getD() const;

        bool operator==(const QuadraticField& other) const;
        bool operator!=(const QuadraticField& other) const;

    private:
        Rational d_;

        static bool isRationalSquare(const Rational& value);
        static bool isPerfectSquare(Integer value);
    };
}