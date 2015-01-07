/*
  ==============================================================================

    PatchPanel.h
    Created: 6 Jan 2015 3:22:02pm
    Author:  eshimizu

  ==============================================================================
*/

#ifndef PATCHPANEL_H_INCLUDED
#define PATCHPANEL_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../components/DMXPatchViewer.h"

//==============================================================================
/*
*/
class PatchPanel : public Component
{
public:
  PatchPanel();
  ~PatchPanel();

  void reload();

  void paint (Graphics&);
  void resized();

private:
  ScopedPointer<TabbedComponent> _tc;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchPanel)
};


#endif  // PATCHPANEL_H_INCLUDED
