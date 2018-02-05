#ifndef MEL_TIMESTAMP_HPP
#define MEL_TIMESTAMP_HPP

#include <string>

namespace mel {

/// Encapsulates a timestamp
class Timestamp {
public:
    /// Default constructor
    Timestamp();

    std::string yyyy_mm_dd() const;

    std::string hh_mm_ss_mmm() const;

    std::string yyyy_mm_dd_hh_mm_ss() const;

    std::string yyyy_mm_dd_hh_mm_ss_mmm() const;
    

public:

    int year;      ///< year
    int month;     ///< month                    [1-12]
    int yday;      ///< day of year              [1-366]
    int mday;      ///< day of month             [1-31]
    int wday;      ///< day of week (Sunday = 1) [1-7]
    int hour;      ///< hour                     [0-23]
    int min;       ///< minute                   [0-59]
    int sec;       ///< second                   [0-59]
    int millisec;  ///< millisecond              [0-999]
};

}  // namespace mel

#endif  // MEL_TIMESTAMP_HPP
