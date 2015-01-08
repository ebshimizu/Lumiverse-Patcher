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


#endif  // PROFILEEDITORPROPERTIES_H_INCLUDED
