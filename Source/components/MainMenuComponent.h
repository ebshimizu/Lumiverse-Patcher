#ifndef _MAINMENUCOMPONENT_H_
#define _MAINMENUCOMPONENT_H_

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class MainMenuComponent : public juce::Component, public MenuBarModel
{
public:
  MainMenuComponent();
  ~MainMenuComponent();

  StringArray getMenuBarNames() override;
  PopupMenu getMenuForIndex(int menuIndex, const String& /*menuName*/) override;
  void menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/) override;

private:
  ScopedPointer<MenuBarComponent> menuBar;
};

#endif