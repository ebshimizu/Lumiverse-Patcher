/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "panels/DevicePanel.h"
#include "panels/PatchPanel.h"
#include <string>

using namespace std;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component, public ApplicationCommandTarget
{
public:
  //==============================================================================
  MainContentComponent();
  ~MainContentComponent();

  void paint (Graphics&);
  void resized();

  //==============================================================================
  // Application Command Target functions.
  ApplicationCommandTarget* getNextCommandTarget() override;
  void getAllCommands(Array<CommandID>& commands) override;
  void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
  bool perform(const InvocationInfo& info) override;

  //==============================================================================

  // reloads various parts of the GUI as needed. 
  void reload();

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)

  //==============================================================================
  // Application Command Target callbacks
  void open();
  void save();
  void saveAs();
  void openProfileEditor();
  void addPatch();

  string m_rigName;
  File m_parentDir;

  ScopedPointer<DevicePanel> m_dp;
  ScopedPointer<PatchPanel> m_pp;
  ScopedPointer<StretchableLayoutResizerBar> m_rb;
  SafePointer<Component> _profileEditorWindow;
  StretchableLayoutManager m_layout;
};


#endif  // MAINCOMPONENT_H_INCLUDED
