/*
  ==============================================================================

    DMXPatchViewer.h
    Created: 7 Jan 2015 5:15:35pm
    Author:  eshimizu

  ==============================================================================
*/

#ifndef DMXPATCHVIEWER_H_INCLUDED
#define DMXPATCHVIEWER_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "DMXMap.h"

//==============================================================================
/*
*/
class DMXPatchViewer : public Component, private ComboBoxListener
{
public:
  DMXPatchViewer(string patchID);
  ~DMXPatchViewer();

  void reload();
  void changeUniverse(int uni);

  void paint (Graphics&);
  void resized();

  // Callback for the combob box.
  void comboBoxChanged(ComboBox* box);

private:
  ComboBox _universeList;
  string _patchID;

  ScopedPointer<DMXMap> _dm;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DMXPatchViewer)
};


#endif  // DMXPATCHVIEWER_H_INCLUDED
