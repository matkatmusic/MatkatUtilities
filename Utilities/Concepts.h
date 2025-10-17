/*
  ==============================================================================

    Concepts.h
    Created: 1 Sep 2025 11:43:57am
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//#include "UDP/PacketTransfer/TransmissionLocations.h"
enum class TransmissionLocation; //forward declaration

/*
 C++ concept usage:
 
 template<typename T, (optional) any other types...>
 concept ConceptName = requires(T t, (optional) other types...)
 {
    { some expression using t( (optional) types...); } -> std::same_as<expression return type >;
 
 }; //always a semicolon at the end
 
 then:
 
 use like this:
 
 template<ConceptName T>
 struct Foo
 {
    T t;
 
    void bar()
    {
        t.someFunction(); //guaranteed to exist because T satisfies ConceptName
    }
 };
 */

//============================================================================
// Has<Member Type> concepts
// these are concepts that check for the existence of certain nested types
//============================================================================
template<typename T>
concept HasType = requires
{
    typename T::Type;
};

template<typename T>
concept HasIDType = requires 
{
    typename T::IDType;
};

template<typename T>
concept HasInfoType = requires
{
    typename T::InfoType;
};

template<typename T>
concept HasOutputType = requires
{
    typename T::OutputType;
};

template<typename T>
concept HasInputType = requires { typename T::InputType; };

template<typename T>
concept HasKeyType = requires
{
    typename T::key_type;
};

template<typename T>
concept HasValueType = requires
{
    typename T::value_type;
};

template<typename T>
concept HasMappedType= requires
{
    typename T::mapped_type;
};

template<typename T>
concept HasIterator = requires
{
    typename T::iterator;
};

template<typename T>
concept HasConstIterator = requires
{
    typename T::const_iterator;
};
//============================================================================
// Has<Member Func> concepts
// these are concepts that check for the existence of certain member functions
//============================================================================

template<typename T>
concept HasGetID = HasIDType<T> && requires(const T& a)
{
    { a.getID() } -> std::same_as<typename T::IDType>;
};

template<typename T>
concept HasGetIndex = requires(T a)
{
    { a.getIndex() } -> std::same_as<juce::uint64>;
};

template<typename T>
concept HasEqualRange = HasKeyType<T> && HasIterator<T> && requires(T t)
{
    { t.equal_range(std::declval<typename T::key_type>()) } -> std::same_as<std::pair<typename T::iterator, typename T::iterator>>;
};

template<typename T>
concept HasPushBack = HasValueType<T> && requires(T t)
{
    { t.push_back(std::declval<typename T::value_type>()) };
};

template<typename T>
concept HasInsert = HasValueType<T> && requires(T t)
{
    { t.insert(std::declval<typename T::value_type>()) };
};

template<typename T>
concept HasClear = requires(T t)
{
    { t.clear() };
};

template<typename T>
concept HasReserve = requires(T t)
{
    { t.reserve(std::declval<size_t>()) };
};

template<typename T>
concept HasSize = requires(T t)
{
    { t.size() } -> std::same_as<size_t>;
};

template<typename T>
concept HasCapacity = requires(T t)
{
    { t.capacity() } -> std::same_as<size_t>;
};

template<typename T>
concept HasNonConstGetData = requires (T t)
{
    { t.getData() } -> std::same_as<void*>;
};

template<typename T>
concept HasConstGetData = requires (T t)
{
    { t.getData() } -> std::same_as<const void*>;
};

/**
 a type T is considered HasGetSize if it has a member function `getSize()` that returns a `size_t`
 */
template<typename T>
concept HasGetSize = requires (T t)
{
    { t.getSize() } -> std::same_as<size_t>;
};

/**
 a type T is considered a HasPull if it has a member function `pull()` that takes a reference to its nested type `Type` and returns a `bool`
 */
template<typename T>
concept HasPull = HasType<T> && requires(T t)
{
    { t.pull(std::declval<typename T::Type&>()) } -> std::same_as<bool>;
};

template<typename T>
concept HasGetNext = HasOutputType<T> && requires(T t, typename T::OutputType& output)
{
    { t.getNext(output) } -> std::same_as<bool>;
};

template<typename T>
concept HasIsPrepared = requires(T t)
{
    { t.isPrepared() } -> std::same_as<bool>;
};

template<typename T>
concept HasIsActivelyProducing = requires(T t)
{
    { t.isActivelyProducing() } -> std::same_as<bool>;
};

template<typename T>
concept HasGetLocationOfNext = requires(T t)
{
    { t.getLocationOfNext() } -> std::same_as<TransmissionLocation>;
};

/**
    a type T is considered a HasPush if it has a member function `push()` that takes a const reference to its nested type `Type` and returns a `bool`
 */
template<typename T>
concept HasPush = HasType<T> && requires(T t)
{
    { t.push(std::declval<typename T::Type>()) } -> std::same_as<bool>;
};

/**
    a type T is considered HasNumAvailableForReading if it has a member function `getNumAvailableForReading()` that returns an `int`
 */
template<typename T>
concept HasGetNumAvailableForReading = requires(T t)
{
    { t.getNumAvailableForReading() } -> std::same_as<int>;
};

//template<typename T>
//concept HasGetInfo = HasInfoType<T> && requires(T t)
//{
//    { t.getInfo() } -> std::same_as<typename T::InfoType>;
//};

template <typename T, typename TArg>
concept IsInputTypeFor = std::is_same_v<typename T::InputType, TArg>;

enum class UDPObjectType;

template<typename T>
concept HasUDPObjectType = requires
{
    { T::udpObjectType } -> std::convertible_to<UDPObjectType>;
};

template <typename T>
concept HasGetNumBytesRequired = requires
{
    { T::getNumBytesRequired() } -> std::same_as<size_t>;
};

template <typename T>
concept HasGetObject = requires(T t)
{
    { t.getObject() } -> std::same_as<typename T::OutputType>;
};

template <typename T>
concept HasGetObjectPtr = requires(T t)
{
    { t.getObjectPtr() } -> std::same_as<typename T::OutputType*>;
};

template<typename T>
concept HasGetBlock = requires(T t)
{
    { t.getBlock() } -> std::same_as<juce::MemoryBlock>;
};

template<typename T>
concept HasGetBlockSize = requires(T t)
{
    { t.getBlockSize() } -> std::same_as<size_t>;
};

/**
 A type T is considered a IsTaskWithBoolResult if it has a call operator() that takes (Args...) and returns a bool
 */
template<typename T, typename ... Args>
concept IsTaskWithBoolResult = requires(T t, Args ... args)
{
    { t(std::forward<Args>(args)...) } -> std::same_as<bool>;
};

template<typename T>
concept IsFifoType =
    HasType<T> &&
//    HasPull<T> &&
//    HasPush<T> &&
    HasGetNext<T> &&
    HasGetNumAvailableForReading<T>;

template<typename T>
concept IsSet = std::is_same_v<T, std::set<typename T::key_type>>;

template<typename T>
concept IsUnorderedSet = std::is_same_v<T, std::unordered_set<typename T::key_type>>;

template<typename T>
concept IsMultiset = std::is_same_v<T, std::multiset<typename T::key_type>>;

template<typename T>
concept IsUnorderedMultiset = std::is_same_v<T, std::unordered_multiset<typename T::key_type>>;

template<typename T>
concept IsAnySetType = IsSet<T> || IsUnorderedSet<T> || IsMultiset<T> || IsUnorderedMultiset<T>;



template<typename T>
concept IsVector = std::is_same_v<T, std::vector<typename T::value_type>>;

template<typename T>
concept IsMap = std::is_same_v<T, std::map<typename T::key_type, typename T::mapped_type>>;

template<typename T>
concept IsMultimap = std::is_same_v<T, std::multimap<typename T::key_type, typename T::mapped_type>>;

template<typename T>
concept IsUnorderedMultimap = std::is_same_v<T, std::unordered_multimap<typename T::key_type, typename T::mapped_type>>;

template<typename T>
concept IsUnorderedMap = std::is_same_v<T, std::unordered_map<typename T::key_type, typename T::mapped_type>>;

template<typename MapType>
concept IsAnyMapType = IsMap<MapType> ||
                       IsMultimap<MapType> ||
                       IsUnorderedMultimap<MapType> ||
                       IsUnorderedMap<MapType>;

/**
 a type `T` is considered IsSorterType for objects of type`ItemType` if it has the member function `compare(const ItemType&, const ItemType&)` and returns a`bool`
 */
template<typename T, typename ItemType>
concept IsSorterType = requires(T t,
                          const ItemType& a,
                          const ItemType& b)
{
    { t.compare(a, b) } -> std::same_as<bool>;
};

template<typename T>
concept ConvertibleToMemoryBlock = requires(T t)
{
    { t.toMemoryBlock() } -> std::same_as<juce::MemoryBlock>;
};

template<typename T>
concept ConvertibleFromMemoryBlock = requires(const juce::MemoryBlock& m)
{
    { T::fromMemoryBlock(m) } -> std::same_as<T>;
    
};

template<typename T>
concept ConvertibleFromMemoryBlockAndSize = requires(const juce::MemoryBlock& m)
{
    { T::fromMemoryBlock(m, std::declval<size_t>()) } -> std::same_as<T>;
};

//template<typename T>
//concept IsPacketType = HasGetID<T> && HasGetInfo<T>;
template<typename T>
concept HasBlockMember = requires(T t)
{
    { t.block } -> std::convertible_to<const juce::MemoryBlock&>;
};

struct TxKey;

template<typename T>
concept HasTxKeyMember = requires(T t)
{
    { t.key } -> std::convertible_to<const TxKey&>;
};

template<typename T>
concept IsSendableItem = HasBlockMember<T> && HasTxKeyMember<T>;

template<class V>
concept VectorOfSendableItems =
    IsVector<V> &&
    IsSendableItem<typename std::remove_cvref_t<V>::value_type>;

template<typename T>
concept HasGetSendItems = requires(T t)
{
    { t.getSendItems() } -> VectorOfSendableItems;
};

/**
 a type T is considered Sendable if:
 - it is convertible to a memory block
 - it has a getInfo() member function
 */
template<typename T>
concept Sendable = HasGetSendItems<T>;


template<typename T>
concept IsWriteBlockCompatible =
    HasNonConstGetData<T> &&
    HasGetSize<T>;

template<typename T>
concept IsSourceType =
    HasGetNext<T> &&
    HasIsPrepared<T> &&
    HasIsActivelyProducing<T>;


template<typename T, typename DataType>
concept SourceType = requires(T t, DataType& d)
{
    /*
     a SourceType is defined as an object that has a member function called getNext(). This function takes a reference to a DataType object and returns a bool indicating whether a new DataType was successfully retrieved.
     
        Additionally, the source must have a member function getLocationOfNext() that returns a TransmissionLocation enum value, indicating where the next data item is being retrieved from.
     */
    HasGetNext<T>;
    HasGetLocationOfNext<T>;
    HasGetNumAvailableForReading<T>;
};

template<typename T, typename DataType>
concept SenderType = requires(T t, const DataType& d)
{
    /*
     a sender is defined as an object with memeber function 'addToOutgoingQueue()' and getSentItems().
        addToOutgoingQueue() takes a const reference to a DataType object and returns a bool indicating whether the item was successfully added to the outgoing queue.
        getSentItems() returns a std::vector<DataType> containing all items that have been sent.
     */
    { t.addToOutgoingQueue(d) } -> std::same_as<bool>;
    { t.getSentItems() } -> std::same_as<std::vector<DataType>>;
    { t.getLocationOfSent() } -> std::same_as<TransmissionLocation>;
};

template<typename T>
concept TimerHandler = requires(T t)
{
    //T must be a valid type name when removing const or reference qualifiers
    typename std::remove_cvref_t<T>;
    //T must be a class.
    requires std::is_class_v<T>;
};

template<typename T>
concept IsContainerType =
    HasValueType<T> &&
    HasClear<T> &&
    HasSize<T> &&
    // HasReserve<T> &&
    // HasCapacity<T> &&
    HasIterator<T> &&
    HasConstIterator<T>;

/*
 the PostReceiveFunc::processReceivedBlock(receivedBlock) function does this:
 BlockWithID -> PacketType::fromMemoryBlock() -> PacketType
 */

template<typename T>
concept IsConvertibleBlockType =
    HasGetBlock<T> &&
    HasGetBlockSize<T>;

template<typename T, typename InputType, typename OutputType>
concept HasProcessReceivedBlock = requires (T t, InputType inputType)
{
    { T::processReceivedBlock(inputType) } -> std::same_as<OutputType>;
};

template<typename T, typename InputType, typename OutputType>
concept CanConvertBlockType = //requires(T t, const InputType& inputType)
//{
    IsConvertibleBlockType<InputType> &&
    
    (ConvertibleFromMemoryBlock<OutputType> || ConvertibleFromMemoryBlockAndSize<OutputType>) &&
    HasProcessReceivedBlock<T, InputType, OutputType>;
//};

template<typename T>
concept IsProducerType = HasOutputType<T> && requires (T t, typename T::OutputType& ot)
{
    { t.getNext(ot) } -> std::same_as<bool>;
};



template<typename T>
concept IsConsumerType = HasInputType<T> && requires (T t, const typename T::InputType& it)
{
    { t.add(it) } -> std::same_as<bool>;
};

template<typename T, typename Producer, typename Consumer>
concept ProducerConsumerConverterFunc = HasOutputType<Producer> && HasInputType<Consumer> && requires
{
    { T::convert(std::declval<typename Producer::OutputType>()) } -> std::same_as<typename Consumer::InputType>;
};
