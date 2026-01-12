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
#include "../MiscUtilities/TypeName.h"

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

bool readInt8(juce::InputStream& input, juce::int8& v);
bool readUint8(juce::InputStream& input, juce::uint8& v);
bool readUint16(juce::InputStream& input, juce::uint16& v);

bool readInt32(juce::InputStream& input, juce::int32& val);
bool readUint64(juce::InputStream& input, juce::uint64& v);

bool readFloat32(juce::InputStream& input, float& f);

bool readString(juce::InputStream& input, juce::String& str);

bool readBlock(juce::InputStream& input, juce::MemoryBlock& blob);

bool readUuid(juce::InputStream& input, juce::Uuid& uuid);

bool readPaddingZeros (size_t bytesRead, juce::InputStream& input);

bool checkBytesAvailable (juce::int64 requiredBytes, const char* message, juce::InputStream& input);

template<typename Container>
bool readContainer(juce::InputStream& input, Container& container)
{
    auto size = input.readInt64BigEndian();
    auto elementSize = input.readInt64BigEndian();
    auto numBytes = size * elementSize;
    if( checkBytesAvailable(numBytes, "stream ended before container was completed!", input) == false )
    {
        return false;
    }

    container.resize(size);
    jassert( numBytes < std::numeric_limits<juce::int64>::max() );
    auto numRead = input.read(static_cast<void*>(container.data()), static_cast<std::size_t>(numBytes));
    if( numRead != container.size() * elementSize )
    {
        juce::Logger::writeToLog("Stream::Readers::detail::readContainer(): failed to read full container from stream!");
        return false;
    }
    
    if( readPaddingZeros(numRead, input) == false )
        return false;
    
    return true;
}

template<typename T_>
bool read(juce::InputStream& is, T_& val)
{
    using T = std::remove_cvref_t<T_>;
    
    if constexpr( std::is_enum_v<T> )
    {
        using U = std::underlying_type_t<T>;
//        auto u = read<U>(is, val);
//        return static_cast<T_>(u);
        U u;
        if( detail::read(is, u) == false ) //recurse to read underlying type
            return false;
        val = static_cast<T_>(u);
        return true;
    }
    else if constexpr (std::is_integral_v<T> )
    {
        if constexpr ( sizeof(T) == 1 )
        {
            if constexpr( std::is_signed_v<T> )
                return detail::readInt8(is, val);
            else
                return detail::readUint8(is, val);
        }
        else if constexpr( std::is_same_v<T, juce::uint16> )
            return detail::readUint16(is, val);
        else if constexpr( std::is_same_v<T, juce::int32> )
            return detail::readInt32(is, val);
        else if constexpr( std::is_same_v<T, juce::uint64> )
            return detail::readUint64(is, val);
    }
    else if constexpr( std::is_same_v<T, float> )
        return detail::readFloat32(is, val);
    else if constexpr( std::is_same_v<T, juce::String> )
        return detail::readString(is, val);
    else if constexpr( std::is_same_v<T, juce::MemoryBlock> )
        return detail::readBlock(is, val);
    else if constexpr( HasReadFromStream<T> )
    {
//        return T::readFromStream(is, val);
        auto ptr = T::readFromStream(is);
        if( ptr == typename T::Ptr{} )
            return false;
        
        val = *ptr;
        return true;
    }
    else if constexpr( std::is_same_v<T, juce::Uuid> )
        return detail::readUuid(is, val);
    else if constexpr( IsContainerType<T> )
        return detail::readContainer<T>(is, val);
    else
    {
//        auto t_name = TYPE_NAME( std::declval<T_>() );
        auto t_name = GetClassName<T_>();
        DBG( "Stream::Readers::read( T_ ): Unimplemented handling for type!!!");
//        DBG( "T_ = " << juce::String(t_name.data(), t_name.size()));
        DBG( "T_ = " << t_name);
//        auto tname = TYPE_NAME(  std::declval<T>() );
        auto tname = GetClassName<T>();
//        DBG( "T = " << juce::String(tname.data(), tname.size()));
        DBG( "T = " << tname);
        jassertfalse; //unimplemented handling for T!!!
    }
    
    return false;
}

} //end namespace detail

//template<typename T_>
//T_ read(juce::InputStream& is)
//{
//    using T = std::remove_cvref_t<T_>;
//    
//    if constexpr( std::is_enum_v<T> )
//    {
//        using U = std::underlying_type_t<T>;
//        auto u = read<U>(is);
//        return static_cast<T_>(u);
//    }
//    else if constexpr (std::is_integral_v<T> )
//    {
//        if constexpr ( sizeof(T) == 1 )
//        {
//            if( std::is_signed_v<T> )
//                return detail::readInt8(is);
//            else
//                return detail::readUint8(is);
//        }
//        else if constexpr( std::is_same_v<T, juce::uint16> )
//            return detail::readUint16(is);
//        else if constexpr( std::is_same_v<T, juce::int32> )
//            return detail::readInt32(is);
//        else if constexpr( std::is_same_v<T, juce::uint64> )
//            return detail::readUint64(is);
//    }
//    else if constexpr( std::is_same_v<T, float> )
//        return detail::readFloat32(is);
//    else if constexpr( std::is_same_v<T, juce::String> )
//        return detail::readString(is);
//    else if constexpr( std::is_same_v<T, juce::MemoryBlock> )
//        return detail::readBlock(is);
//    else if constexpr( HasReadFromStream<T> )
//        return T::readFromStream(is);
//    else if constexpr( std::is_same_v<T, juce::Uuid> )
//        return detail::readUuid(is);
//    else if constexpr( IsContainerType<T> )
//        return detail::readContainer<T>(is);
//    else
//    {
//        auto t_name = TYPE_NAME( std::declval<T_>() );
//        DBG( "Stream::Readers::read( T_ ): Unimplemented handling for type!!!");
//        DBG( "T_ = " << juce::String(t_name.data(), t_name.size()));
//        auto tname = TYPE_NAME(  std::declval<T>() );
//        DBG( "T = " << juce::String(tname.data(), tname.size()));
//        jassertfalse; //unimplemented handling for T!!!
//    }
//    return T{};
//}



template<typename T_>
bool read(juce::InputStream& is, T_& val)
{
    using T = std::remove_cvref_t<T_>;
    
    if constexpr( std::is_enum_v<T> )
    {
        using U = std::underlying_type_t<T>;
//        auto u = read<U>(is, val);
//        return static_cast<T_>(u);
        U u;
        if( detail::read(is, u) == false ) //recurse to read underlying type
            return false;
        val = static_cast<T_>(u);
        return true;
    }
    
    return detail::read(is, val);
    /*
    else if constexpr (std::is_integral_v<T> )
    {
        if constexpr ( sizeof(T) == 1 )
        {
            if( std::is_signed_v<T> )
            {
                juce::int8 v;
                if( detail::read(is, v) == false )
                    return false;
                val = static_cast<T_>(v);
                return true;
            }
            else
            {
                juce::uint8 v;
                if( detail::read(is, v) == false )
                    return false;
                val = static_cast<T_>(v);
                return true;
            }
        }
        else if constexpr( std::is_same_v<T, juce::uint16> )
            return detail::read(is, val);
        else if constexpr( std::is_same_v<T, juce::int32> )
            return detail::read(is, val);
        else if constexpr( std::is_same_v<T, juce::uint64> )
            return detail::read(is, val);
    }
    else if constexpr( std::is_same_v<T, float> )
        return detail::read(is, val);
    else if constexpr( std::is_same_v<T, juce::String> )
        return detail::read(is, val);
    else if constexpr( std::is_same_v<T, juce::MemoryBlock> )
        return detail::read(is, val);
    else if constexpr( HasReadFromStream<T> )
    {
//        return T::readFromStream(is, val);
        auto ptr = T::readFromStream(is);
        if( ptr == typename T::Ptr{} )
            return false;
        
        val = *ptr;
        return true;
    }
    else if constexpr( std::is_same_v<T, juce::Uuid> )
        return detail::read(is, val);
    else if constexpr( IsContainerType<T> )
        return detail::read(is, val);
    
//    auto t_name = TYPE_NAME( std::declval<T_>() );
    auto t_name = GetClassName<T_>();
    DBG( "Stream::Readers::read( T_ ): Unimplemented handling for type!!!");
//    DBG( "T_ = " << juce::String(t_name.data(), t_name.size()));
    DBG( "T_ = " << t_name );
//    auto tname = TYPE_NAME(  std::declval<T>() );
    auto tname = GetClassName<T>();
//    DBG( "T = " << juce::String(tname.data(), tname.size()));
    DBG( "T = " << tname );
    jassertfalse; //unimplemented handling for T!!!
    return false;
     */
}
/*
 recursively iterates through `args`, reading multiple values from the input stream, writing the results to the `first` reference arg during each recursion.
 */
//template<typename T_, typename ... Args>
//void read(juce::InputStream& is, T_& first, Args& ... args)
//{
//    static_assert(!std::is_const_v<T_>, "read needs non-const lvalues");
//
//    using T = std::remove_cvref_t<T_>;
//    first = read<T>(is);
//
//    if constexpr (sizeof...(Args) > 0)
//        read(is, args...);
//}

template<typename T_, typename ... Args>
bool read(juce::InputStream& is, T_& first, Args& ... args)
{
    static_assert(!std::is_const_v<T_>, "read needs non-const lvalues");
    using T = std::remove_cvref_t<T_>;

    bool result = read<T>(is, first);
    
    if constexpr (sizeof...(Args) > 0)
        result |= read(is, args...);
    
    return result;
}
} //end namespace Readers
} //end namespace Stream
