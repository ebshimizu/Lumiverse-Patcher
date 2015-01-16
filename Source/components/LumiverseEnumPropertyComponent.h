#ifndef _LUMIVERSEENUMPROPERTYCOMPONENT_H_
#define _LUMIVERSEENUMPROPERTYCOMPONENT_H_

#pragma once

#include "LumiverseCore.h"
#include <LumiverseShowControl/LumiverseShowControl.h>

#include "../JuceLibraryCode/JuceHeader.h"

using namespace Lumiverse;
using namespace Lumiverse::ShowControl;

enum UpdateSource;
class CueTable;

class LumiverseEnumPropertyComponent : public PropertyComponent, private SliderListener, private ComboBoxListener
{
public:
  LumiverseEnumPropertyComponent(string name, DeviceSet devices, LumiverseEnum* data);
  ~LumiverseEnumPropertyComponent();

  void refresh();
  void sliderValueChanged(Slider* slider);
  void sliderDragEnded(Slider* slider) override;
  void comboBoxChanged(ComboBox* box);
  void resized() override;
  void paint(Graphics& g) override;

protected:
  Slider m_slider;
  ComboBox m_modeBox;
  ComboBox m_interpModeBox;
  ComboBox m_valBox;
  Lumiverse::DeviceSet m_devices;

  // The actual values.
  float m_tweak;
  int m_val;
  map<int, string> m_vals;    // IDs are offset by +1 from the actual index of the enum
  LumiverseEnum::Mode m_mode;
  LumiverseEnum::InterpolationMode m_interpMode;

  // Possible values for the enumeration
  StringArray m_values;

  // If we're looking at a mixed type selection, we probably shouldn't
  // display anything for this enumeration since the intervals might not match up.
  bool m_mixedType;

  UpdateSource m_src;
  CueTable* m_ct;

  void updateData();
};

#endif