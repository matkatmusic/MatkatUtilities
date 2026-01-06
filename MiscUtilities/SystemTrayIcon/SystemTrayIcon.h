/*
  ==============================================================================

    SystemTrayIcon.h
    Created: 7 Jul 2025 10:29:27am
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct SystemTrayIcon : public juce::SystemTrayIconComponent
{
    void mouseDown(const juce::MouseEvent& e) override;
};

#if JUCE_MAC //https://forum.juce.com/t/macos-app-as-background-process-with-system-tray-icon-and-native-menu/39905/3?u=matkatmusic
class DummyMenuBarModel final : public juce::MenuBarModel
{
public:
    DummyMenuBarModel();
    ~DummyMenuBarModel() override;
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex (int, const juce::String&) override;
    void menuItemSelected (int, int) override;
};
#endif
