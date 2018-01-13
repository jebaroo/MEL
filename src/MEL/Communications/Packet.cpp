#include <MEL/Communications/Packet.hpp>
#include <MEL/Communications/Socket.hpp>
#include <cstring>
#include <cwchar>

namespace mel
{

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Packet::Packet() :
    read_pos_(0),
    send_pos_(0),
    is_valid_(true)
{
}

Packet::~Packet() {
}

void Packet::append(const void* data, std::size_t send_in_bytes) {
    if (data && (send_in_bytes > 0)) {
        std::size_t start = data_.size();
        data_.resize(start + send_in_bytes);
        std::memcpy(&data_[start], data, send_in_bytes);
    }
}

void Packet::clear() {
    data_.clear();
    read_pos_ = 0;
    is_valid_ = true;
}

const void* Packet::get_data() const {
    return !data_.empty() ? &data_[0] : NULL;
}

std::size_t Packet::get_data_size() const {
    return data_.size();
}

bool Packet::end_of_packet() const {
    return read_pos_ >= data_.size();
}


Packet::operator BoolType() const {
    return is_valid_ ? &Packet::check_size : NULL;
}

Packet& Packet::operator >>(bool& data) {
    uint8 value;
    if (*this >> value)
        data = (value != 0);
    return *this;
}

Packet& Packet::operator >>(int8& data) {
    if (check_size(sizeof(data))) {
        data = *reinterpret_cast<const int8*>(&data_[read_pos_]);
        read_pos_ += sizeof(data);
    }
    return *this;
}

Packet& Packet::operator >>(uint8& data) {
    if (check_size(sizeof(data))) {
        data = *reinterpret_cast<const uint8*>(&data_[read_pos_]);
        read_pos_ += sizeof(data);
    }
    return *this;
}

Packet& Packet::operator >>(int16& data) {
    if (check_size(sizeof(data))) {
        data = ntohs(*reinterpret_cast<const int16*>(&data_[read_pos_]));
        read_pos_ += sizeof(data);
    }
    return *this;
}

Packet& Packet::operator >>(uint16& data) {
    if (check_size(sizeof(data))) {
        data = ntohs(*reinterpret_cast<const uint16*>(&data_[read_pos_]));
        read_pos_ += sizeof(data);
    }
    return *this;
}

Packet& Packet::operator >>(int32& data) {
    if (check_size(sizeof(data))) {
        data = ntohl(*reinterpret_cast<const int32*>(&data_[read_pos_]));
        read_pos_ += sizeof(data);
    }
    return *this;
}

Packet& Packet::operator >>(uint32& data) {
    if (check_size(sizeof(data))) {
        data = ntohl(*reinterpret_cast<const uint32*>(&data_[read_pos_]));
        read_pos_ += sizeof(data);
    }
    return *this;
}

Packet& Packet::operator >>(int64& data) {
    if (check_size(sizeof(data))) {
        // Since ntohll is not available everywhere, we have to convert
        // to network byte order (big endian) manually
        const uint8* bytes = reinterpret_cast<const uint8*>(&data_[read_pos_]);
        data = (static_cast<int64>(bytes[0]) << 56) |
               (static_cast<int64>(bytes[1]) << 48) |
               (static_cast<int64>(bytes[2]) << 40) |
               (static_cast<int64>(bytes[3]) << 32) |
               (static_cast<int64>(bytes[4]) << 24) |
               (static_cast<int64>(bytes[5]) << 16) |
               (static_cast<int64>(bytes[6]) <<  8) |
               (static_cast<int64>(bytes[7])      );
        read_pos_ += sizeof(data);
    }
    return *this;
}

Packet& Packet::operator >>(uint64& data) {
    if (check_size(sizeof(data))) {
        // Since ntohll is not available everywhere, we have to convert
        // to network byte order (big endian) manually
        const uint8* bytes = reinterpret_cast<const uint8*>(&data_[read_pos_]);
        data = (static_cast<uint64>(bytes[0]) << 56) |
               (static_cast<uint64>(bytes[1]) << 48) |
               (static_cast<uint64>(bytes[2]) << 40) |
               (static_cast<uint64>(bytes[3]) << 32) |
               (static_cast<uint64>(bytes[4]) << 24) |
               (static_cast<uint64>(bytes[5]) << 16) |
               (static_cast<uint64>(bytes[6]) <<  8) |
               (static_cast<uint64>(bytes[7])      );
        read_pos_ += sizeof(data);
    }

    return *this;
}

Packet& Packet::operator >>(float& data) {
    if (check_size(sizeof(data))) {
        data = *reinterpret_cast<const float*>(&data_[read_pos_]);
        read_pos_ += sizeof(data);
    }
    return *this;
}

Packet& Packet::operator >>(double& data) {
    if (check_size(sizeof(data))) {
        data = *reinterpret_cast<const double*>(&data_[read_pos_]);
        read_pos_ += sizeof(data);
    }
    return *this;
}

Packet& Packet::operator >>(char* data) {
    // First extract string length
    uint32 length = 0;
    *this >> length;
    if ((length > 0) && check_size(length)) {
        // Then extract characters
        std::memcpy(data, &data_[read_pos_], length);
        data[length] = '\0';

        // Update reading position
        read_pos_ += length;
    }
    return *this;
}

Packet& Packet::operator >>(std::string& data) {
    // First extract string length
    uint32 length = 0;
    *this >> length;
    data.clear();
    if ((length > 0) && check_size(length)) {
        // Then extract characters
        data.assign(&data_[read_pos_], length);

        // Update reading position
        read_pos_ += length;
    }
    return *this;
}

Packet& Packet::operator >>(wchar_t* data) {
    // First extract string length
    uint32 length = 0;
    *this >> length;
    if ((length > 0) && check_size(length * sizeof(uint32))) {
        // Then extract characters
        for (uint32 i = 0; i < length; ++i)
        {
            uint32 character = 0;
            *this >> character;
            data[i] = static_cast<wchar_t>(character);
        }
        data[length] = L'\0';
    }
    return *this;
}

Packet& Packet::operator >>(std::wstring& data) {
    // First extract string length
    uint32 length = 0;
    *this >> length;
    data.clear();
    if ((length > 0) && check_size(length * sizeof(uint32))) {
        // Then extract characters
        for (uint32 i = 0; i < length; ++i)
        {
            uint32 character = 0;
            *this >> character;
            data += static_cast<wchar_t>(character);
        }
    }
    return *this;
}

Packet& Packet::operator <<(bool data) {
    *this << static_cast<uint8>(data);
    return *this;
}

Packet& Packet::operator <<(int8 data) {
    append(&data, sizeof(data));
    return *this;
}

Packet& Packet::operator <<(uint8 data) {
    append(&data, sizeof(data));
    return *this;
}

Packet& Packet::operator <<(int16 data) {
    int16 toWrite = htons(data);
    append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator <<(uint16 data) {
    uint16 toWrite = htons(data);
    append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator <<(int32 data) {
    int32 toWrite = htonl(data);
    append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator <<(uint32 data) {
    uint32 toWrite = htonl(data);
    append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator <<(int64 data) {
    // Since htonll is not available everywhere, we have to convert
    // to network byte order (big endian) manually
    uint8 toWrite[] =
    {
        static_cast<uint8>((data >> 56) & 0xFF),
        static_cast<uint8>((data >> 48) & 0xFF),
        static_cast<uint8>((data >> 40) & 0xFF),
        static_cast<uint8>((data >> 32) & 0xFF),
        static_cast<uint8>((data >> 24) & 0xFF),
        static_cast<uint8>((data >> 16) & 0xFF),
        static_cast<uint8>((data >>  8) & 0xFF),
        static_cast<uint8>((data      ) & 0xFF)
    };
    append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator <<(uint64 data) {
    // Since htonll is not available everywhere, we have to convert
    // to network byte order (big endian) manually
    uint8 toWrite[] =
    {
        static_cast<uint8>((data >> 56) & 0xFF),
        static_cast<uint8>((data >> 48) & 0xFF),
        static_cast<uint8>((data >> 40) & 0xFF),
        static_cast<uint8>((data >> 32) & 0xFF),
        static_cast<uint8>((data >> 24) & 0xFF),
        static_cast<uint8>((data >> 16) & 0xFF),
        static_cast<uint8>((data >>  8) & 0xFF),
        static_cast<uint8>((data      ) & 0xFF)
    };
    append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator <<(float data) {
    append(&data, sizeof(data));
    return *this;
}

Packet& Packet::operator <<(double data) {
    append(&data, sizeof(data));
    return *this;
}

Packet& Packet::operator <<(const char* data) {
    // First insert string length
    uint32 length = static_cast<uint32>(std::strlen(data));
    *this << length;
    // Then insert characters
    append(data, length * sizeof(char));
    return *this;
}

Packet& Packet::operator <<(const std::string& data) {
    // First insert string length
    uint32 length = static_cast<uint32>(data.size());
    *this << length;
    // Then insert characters
    if (length > 0)
        append(data.c_str(), length * sizeof(std::string::value_type));
    return *this;
}

Packet& Packet::operator <<(const wchar_t* data) {
    // First insert string length
    uint32 length = static_cast<uint32>(std::wcslen(data));
    *this << length;
    // Then insert characters
    for (const wchar_t* c = data; *c != L'\0'; ++c)
        *this << static_cast<uint32>(*c);
    return *this;
}

Packet& Packet::operator <<(const std::wstring& data) {
    // First insert string length
    uint32 length = static_cast<uint32>(data.size());
    *this << length;
    // Then insert characters
    if (length > 0) {
        for (std::wstring::const_iterator c = data.begin(); c != data.end(); ++c)
            *this << static_cast<uint32>(*c);
    }
    return *this;
}

bool Packet::check_size(std::size_t size) {
    is_valid_ = is_valid_ && (read_pos_ + size <= data_.size());

    return is_valid_;
}

const void* Packet::on_send(std::size_t& size) {
    size = get_data_size();
    return get_data();
}

void Packet::on_receive(const void* data, std::size_t size) {
    append(data, size);
}

} // namespace mel