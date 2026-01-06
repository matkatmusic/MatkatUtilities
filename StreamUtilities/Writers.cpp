/*
  ==============================================================================

    Writers.cpp
    Created: 17 Aug 2025 9:18:06pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "Writers.h"

namespace Stream
{
namespace Writers
{
namespace detail
{
/*
 all of these are modifications of the OscOutputStream code
 */
bool writeInt8 (juce::int8 value, juce::OutputStream& output)
{
    return output.writeByte(static_cast<char>(value));
}

bool writeUint8( juce::uint8 value, juce::OutputStream& output)
{
    return writeInt8(static_cast<juce::int8>(value), output);
}

bool writeUint16 (juce::uint16 value, juce::OutputStream& output)
{
    return output.writeShortBigEndian(static_cast<juce::uint16>(value));
}

bool writeInt32 (juce::int32 value, juce::OutputStream& output)
{
    return output.writeIntBigEndian (value);
}

bool writeUint64 (juce::uint64 value, juce::OutputStream& output)
{
    return output.writeInt64BigEndian (juce::int64 (value));
}

bool writeFloat32 (float value, juce::OutputStream& output)
{
    return output.writeFloatBigEndian (value);
}

bool writeString (const juce::String& value, juce::OutputStream& output)
{
    if (! output.writeString (value))
        return false;
    
    const size_t numPaddingZeros = ~value.getNumBytesAsUTF8() & 3;
    
    return output.writeRepeatedByte ('\0', numPaddingZeros);
}

} //end namespace detail

bool write(juce::OutputStream& os) { return false; }

}//end namespace Writers
} //end namespace Stream
