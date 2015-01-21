#ifndef _CONTROLSPANEL_H_
#define _CONTROLSPANEL_H_

#pragma once

#include "LumiverseCore.h"
#include <LumiverseShowControl/LumiverseShowControl.h>

#include "LumiverseEnumPropertyComponent.h"
#include "../JuceLibraryCode/JuceHeader.h"

using namespace Lumiverse;
using namespace Lumiverse::ShowControl;

class CueTable;

enum UpdateSource {
  TABLE, PROG, CUE, CMD, CTL
};

//==============================================================================
class LumiverseFloatPropertySlider : public SliderPropertyComponent
{
public:
  LumiverseFloatPropertySlider(string propertyName, DeviceSet devices, LumiverseFloat* val);
  void setValue(double newValue) override;
  double getValue() const override { return val; }
  void sliderDragEnded(Slider* slider) override;

private:
  Lumiverse::DeviceSet devices;
  string param;
  double val;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseFloatPropertySlider)
};

//==============================================================================
class LumiverseOrientationPropertySlider : public SliderPropertyComponent
{
public:
  LumiverseOrientationPropertySlider(string propertyName, DeviceSet devices, LumiverseOrientation* val);
	void setValue(double newValue) override;
	double getValue() const override { return val; }
  void sliderDragEnded(Slider* slider) override;

private:
	Lumiverse::DeviceSet devices;
	string param;
	double val;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseOrientationPropertySlider)
};

//==============================================================================
class LumiverseColorPropertySlider : public SliderPropertyComponent
{
public:
  LumiverseColorPropertySlider(string propertyName, string channelName, DeviceSet devices, double val);

  void setValue(double newVal) override;
  double getValue() const override { return m_val; }
  void sliderDragEnded(Slider* slider) override;

private:
  Lumiverse::DeviceSet m_devices;
  string m_param;
  string m_channel;
  double m_val;
};

//==============================================================================
class MetadataPropertyTextEditor : public TextPropertyComponent
{
public:
	MetadataPropertyTextEditor(string propertyName, DeviceSet devices);
	void setText(const String &newText) override;
	String 	getText() const override { return m_val; }

private:
	string combineMetadataWithSameKey(set<string> metadata);
	Lumiverse::DeviceSet m_devices;
	string m_metadata;
	string m_val;
};

//==============================================================================
class MetadataPropertyButton : public ButtonPropertyComponent
{
public:
	MetadataPropertyButton(DeviceSet devices)
		: m_devices(devices), ButtonPropertyComponent(" ", true) { }
	void buttonClicked() override;
	String getButtonText() const override { return "Add";  }

private:
	Lumiverse::DeviceSet m_devices;
};

//==============================================================================
class DMXPatchAddrProperty : public TextPropertyComponent
{
public:
  DMXPatchAddrProperty(string patchID, string deviceID);
  void setText(const String& newText) override;
  String getText() const override;
private:
  string _patchID;
  string _deviceID;
};

//==============================================================================
class DMXPatchUniverseProperty : public TextPropertyComponent
{
public:
  DMXPatchUniverseProperty(string patchID, string deviceID);
  void setText(const String& newText) override;
  String getText() const override;
private:
  string _patchID;
  string _deviceID;
};

//==============================================================================
// Component that displays the properties of a selected device
// or selected set of devices.
class ControlsPanel : public Component
{
public:
  ControlsPanel();
  ~ControlsPanel();

  void paint(Graphics& g);
  void resized();

  // This panel only has to be updated when the selection changes. This function handles that.
  void updateProperties(Lumiverse::DeviceSet activeDevices);

private:
  PropertyPanel properties;
};

#endif