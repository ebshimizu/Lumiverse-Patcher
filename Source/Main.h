#ifndef _MAIN_H_
#define _MAIN_H_

/*
==============================================================================

This file was auto-generated by the Introjucer!

It contains the basic startup code for a Juce application.

==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "components/MainMenuComponent.h"

#include "LumiverseCore.h"
#include <memory>

using namespace std;
using namespace Lumiverse;
//==============================================================================
/*
This class implements the desktop window that contains an instance of
our MainContentComponent class.
*/
class MainWindow : public DocumentWindow
{
public:
  MainWindow(String name);

  ~MainWindow();

  void closeButtonPressed() override;

  /* Note: Be careful if you override any DocumentWindow methods - the base
  class uses a lot of them, so by overriding you might break its functionality.
  It's best to do all your work in your content component instead, but if
  you really have to override any DocumentWindow methods, make sure your
  subclass also calls the superclass's method.
  */

  static MainWindow* getMainWindow();

  // This thing will contain all the possible actions the application can do.
  static ApplicationCommandManager& getApplicationCommandManager();

  // Only one Rig can be open in the entire application at once, so we have a single
  // unique pointer for the rig.
  static unique_ptr<Rig>& getRig();

  // Mappings for commands
  enum CommandIDs
  {
    // File
    open = 0x0001,
    close = 0x0002,
    save = 0x0003,
    saveAs = 0x0006,

    // Device
    openProfileEditor = 0x1001
  };

  // Apparently we just leave this here to implement tooltips?
  TooltipWindow tooltips;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
  ScopedPointer<MainMenuComponent> mainMenu;
};

#endif