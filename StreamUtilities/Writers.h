/*
  ==============================================================================

    Writers.h
    Created: 17 Aug 2025 9:18:06pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include "../MiscUtilities/TypeName.h"
#include "../MiscUtilities/Concepts.h"
#include "../MiscUtilities/Indenter.h"

namespace Stream
{

namespace Writers
{

namespace detail
{
/*
 all of these are modifications of the OscOutputStream code
 */
bool writeInt8 (juce::int8 value, juce::OutputStream& output);
bool writeUint8( juce::uint8 value, juce::OutputStream& output);

bool writeUint16 (juce::uint16 value, juce::OutputStream& output);

bool writeInt32 (juce::int32 value, juce::OutputStream& output);

bool writeUint64 (juce::uint64 value, juce::OutputStream& output);
bool writeFloat32 (float value, juce::OutputStream& output);

bool writeString (const juce::String& value, juce::OutputStream& output);

template<typename T>
requires ( IsWriteBlockCompatible<T> )
bool writeBlock (const T& blob, juce::OutputStream& output)
{
    INDENT
    auto size = blob.getSize();
    if( size == 0 )
        return false;
    
    DBG( Indenter() << "writing block size to output" );
    if( output.writeIntBigEndian( (int)size) == false )
    {
        return false;
    }
    
    DBG( Indenter() << "writing block data to output" );
    if( output.write(blob.getData(), size) == false )
    {
        return false;
    }
    
    const size_t numPaddingZeros = ~(size - 1) & 3;
    if( numPaddingZeros > 0 )
    {
        DBG( Indenter() << "writing padding zeros to output" );
        return output.writeRepeatedByte (0, numPaddingZeros);
    }
    
    return true;
}
} //end namespace detail

bool write(juce::OutputStream& os);

template<typename T_, typename ... Args>
bool write(juce::OutputStream& os, T_&& firstArg, Args&& ... args )
{
    bool result = false;
    using T = std::remove_cvref_t<T_>;
    
    if constexpr( std::is_enum_v<T> )
    {
        using U = std::underlying_type_t<T>;
        result |= write(os, static_cast<U>(firstArg));
    }
    else if constexpr (std::is_integral_v<T> )
    {
        if constexpr ( sizeof(T) == 1 )
        {
            if( std::is_signed_v<T> )
                result |= detail::writeInt8(firstArg, os);
            else
                result |= detail::writeUint8(firstArg, os);
        }
        else if constexpr( std::is_same_v<T, juce::uint16> )
            result |= detail::writeUint16(firstArg, os);
        else if constexpr( std::is_same_v<T, juce::int32> )
            result |= detail::writeInt32(firstArg, os);
        else if constexpr( std::is_same_v<T, juce::uint64> )
            result |= detail::writeUint64(firstArg, os);
    }
    else if constexpr( std::is_same_v<T, float> )
        result |= detail::writeFloat32(firstArg, os);
    else if constexpr( std::is_same_v<T, juce::String> )
        result |= detail::writeString(firstArg, os);
    else if constexpr( IsWriteBlockCompatible<T> )
        result |= detail::writeBlock(firstArg, os);
    else if constexpr( HasWriteToStream<T> )
        result |= T::writeToStream(firstArg, os);
    else
    {
//        auto t_name = TYPE_NAME(T_);
//        DBG( "T_ = " << juce::String(t_name.data(), t_name.size()));
//        auto tname = TYPE_NAME(T);
//        DBG( "T = " << juce::String(tname.data(), tname.size()));
        jassertfalse; //unimplemented handling for T!!!
    }

    if( result == false )
    {
        return result;
    }

    if( sizeof...(Args) == 0 )
        return result;

    return write(os, std::forward<Args>(args)...);
}

}//end namespace Writers
} //end namespace Stream
