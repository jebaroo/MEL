// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_RECORD_HPP
#define MEL_RECORD_HPP

#include <MEL/Config.hpp>
#include <sys/stat.h>
#include <MEL/Logging/Severity.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Core/Timestamp.hpp>
#include <sstream>

namespace mel {
namespace detail {

// Stream output operators as free functions

inline void operator<<(std::ostringstream& stream, const char* data) {
    data = data ? data : "(null)";
    std::operator<<(stream, data);
}

inline void operator<<(std::ostringstream& stream, const std::string& data) {
    mel::detail::operator<<(stream, data.c_str());
}

#ifdef _WIN32
namespace meta {

template <class T, class Stream>
inline char operator<<(Stream&, const T&);

template <class T, class Stream>
struct Streamable {
    enum {
        value =
            sizeof(operator<<(*reinterpret_cast<Stream*>(0),
                              *reinterpret_cast<const T*>(0))) != sizeof(char)
    };
};

template <class Stream>
struct Streamable<std::ios_base&(std::ios_base&), Stream> {
    enum { value = true };
};

template <class Stream, size_t N>
struct Streamable<wchar_t[N], Stream> {
    enum { value = false };
};

template <class Stream, size_t N>
struct Streamable<const wchar_t[N], Stream> {
    enum { value = false };
};

template <bool B, class T = void>
struct enableIf {};

template <class T>
struct enableIf<true, T> {
    typedef T type;
};
}  // namespace meta

template <class T>
inline typename meta::enableIf<meta::Streamable<T, std::ostream>::value &&
                                   !meta::Streamable<T, std::wostream>::value,
                               void>::type
operator<<(std::wostringstream& stream, const T& data) {
    std::ostringstream ss;
    ss << data;
    stream << ss.str();
}
#endif
}  // namespace detail

//==============================================================================
// CLASS DECLARATION
//==============================================================================
//
/// Encapsulates a log line record
class MEL_API Record {
public:
    /// Constructor
    Record(Severity severity,
           const char* func,
           size_t line,
           const char* file,
           Timestamp timestamp = Timestamp());

    /// Destructor
    virtual ~Record();

    // Stream operator overloads
    Record& operator<<(char data);

    Record& operator<<(std::ostream& (*data)(std::ostream&));

    template <typename T>
    Record& operator<<(const T& data) {
        using namespace detail;
        message_ << data;
        return *this;
    }

    /// Returns timestamp at which Record was constructed
    virtual const Timestamp& get_timestamp() const;

    /// Gets the severity of a record
    virtual Severity get_severity() const;

    /// Gets ID of thread a Record was made on
    virtual unsigned int get_tid_() const;

    /// Gets the line number where the Record was made
    virtual size_t get_line() const;

    /// Gets the message contained by a Record
    virtual const char* get_message() const;

    /// Gets the name of the function in which the Record was made
    virtual const char* get_func() const;

    /// Gets the name of the file in which the Record was made
    virtual const char* get_file() const;

private:
    Timestamp timestamp_;              ///< timestamp
    const Severity severity_;          /// Record severity
    const unsigned int tid_;           ///< thread ID
    const size_t line_;                ///< line number
    std::ostringstream message_;       ///< string stream for message
    const char* const func_;           ///< function name string
    const char* const file_;           ///< file name string
    mutable std::string func_str_;     ///< function name string
    mutable std::string message_str_;  ///< message string
};

inline MEL_API std::string process_function_name(const char* func);

}  // namespace mel

#endif  // MEL_RECORD_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
