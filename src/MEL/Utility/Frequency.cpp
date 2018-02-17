#include <MEL/Utility/Frequency.hpp>
#include <MEL/Utility/Time.hpp>

namespace mel {

const Frequency Frequency::Zero;

Frequency::Frequency() :
    hertz_(0)
{

}

Frequency::Frequency(int64 hertz) :
    hertz_(hertz)
{

}

int64 Frequency::as_hertz() const {
    return hertz_;
}

int32 Frequency::as_kilohertz() const {
    return static_cast<int32>(hertz_ / 1000);
}

double Frequency::as_megahertz() const {
    return hertz_ / 1000000.0;
}

Time Frequency::to_time() const {
    return seconds(1.0 / static_cast<double>(hertz_));
}

//==============================================================================
// INSTANTIATION FUNCTIONS
//==============================================================================

Frequency hertz(int64 amount) {
    return Frequency(amount);
}

Frequency kilohertz(int32 amount) {
    return Frequency(static_cast<int64>(amount * 1000.0));
}

Frequency megahertz(double amount) {
    return Frequency(static_cast<int64>(amount * 1000000.0));
}

} // namespace mel