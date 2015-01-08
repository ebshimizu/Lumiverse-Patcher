/*
  ==============================================================================

    ProfileEditorProperties.h
    Created: 8 Jan 2015 3:47:10pm
    Author:  eshimizu

  ==============================================================================
*/

#ifndef PROFILEEDITORPROPERTIES_H_INCLUDED
#define PROFILEEDITORPROPERTIES_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "LumiverseCore.h"

using namespace Lumiverse;

class DeleteParameterButton : public ButtonPropertyComponent
{
public:
  DeleteParameterButton(Device* d, string paramName, function<void()> callback) :
    ButtonPropertyComponent("Delete", true), _d(d), _paramName(paramName), _cb(callback)
  {
  }

  ~DeleteParameterButton() {}

  void buttonClicked() override {
    juce::AlertWindow w("Delete Parameter",
      "Are you sure you want to delete " + _paramName + "?",
      juce::AlertWindow::WarningIcon);

    w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    if (w.runModalLoop() != 0) // is they picked 'delete'
    {
      _d->deleteParameter(_paramName);
      _cb();
    }
  }

  String getButtonText() const {
    return "Delete";
  }

private:
  Device* _d;
  string _paramName;
  function<void()> _cb;
};

//==============================================================================
/*
*/
class LumiverseFloatProperty : public TextPropertyComponent
{
  // Indicates which part of the LumiverseFloat we're dealing with.
public:
  enum FloatPropAttr {
    DEFAULT, MIN, MAX
  };

  LumiverseFloatProperty(const String& propertyName, LumiverseFloat* data, FloatPropAttr attr) :
    TextPropertyComponent(propertyName, 50, false), _data(data), _attr(attr)
  {
  }

  ~LumiverseFloatProperty()
  {
  }

  void setText(const String &newText) override {
    float val = newText.getFloatValue();
    
    switch (_attr) {
    case (DEFAULT) :
      _data->setDefault(val);
      _data->setVal(val);
      break;
    case (MIN) :
      _data->setMin(val);
      break;
    case (MAX) :
      _data->setMax(val);
      break;
    default:
      break;
    }
  }

  String getText() const override {
    switch (_attr) {
    case (DEFAULT) :
      return String(_data->getDefault());
    case (MIN) :
      return String(_data->getMin());
    case (MAX) :
      return String(_data->getMax());
    default:
      return "";
    }
  }

private:
  LumiverseFloat* _data;
  FloatPropAttr _attr;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LumiverseFloatProperty)
};

class LumiverseOrientationProperty : public TextPropertyComponent
{
  // Indicates which part of the LumiverseFloat we're dealing with.
public:
  enum OriPropAttr {
    DEFAULT, MIN, MAX
  };

  LumiverseOrientationProperty(const String& propertyName, LumiverseOrientation* data, OriPropAttr attr) :
    TextPropertyComponent(propertyName, 50, false), _data(data), _attr(attr)
  {
  }

  ~LumiverseOrientationProperty()
  {
  }

  void setText(const String &newText) override {
    float val = newText.getFloatValue();

    switch (_attr) {
    case (DEFAULT) :
      _data->setDefault(val);
      _data->setVal(val);
      break;
    case (MIN) :
      _data->setMin(val);
      break;
    case (MAX) :
      _data->setMax(val);
      break;
    default:
      break;
    }
  }

  String getText() const override {
    switch (_attr) {
    case (DEFAULT) :
      return String(_data->getDefault());
    case (MIN) :
      return String(_data->getMin());
    case (MAX) :
      return String(_data->getMax());
    default:
      return "";
    }
  }

private:
  LumiverseOrientation* _data;
  OriPropAttr _attr;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseOrientationProperty)
};

class LumiverseOrientationUnits : public ChoicePropertyComponent
{
public:
  LumiverseOrientationUnits(const String& propertyName, LumiverseOrientation* data, function<void()> callback) :
    ChoicePropertyComponent(propertyName), _data(data), _cb(callback)
  {
    choices.add("Degrees");
    choices.add("Radians");
  }
  ~LumiverseOrientationUnits() {}

  void setIndex(int newIndex) override {
    if (newIndex == 0) {
      _data->setUnit(DEGREE);
    }
    else if (newIndex == 1) {
      _data->setUnit(RADIAN);
    }

    _cb();
  }

  int getIndex() const override {
    if (_data->getUnit() == DEGREE) {
      return 0;
    }
    else if (_data->getUnit() == RADIAN) {
      return 1;
    }
    else {
      return 0;
    }
  }

private:
  LumiverseOrientation* _data;
  function<void()> _cb;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseOrientationUnits);
};

#endif  // PROFILEEDITORPROPERTIES_H_INCLUDED
