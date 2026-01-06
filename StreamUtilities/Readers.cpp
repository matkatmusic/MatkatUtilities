/*
  ==============================================================================

    Readers.cpp
    Created: 17 Aug 2025 9:18:13pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "Readers.h"
#include "../MiscUtilities/Indenter.h"

namespace Stream
{
namespace Readers
{
namespace detail
{
/*
 all of these are modifications of the OscInputStream code
 */

juce::int32 readInt32(juce::InputStream& input)
{
    checkBytesAvailable (4, "Stream::Reader input stream exhausted while reading int32", input);
    return input.readIntBigEndian();
}

juce::int8 readInt8(juce::InputStream& input)
{
    checkBytesAvailable(1, "Stream::Reader input stream exhausted while reading int8", input);
    return (juce::int8)input.readByte();
}

juce::uint8 readUint8(juce::InputStream& input)
{
    return static_cast<juce::uint8>(readInt8(input));
}

juce::uint16 readUint16(juce::InputStream& input)
{
    checkBytesAvailable(2, "Stream::Reader input stream exhausted while reading uint16", input);
    return static_cast<juce::uint16>(input.readShortBigEndian());
}

juce::uint64 readUint64(juce::InputStream& input)
{
    checkBytesAvailable (8, "Stream::Reader input stream exhausted while reading uint64", input);
    return (juce::uint64) input.readInt64BigEndian();
}

float readFloat32(juce::InputStream& input)
{
    checkBytesAvailable (4, "Stream::Reader input stream exhausted while reading float", input);
    return input.readFloatBigEndian();
}

juce::String readString(juce::InputStream& input)
{
    checkBytesAvailable (4, "Stream::Reader input stream exhausted while reading string", input);
    
    auto posBegin = (size_t) input.getPosition();
    auto s = input.readString();
    auto posEnd = (size_t) input.getPosition();
    
    input.setPosition(posEnd - 1);
    if( input.readByte() != '\0' )
//    if (static_cast<const char*> (input.getData()) [posEnd - 1] != '\0')
    {
        juce::Logger::writeToLog ("Stream::Reader input stream exhausted before finding null terminator of string");
        jassertfalse;
        return {};
    }
    
    size_t bytesRead = posEnd - posBegin;
    readPaddingZeros (bytesRead, input);
    
    return s;
}

juce::MemoryBlock readBlock(juce::InputStream& input)
{
    INDENT
    checkBytesAvailable (4, "Stream::Reader input stream exhausted while reading blob", input);
    
    DBG( Indenter() << "reading size from input" );
    auto blobDataSize = input.readIntBigEndian();
    checkBytesAvailable ((blobDataSize + 3) % 4, "Stream::Reader input stream exhausted before reaching end of blob", input);
    
    juce::MemoryBlock blob;
    
    
    DBG( Indenter() << "reading block from input" );
    auto bytesRead =
#if JUCE_WINDOWS
    input.readIntoMemoryBlock (blob, (juce::ssize_t) blobDataSize);
#elif JUCE_MAC
    input.readIntoMemoryBlock(blob, (ssize_t) blobDataSize);
#endif
    readPaddingZeros (bytesRead, input);
    
    return blob;
}

void readPaddingZeros (size_t bytesRead, juce::InputStream& input)
{
    size_t numZeros = ~(bytesRead - 1) & 0x03;
    
    if( numZeros > 0 )
    {
        INDENT
        DBG( Indenter() << "reading padding zeros from input" );
    }
    
    while (numZeros > 0)
    {
        if (input.isExhausted() || input.readByte() != 0)
        {
            juce::Logger::writeToLog ("Stream::Reader input stream format error: missing padding zeros");
            jassertfalse;
            break;
        }
        
        --numZeros;
    }
}

bool checkBytesAvailable (juce::int64 requiredBytes, const char* message, juce::InputStream& input)
{
    if (input.getNumBytesRemaining() < requiredBytes)
    {
        juce::Logger::writeToLog (message);
        jassertfalse;
        return false;
    }
    
    return true;
}
} //end namespace detail

} //end namespace Readers

} //end namespace Stream
