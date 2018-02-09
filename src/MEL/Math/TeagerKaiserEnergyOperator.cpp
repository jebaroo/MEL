#include <MEL/Math/TeagerKaiserEnergyOperator.hpp>

namespace mel {

TeagerKaiserEnergyOperator::TeagerKaiserEnergyOperator() :
    Process(),
    s_(std::vector<double>(n_, 0.0))
{}

double TeagerKaiserEnergyOperator::process( const double x, const Time& current_time) {
    double y;
    y = s_[0] * s_[0] - x * s_[1];
    s_[0] = x;
    s_[1] = s_[0];
    return y;
}


void TeagerKaiserEnergyOperator::reset() {
    s_ = std::vector<double>(n_, 0.0);
}


} // mel