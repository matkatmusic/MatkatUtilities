/*
  ==============================================================================

    ReaderWriterTests.cpp
    Created: 8 Jan 2026 1:37:27pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "ReaderWriterTests.h"

#include "Readers.h"
#include "Writers.h"

StreamReaderWriterTests::StreamReaderWriterTests() : juce::UnitTest("StreamReaderWriterTests")
{
    
}

void StreamReaderWriterTests::runTest()
{
    beginTest("Int 8 Reader/Writer Test");
#pragma mark Int8 Reader/Writer test
    {
        juce::MemoryBlock mb;
        const juce::int8 testVal = 8;
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, testVal);
        }
        {
            juce::MemoryInputStream input(mb, false);
            std::remove_cv<decltype(testVal)>::type val;
            Stream::Readers::read(input, val);
            expect(val == testVal);
        }
    }
#pragma mark Int32 Reader/Writer test
    {
        juce::MemoryBlock mb;
        const juce::int32 testVal = -8;
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, testVal);
        }
        {
            juce::MemoryInputStream input(mb, false);
            std::remove_cv<decltype(testVal)>::type val;
            Stream::Readers::read(input, val);
            jassert(val == testVal);
        }
    }
#pragma mark Uint64 Reader/Writer test
    {
        juce::MemoryBlock mb;
        const juce::uint64 testVal = 888888888;
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, testVal);
        }
        {
            juce::MemoryInputStream input(mb, false);
            std::remove_cv<decltype(testVal)>::type val;
            Stream::Readers::read(input, val);
            jassert(val == testVal);
        }
    }
#pragma mark Float32 Reader/Writer test
    {
        juce::MemoryBlock mb;
        const float testVal = 8.888f;
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, testVal);
        }
        {
            juce::MemoryInputStream input(mb, false);
            std::remove_cv<decltype(testVal)>::type val;
            Stream::Readers::read(input, val);
            jassert(val == testVal);
        }
    }
#pragma mark String Reader/Writer test
    {
        juce::MemoryBlock mb;
        const juce::String testVal = "8.8888f";
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, testVal);
        }
        {
            juce::MemoryInputStream input(mb, false);
            std::remove_cv<decltype(testVal)>::type val;
            Stream::Readers::read(input, val);
            jassert(val == testVal);
        }
    }
#pragma mark Block Reader/Writer test
    {
        juce::MemoryBlock mb;
        juce::String testVal {"8.8888f"};
        const auto testBlock = juce::MemoryBlock(testVal.toRawUTF8(), testVal.getNumBytesAsUTF8());
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, testBlock);
        }
        {
            juce::MemoryInputStream input(mb, false);
            std::remove_cv<decltype(testBlock)>::type val;
            Stream::Readers::read(input, val);
            jassert(val == testBlock);
        }
    }
#pragma mark multi value Reader/WriterTest
    {
        juce::MemoryBlock mb;
        const juce::int8 testVal1 = 8;
        const juce::int32 testVal2 = -8;
        const juce::uint64 testVal3 = 888888888;
        const float testVal4 = 8.888f;
        const juce::String testVal5 = "8.8888f";
        const auto testVal6 = juce::MemoryBlock(testVal5.toRawUTF8(), testVal5.getNumBytesAsUTF8());

        //testing writing them in order
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output,
                                   testVal1,
                                   testVal2,
                                   testVal3,
                                   testVal4,
                                   testVal5,
                                   testVal6);
        }
        {
            juce::MemoryInputStream input(mb, false);
            auto val1 = Stream::Readers::read<std::remove_cv_t<decltype(testVal1)>>(input);
            jassert(val1 == testVal1);
            auto val2 = Stream::Readers::read<std::remove_cv_t<decltype(testVal2)>>(input);
            jassert(val2 == testVal2);
            auto val3 = Stream::Readers::read<std::remove_cv_t<decltype(testVal3)>>(input);
            jassert(val3 == testVal3);
            auto val4 = Stream::Readers::read<std::remove_cv_t<decltype(testVal4)>>(input);
            jassert(val4 == testVal4);
            auto val5 = Stream::Readers::read<std::remove_cv_t<decltype(testVal5)>>(input);
            jassert(val5 == testVal5);
            auto val6 = Stream::Readers::read<std::remove_cv_t<decltype(testVal6)>>(input);
            jassert(val6 == testVal6);
        }

        //testing writing them in random order: 5 4 1 3 6 2
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output,
                                   testVal5,
                                   testVal4,
                                   testVal1,
                                   testVal3,
                                   testVal6,
                                   testVal2);
        }

        {
            juce::MemoryInputStream input(mb, false);
            auto val5 = Stream::Readers::read<std::remove_cv_t<decltype(testVal5)>>(input);
            jassert(val5 == testVal5);
            auto val4 = Stream::Readers::read<std::remove_cv_t<decltype(testVal4)>>(input);
            jassert(val4 == testVal4);
            auto val1 = Stream::Readers::read<std::remove_cv_t<decltype(testVal1)>>(input);
            jassert(val1 == testVal1);
            auto val3 = Stream::Readers::read<std::remove_cv_t<decltype(testVal3)>>(input);
            jassert(val3 == testVal3);
            auto val6 = Stream::Readers::read<std::remove_cv_t<decltype(testVal6)>>(input);
            jassert(val6 == testVal6);
            auto val2 = Stream::Readers::read<std::remove_cv_t<decltype(testVal2)>>(input);
            jassert(val2 == testVal2);
        }
    }

    // New tests to cover additional template branches in Readers/Writers
    beginTest("Unsigned 8 and 16 Reader/Writer Tests");
    {
        // uint8
        juce::MemoryBlock mb;
        const juce::uint8 u8 = 200;
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, u8);
        }
        {
            juce::MemoryInputStream input(mb, false);
            std::remove_cv<decltype(u8)>::type val;
            Stream::Readers::read(input, val);
            expect(val == u8);
        }

        // uint16
        mb.reset();
        const juce::uint16 u16 = 65530;
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, u16);
        }
        {
            juce::MemoryInputStream input(mb, false);
            std::remove_cv<decltype(u16)>::type val;
            Stream::Readers::read(input, val);
            expect(val == u16);
        }
    }

    beginTest("Enum Reader/Writer Test");
    {
        enum class LocalEnum : juce::int32 { A = 1, B = -2 };
        juce::MemoryBlock mb;
        const LocalEnum e = LocalEnum::B;
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, e);
        }
        {
            juce::MemoryInputStream input(mb, false);
            LocalEnum val;
            Stream::Readers::read(input, val);
            expect(static_cast<juce::int32>(val) == static_cast<juce::int32>(e));
        }
    }

    beginTest("Uuid Reader/Writer Test");
    {
        juce::MemoryBlock mb;
        auto testUuid = juce::Uuid();
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, testUuid);
        }
        {
            juce::MemoryInputStream input(mb, false);
            auto val = Stream::Readers::read<juce::Uuid>(input);
            expect(val == testUuid);
        }
    }

    beginTest("Container Reader Test (std::vector<int32>)");
    {
        // construct a memory block that matches the format readContainer expects
        std::vector<juce::int32> vec { 10, 20, -30 };
        juce::MemoryBlock mb;
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, vec);
        }
        {
            juce::MemoryInputStream input(mb, false);
            auto readVec = Stream::Readers::read<decltype(vec)>(input);
            expect(readVec == vec);
        }
    }

    beginTest("HasWriteToStream/HasReadFromStream Custom Type Test");
    {
        struct TestSerializable
        {
            using Ptr = TestSerializable; // concept expects T::Ptr type
            int v = 0;

            static bool writeToStream(const TestSerializable& t, juce::OutputStream& os)
            {
                return Stream::Writers::write(os, t.v);
            }

            static Ptr readFromStream(juce::InputStream& is)
            {
                Ptr p;
                Stream::Readers::read(is, p.v);
                return p;
            }
        };

        juce::MemoryBlock mb;
        TestSerializable t{ 12345 };
        {
            auto output = juce::MemoryOutputStream(mb, false);
            // exercise writer path that calls T::writeToStream
            Stream::Writers::write(output, t);
        }
        {
            juce::MemoryInputStream input(mb, false);
            // exercise reader path that calls T::readFromStream
            auto readObj = Stream::Readers::read<TestSerializable>(input);
            expect(readObj.v == t.v);
        }
    }

    beginTest("Mixed variadic Reader/Writer recursive paths");
    {
        // combine several types to exercise recursive variadic writer/reader
        juce::MemoryBlock mb;

        enum class VE : juce::int8 { X = 5 };
        VE e = VE::X;
        juce::uint8 u8 { 250 };
        juce::uint16 u16 { 60000 };
        juce::Uuid uuid;
        std::vector<juce::int32> vec { 1, 2, 3 };

        struct S
        {
            using Ptr = S;
            juce::int32 v = 77;
            static bool writeToStream(const S& s, juce::OutputStream& os)
            {
                return Stream::Writers::write(os, s.v);
            }
            
            static Ptr readFromStream(juce::InputStream& is)
            {
                Ptr p;
                Stream::Readers::read(is, p.v);
                return p;
            }
        };
        S s{ 79 };

        // write primitives and serializable S via the variadic writer
        {
            auto output = juce::MemoryOutputStream(mb, false);
            Stream::Writers::write(output, e, u8, u16, uuid, s, vec);
        }

        {
            juce::MemoryInputStream input(mb, false);
            auto re = Stream::Readers::read<decltype(e)>(input);
            auto ru8 = Stream::Readers::read<decltype(u8)>(input);
            auto ru16 = Stream::Readers::read<decltype(u16)>(input);
            auto ruuid = Stream::Readers::read<decltype(uuid)>(input);
            auto rs = Stream::Readers::read<decltype(s)>(input);
            auto rvec = Stream::Readers::read<decltype(vec)>(input);

            expect(re == e);
            expect(ru8 == u8);
            expect(ru16 == u16);
            expect(ruuid == uuid);
            expect(rs.v == s.v);
            expect(rvec == vec);
        }
    }
}
