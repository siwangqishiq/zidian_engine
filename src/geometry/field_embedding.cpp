
#include "geometry/field_embedding.h"

#include <stdexcept>
#include <vector>

namespace zidian {
    // 构造
    FieldEmbedding::FieldEmbedding(
        const QuadraticField& source,
        const QuadraticField& target
    )
        : source_(source),
          target_(target) {

        validateEmbedding(source_);
    }

    // 获取源域和目标域
    const QuadraticField& FieldEmbedding::getSource() const {
        return source_;
    }

    const QuadraticField& FieldEmbedding::getTarget() const {
        return target_;
    }

    // 验证嵌入是否存在
    //
    // 对于：
    //
    // K = K0(sqrt(d))
    //
    // 要求：
    //
    // 1. K0 能够嵌入 target。
    // 2. sqrt(phi(d)) 存在于 target。
    //
    // 通过 getMappedGenerator() 获取生成元映射，
    // 同时完成缓存。
    void FieldEmbedding::validateEmbedding(
        const QuadraticField& source
    ) const {

        if (source.isBaseField()) {
            return;
        }

        const QuadraticField* parent =
            source.getParent();

        if (parent == nullptr) {
            throw std::logic_error(
                "FieldEmbedding: invalid quadratic extension"
            );
        }

        // 先验证父域。
        validateEmbedding(*parent);

        // 获取当前扩张生成元的映射。
        // 如果已经计算过，直接从缓存读取。
        getMappedGenerator(source);
    }

    // 获取扩张生成元的映射
    //
    // source = parent(sqrt(d))
    //
    // phi(sqrt(d)) = sqrt(phi(d))
    //
    // 结果缓存在 mappedGenerators_ 中。
    const QuadraticNumber& FieldEmbedding::getMappedGenerator(
        const QuadraticField& source
    ) const {

        if (source.isBaseField()) {
            throw std::logic_error(
                "FieldEmbedding: base field has no extension generator"
            );
        }

        // 查询缓存
        auto it = mappedGenerators_.find(&source);

        if (it != mappedGenerators_.end()) {
            return it->second;
        }

        // 获取扩张信息
        const QuadraticNumber* radicand =
            source.getRadicand();

        if (radicand == nullptr || source.getParent() == nullptr) {
            throw std::logic_error(
                "FieldEmbedding: invalid quadratic extension"
            );
        }

        // 转换 radicand
        //
        // phi(d)
        QuadraticNumber mappedRadicand =
            convertRecursive(*radicand);

        // 在目标域中求正平方根
        //
        // phi(sqrt(d)) = sqrt(phi(d))
        QuadraticNumber mappedSqrt(target_);

        if (!mappedRadicand.trySqrt(mappedSqrt)) {
            throw std::invalid_argument(
                "FieldEmbedding: target field does not contain "
                "the required square root"
            );
        }

        // 写入缓存
        auto result = mappedGenerators_.emplace(
            &source,
            mappedSqrt
        );

        return result.first->second;
    }

    // 递归转换
    //
    // source = parent(sqrt(d))
    //
    // value = A + B * sqrt(d)
    //
    // phi(value) =
    //     phi(A) + phi(B) * phi(sqrt(d))
    QuadraticNumber FieldEmbedding::convertRecursive(
        const QuadraticNumber& value
    ) const {

        const QuadraticField& source =
            value.getField();

        const auto& coefficients =
            value.getCoefficients();

        // 根域 Q
        if (source.isBaseField()) {

            if (coefficients.size() != 1) {
                throw std::logic_error(
                    "FieldEmbedding: invalid base field coefficients"
                );
            }

            return QuadraticNumber(
                target_,
                coefficients[0]
            );
        }

        // 当前扩张域
        const QuadraticField* parent =
            source.getParent();

        if (parent == nullptr) {
            throw std::logic_error(
                "FieldEmbedding: invalid quadratic extension"
            );
        }

        const std::size_t parentDimension =
            parent->getDimension();

        if (coefficients.size() != source.getDimension()) {
            throw std::logic_error(
                "FieldEmbedding: invalid coefficient dimension"
            );
        }

        // 拆分：
        //
        // value = A + B * sqrt(d)
        std::vector<Rational> aCoefficients(
            parentDimension,
            Rational(0)
        );

        std::vector<Rational> bCoefficients(
            parentDimension,
            Rational(0)
        );

        for (std::size_t i = 0;
             i < parentDimension;
             ++i) {

            aCoefficients[i] = coefficients[i];

            bCoefficients[i] =
                coefficients[parentDimension + i];
        }

        QuadraticNumber a(
            *parent,
            aCoefficients
        );

        QuadraticNumber b(
            *parent,
            bCoefficients
        );

        // 递归转换 A 和 B
        QuadraticNumber mappedA =
            convertRecursive(a);

        QuadraticNumber mappedB =
            convertRecursive(b);

        // 获取当前扩张生成元的映射
        //
        // 如果之前计算过，这里不会再次调用 trySqrt。
        const QuadraticNumber& mappedSqrt =
            getMappedGenerator(source);

        // 合成最终结果
        //
        // phi(value) =
        //     phi(A) + phi(B) * phi(sqrt(d))
        return mappedA + mappedB * mappedSqrt;
    }

    // 对外转换接口
    QuadraticNumber FieldEmbedding::convert(
        const QuadraticNumber& value
    ) const {

        if (&value.getField() != &source_) {
            throw std::invalid_argument(
                "FieldEmbedding: value does not belong to source field"
            );
        }

        return convertRecursive(value);
    }

}
