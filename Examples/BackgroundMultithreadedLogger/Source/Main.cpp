/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>

#include <LoggerWithOptionalCout.h>

#include <SystemTrayIcon.h>
#include <BackgroundMultiuserLogger.h>

struct BackgroundJob : juce::Thread
{
    BackgroundJob(int num);
    
    void run() override;
    
    int counter = 10;
};

BackgroundJob::BackgroundJob(int num) : juce::Thread(juce::String("BackgroundJob_") + juce::String(num))
{
    startThread();
}

void BackgroundJob::run()
{
    BML::writeToLog(getThreadName() + " has started running" );
    while( threadShouldExit() == false )
    {
        if( counter == 0 )
        {
            break;
        }
        
        BML::writeToLog(getThreadName() + " decrementing the counter. remaining: " + juce::String(counter) );
        wait(500);
        --counter;
    }
    
    BML::writeToLog(getThreadName() + " has finished running" );
}

class LoggerExample : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override;
    const juce::String getApplicationVersion() override;
    
    ~LoggerExample() override;
    
    void initialise (const juce::String& commandLineParameters) override;
    
    void shutdown() override;
    void systemRequestedQuit() override;
    bool moreThanOneInstanceAllowed() override;
    
    void unhandledException (const std::exception*,
                                     const juce::String& sourceFilename,
                             int lineNumber) override;
private:
    std::unique_ptr<LoggerWithOptionalCout> fileLogger;
    void configureLogger();
#if JUCE_MAC
    std::unique_ptr<DummyMenuBarModel> model;
#endif
    std::unique_ptr<SystemTrayIcon> systemTrayIcon;
    
    std::vector< std::unique_ptr<BackgroundJob> > backgroundJobs;
};
//==============================================================================
LoggerExample::~LoggerExample()
{
    
}

const juce::String LoggerExample::getApplicationName()     { return ProjectInfo::projectName; }
const juce::String LoggerExample::getApplicationVersion()  { return ProjectInfo::versionString; }

#if JUCE_MAC
void signalHandler(int signal)
{
    if( signal == SIGINT )
    {
        BML::writeToLog("Received SIGINT signal, exiting gracefully...");
        if( auto instance = juce::JUCEApplication::getInstance() )
        {
            instance->systemRequestedQuit();
        }
    }
    else if( signal == SIGTERM )
    {
        BML::writeToLog("Received SIGTERM signal, exiting gracefully...");
        if( auto instance = juce::JUCEApplication::getInstance() )
        {
            instance->systemRequestedQuit();
        }
    }
    else
    {
        BML::writeToLog("Received unknown signal: " + juce::String(signal) + ", exiting gracefully...");
        if( auto instance = juce::JUCEApplication::getInstance() )
        {
            instance->systemRequestedQuit();
        }
    }
}
#endif

void LoggerExample::initialise (const juce::String& commandLineParameters)
{
    /*
     set up logging
     */
    configureLogger();
    
#if JUCE_MAC
    model = std::make_unique<DummyMenuBarModel>();

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
#endif
    
    systemTrayIcon = std::make_unique<SystemTrayIcon>();
    juce::Image iconImage = juce::ImageFileFormat::loadFrom(BinaryData::tray_icon_jpg, BinaryData::tray_icon_jpgSize);
    if( iconImage.isNull() )
    {
        jassertfalse; //failed to load tray icon image!
    }
    else
    {
        systemTrayIcon->setIconImage(iconImage, iconImage);
    }
    
    auto args = juce::ArgumentList(ProjectInfo::projectName, commandLineParameters);
    
    BML::writeToLog("Command line arguments: " + commandLineParameters);
    
    BML::writeToLog("Launching 10 Background Jobs");
    for( int i = 0; i < 10; ++i )
    {
        backgroundJobs.push_back( std::make_unique<BackgroundJob>(i) );
    }
}

bool LoggerExample::moreThanOneInstanceAllowed()
{
    return true;
}

void LoggerExample::configureLogger()
{
    auto welcomeMessage = juce::String("Welcome to ") + ProjectInfo::projectName;
    welcomeMessage << " ";
    welcomeMessage << ProjectInfo::versionString;
    welcomeMessage << " spawned at ";
    welcomeMessage << juce::Time::getCurrentTime().toISO8601(true);
    
    auto logger = std::unique_ptr<juce::FileLogger>(juce::FileLogger::createDateStampedLogger(ProjectInfo::projectName, "session", ".log", welcomeMessage));
    fileLogger = std::make_unique<LoggerWithOptionalCout>(true, std::move(logger));
}

void LoggerExample::systemRequestedQuit()
{
    BML::writeToLog("LoggerExample::systemRequestedQuit()");
    
    BML::writeToLog("Shutting down background jobs");
    for( auto& job : backgroundJobs )
    {
        if( job )
            job->stopThread(1000);
    }
    
    quit();
}

void LoggerExample::shutdown()
{
    BML::writeToLog("UDPSender::shutdown()");
    
    BML::printAllRemainingMessages();
    BML::deleteInstance();
    /*
     this should be the last thing you do in your shutdown method.
     */
    juce::Logger::setCurrentLogger(nullptr);
}

void LoggerExample::unhandledException(const std::exception* e,
                                    const juce::String& sourceFilename,
                            int lineNumber)
{
    juce::String message;
    message << "Exception thrown: " << juce::NewLine();
    message << "file: " << sourceFilename << juce::NewLine();
    message << "line: " << lineNumber << juce::NewLine();
    if( e != nullptr )
    {
        message << "message: " << juce::String(e->what()) << juce::NewLine();
    }
    
    BML::writeToLog(message);
}

START_JUCE_APPLICATION(LoggerExample)
