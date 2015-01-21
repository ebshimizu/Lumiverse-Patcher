/*
  ==============================================================================

    DevicePanel.h
    Created: 6 Jan 2015 3:21:40pm
    Author:  eshimizu

  ==============================================================================
*/

#ifndef DEVICEPANEL_H_INCLUDED
#define DEVICEPANEL_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../components/DeviceTable.h"
#include "../components/ControlsPanel.h"
#include "LumiverseCore.h"

using namespace Lumiverse;
//==============================================================================
/*
*/
class DevicePanel    : public Component
{
public:
  DevicePanel();
  ~DevicePanel();

  void paint (Graphics&);
  void resized();

  void reload();

  DeviceSet getSelectedDevices();
  void updateSelectedDevices(DeviceSet d);

private:
  StretchableLayoutManager _layout;
  ScopedPointer<ControlsPanel> _cp;
  ScopedPointer<DeviceTable> _dt;
  ScopedPointer<StretchableLayoutResizerBar> _rb;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DevicePanel)
};


#endif  // DEVICEPANEL_H_INCLUDED
