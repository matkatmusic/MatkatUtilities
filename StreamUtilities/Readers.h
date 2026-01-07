/*
  ==============================================================================

    Readers.h
    Created: 17 Aug 2025 9:18:13pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../MiscUtilities/Concepts.h"

namespace Stream
{
namespace Readers
{
namespace detail
{
/*
 all of these are modifications of the OscInputStream code
 */
//void readPaddingZeros (size_t bytesRead, juce::InputStream& input);
//bool checkBytesAvailable (juce::int64 requiredBytes, const char* message, juce::InputStream& input);

juce::int8 readInt8(juce::InputStream& input);
juce::uint8 readUint8(juce::InputStream& input);
juce::uint16 readUint16(juce::InputStream& input);

juce::int32 readInt32(juce::InputStream& input);
juce::uint64 readUint64(juce::InputStream& input);

float readFloat32(juce::InputStream& input);

juce::String readString(juce::InputStream& input);

juce::MemoryBlock readBlock(juce::InputStream& input);

void readPaddingZeros (size_t bytesRead, juce::InputStream& input);

bool checkBytesAvailable (juce::int64 requiredBytes, const char* message, juce::InputStream& input);
} //end namespace detail

template<typename T_>
T_ read(juce::InputStream& is)
{
    using T = std::remove_cvref_t<T_>;
    
    if constexpr( std::is_enum_v<T> )
    {
        using U = std::underlying_type_t<T>;
        auto u = read<U>(is);
        return static_cast<T_>(u);
    }
    else if constexpr (std::is_integral_v<T> )
    {
        if constexpr ( sizeof(T) == 1 )
        {
            if( std::is_signed_v<T> )
                return detail::readInt8(is);
            else
                return detail::readUint8(is);
        }
        else if constexpr( std::is_same_v<T, juce::uint16> )
            return detail::readUint16(is);
        else if constexpr( std::is_same_v<T, juce::int32> )
            return detail::readInt32(is);
        else if constexpr( std::is_same_v<T, juce::uint64> )
            return detail::readUint64(is);
    }
    else if constexpr( std::is_same_v<T, float> )
        return detail::readFloat32(is);
    else if constexpr( std::is_same_v<T, juce::String> )
        return detail::readString(is);
    else if constexpr( std::is_same_v<T, juce::MemoryBlock> )
        return detail::readBlock(is);
    else if constexpr( HasReadFromStream<T> )
        return T::readFromStream(is);
    
    jassertfalse; //unimplemented handler for type!!!
    return T{};
}

/*
 recursively iterates through `args`, reading multiple values from the input stream, writing the results to the `first` reference arg during each recursion.
 be warned that this function doesn't signal if the inner `read<T>` calls fail!
 */
template<typename T_, typename ... Args>
void read(juce::InputStream& is, T_& first, Args& ... args)
{
    static_assert(!std::is_const_v<T_>, "read needs non-const lvalues");

    using T = std::remove_cvref_t<T_>;
    first = read<T>(is);

    if constexpr (sizeof...(Args) > 0)
        read(is, args...);
}
} //end namespace Readers
} //end namespace Stream
