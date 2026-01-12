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
/*
 TODO: All of these functions should return bool (or std::optional?) and take the desired type as a reference param:
 example:
 bool readInt32(juce::inputStream& input, juce::int32& val)
 {
    if( checkBytesAvailable(4, "message") == false )
        return false;
    val = input.readIntBigEndian();
    return true;
 }
 */
bool readInt32(juce::InputStream& input, juce::int32& val)
{
    if( checkBytesAvailable (4, "Stream::Reader input stream exhausted while reading int32", input) == false )
        return false;
    val = input.readIntBigEndian();
    return true;
}

bool readInt8(juce::InputStream& input, juce::int8& v)
{
    if( checkBytesAvailable(1, "Stream::Reader input stream exhausted while reading int8", input) == false )
        return false;
    v = static_cast<juce::int8>(input.readByte());
    return true;
}

bool readUint8(juce::InputStream& input, juce::uint8& v)
{
    if( checkBytesAvailable(1, "Stream::Reader input stream exhausted while reading uint8", input) == false )
        return false;
    v = static_cast<juce::uint8>(input.readByte());
    return true;
}

bool readUint16(juce::InputStream& input, juce::uint16& v)
{
    if( checkBytesAvailable(2, "Stream::Reader input stream exhausted while reading uint16", input) == false )
        return false;
    v = static_cast<juce::uint16>(input.readShortBigEndian());
    return true;
}

bool readUint64(juce::InputStream& input, juce::uint64& v)
{
    if( checkBytesAvailable (8, "Stream::Reader input stream exhausted while reading uint64", input) == false )
        return false;
    v = static_cast<juce::uint64>(input.readInt64BigEndian());
    return true;
}

bool readFloat32(juce::InputStream& input, float& f)
{
    if( checkBytesAvailable (4, "Stream::Reader input stream exhausted while reading float", input) == false )
        return false;
    f = input.readFloatBigEndian();
    return true;
}

bool readString(juce::InputStream& input, juce::String& str)
{
    if( checkBytesAvailable (4, "Stream::Reader input stream exhausted while reading string", input) == false )
        return false;
    
    auto posBegin = (size_t) input.getPosition();
    str = input.readString();
    auto posEnd = (size_t) input.getPosition();
    
    input.setPosition(posEnd - 1);
    if( input.readByte() != '\0' )
//    if (static_cast<const char*> (input.getData()) [posEnd - 1] != '\0')
    {
        juce::Logger::writeToLog ("Stream::Reader input stream exhausted before finding null terminator of string");
        jassertfalse;
        return false;
    }
    
    size_t bytesRead = posEnd - posBegin;
    if( readPaddingZeros (bytesRead, input) == false )
        return false;
    
    return true;
}

bool readBlock(juce::InputStream& input, juce::MemoryBlock& blob)
{
    INDENT
    if( checkBytesAvailable (4, "Stream::Reader input stream exhausted while reading blob", input) == false )
    {
        return false;
    }
    
    DBG( Indenter() << "reading size from input" );
    auto blobDataSize = input.readIntBigEndian();
    if( checkBytesAvailable ((blobDataSize + 3) % 4, "Stream::Reader input stream exhausted before reaching end of blob", input) == false )
    {
        return false;
    }
    
    DBG( Indenter() << "reading block from input" );
    auto bytesRead =
#if JUCE_WINDOWS
    input.readIntoMemoryBlock (blob, (juce::ssize_t) blobDataSize);
#elif JUCE_MAC
    input.readIntoMemoryBlock(blob, (ssize_t) blobDataSize);
#endif
    
    if( blobDataSize != bytesRead )
    {
        jassertfalse;
        return false;
    }
    
    if( readPaddingZeros (bytesRead, input) == false )
        return false;
    
    return true;
}

bool readUuid(juce::InputStream& input, juce::Uuid& uuid)
{
    juce::MemoryBlock block;
    if( readBlock(input, block) == false )
        return false;
    
    jassert( block.getSize() >= uuid.size() );
    
    uuid = static_cast<const juce::uint8*>(block.getData());
    
    return true;
}

bool readPaddingZeros (size_t bytesRead, juce::InputStream& input)
{
    size_t numZeros = ~(bytesRead - 1) & 0x03;
    
    if( numZeros == 0 )
        return true;
    
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
            return false;
        }
        
        --numZeros;
    }
    
    return true;
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
