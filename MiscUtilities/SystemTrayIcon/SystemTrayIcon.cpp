/*
  ==============================================================================

    SystemTrayIcon.cpp
    Created: 7 Jul 2025 10:29:27am
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "SystemTrayIcon.h"

void SystemTrayIcon::mouseDown(const juce::MouseEvent& e)
{
    juce::PopupMenu menu;
    menu.addItem("Quit", true, false, [this]()
                 {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    });
    
#if JUCE_MAC
    showDropdownMenu(menu);
#elif JUCE_WINDOWS
    //TODO: why doesn't this appear on windows?
    menu.showMenuAsync(juce::PopupMenu::Options().withParentComponent(this).withStandardItemHeight(18).withMinimumWidth(100));
#endif
}

#if JUCE_MAC
DummyMenuBarModel::DummyMenuBarModel()
{
    juce::MenuBarModel::setMacMainMenu(this);
}
DummyMenuBarModel::~DummyMenuBarModel()
{
    juce::MenuBarModel::setMacMainMenu (nullptr);
}

juce::StringArray DummyMenuBarModel::getMenuBarNames()  { return {""}; }
juce::PopupMenu DummyMenuBarModel::getMenuForIndex (int, const juce::String&)  { return juce::PopupMenu(); }
void DummyMenuBarModel::menuItemSelected (int, int)  {}
#endif
