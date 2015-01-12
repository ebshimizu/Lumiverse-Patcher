/*
  ==============================================================================

    ProfileEditor.cpp
    Created: 8 Jan 2015 2:29:43pm
    Author:  eshimizu

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "ProfileEditor.h"
#include "../components/ProfileEditorProperties.h"

//==============================================================================
ProfileEditorComponent::ProfileEditorComponent()
{
  _device = new Device("template", 0, "[New Device Type]");
  _device->setParam("enum", (LumiverseType*)new LumiverseEnum());

  // When this initializes we don't have a file assigned.
  addAndMakeVisible(_browse = new TextButton("Open...", "Find a Profile to Open"));
  _browse->setName("Browse");
  _browse->addListener(this);

  addAndMakeVisible(_addParam = new TextButton("Add Parameter", "Add a new Parameter"));
  _addParam->setName("AddParam");
  _addParam->addListener(this);

  addAndMakeVisible(_params = new PropertyPanel("Parameters"));
  reloadParameters();
}

ProfileEditorComponent::~ProfileEditorComponent()
{
  delete _device;
  _browse = nullptr;
}

void ProfileEditorComponent::close() {

}

void ProfileEditorComponent::paint (Graphics& g)
{
  g.fillAll (Colours::grey);   // clear the background

  auto area = getLocalBounds();
  area.reduce(3, 3);

  g.setColour(Colours::black);
  Font bold(16, Font::bold);
  
  g.setFont(bold);

  String text;
  if (!_profile.exists()) {
    text = "New Profile";
  }
  else {
    text = "Profile: " + _profile.getFullPathName();
  }
  auto topRec = area.removeFromTop(30);
  topRec.removeFromRight(100);
  g.drawFittedText(text, topRec, Justification::left, 1);

  g.setFont(16);
  g.drawFittedText("Parameters", area.removeFromTop(30), Justification::centredLeft, 1);
}

void ProfileEditorComponent::resized()
{
  auto area = getLocalBounds();

  // Filename and browse button on top.
  area.reduce(3, 3);
  _browse->setBounds(area.removeFromTop(30).removeFromRight(100));

  _addParam->setBounds(area.removeFromTop(30).removeFromBottom(20).removeFromRight(100));
  _params->setBounds(area.removeFromTop(300));
}

void ProfileEditorComponent::buttonClicked(Button* b) {
  if (b->getName() == "AddParam") {
    addParam();
  }
}

void ProfileEditorComponent::reloadParameters() {
  _params->clear();

  // Clear everything in this map
  for (auto kvp : _paramGUIs) {
    kvp.second.clear();
  }
  _paramGUIs.clear();

  // For each paramter we want to add a section that lets us edit it
  for (auto p : _device->getParamNames()) {
    LumiverseType* data = _device->getParam(p);
    String sectionName = p + ": " + data->getTypeName();

    if (data->getTypeName() == "float") {
      LumiverseFloat* fData = (LumiverseFloat*)(data);
      _paramGUIs[p].add(new LumiverseFloatProperty("Default", fData, LumiverseFloatProperty::DEFAULT));
      _paramGUIs[p].add(new LumiverseFloatProperty("Min", fData, LumiverseFloatProperty::MIN));
      _paramGUIs[p].add(new LumiverseFloatProperty("Max", fData, LumiverseFloatProperty::MAX));
    }
    else if (data->getTypeName() == "orientation") {
      LumiverseOrientation* oData = (LumiverseOrientation*)(data);
      _paramGUIs[p].add(new LumiverseOrientationProperty("Default", oData, LumiverseOrientationProperty::DEFAULT));
      _paramGUIs[p].add(new LumiverseOrientationProperty("Min", oData, LumiverseOrientationProperty::MIN));
      _paramGUIs[p].add(new LumiverseOrientationProperty("Max", oData, LumiverseOrientationProperty::MAX));
      _paramGUIs[p].add(new LumiverseOrientationUnits("Units", oData, [p,this]{ this->reloadAll(p); }));
    }
    else if (data->getTypeName() == "color") {
      LumiverseColor* cData = (LumiverseColor*)(data);
      _paramGUIs[p].add(new LumiverseColorModeProperty("Mode", cData, [this]{ this->reloadParameters(); }));
      _paramGUIs[p].add(new LumiverseColorWeightProperty("Weight", cData));
      if (cData->getMode() != BASIC_RGB && cData->getMode() != BASIC_CMY) {
        _paramGUIs[p].add(new LumiverseColorChannelsProperty("Channels", cData, [p, this]{ this->reloadAll(p); this->_params->resized(); }));
        if (cData->getBasisVectors().size() > 0) {
          _paramGUIs[p].add(new LumiverseColorBasisVectorsProperty("Color Basis Vectors", cData, []{}));
        }
        _paramGUIs[p].add(new LumiverseColorBasisVectorButton(cData, [this]{ this->reloadParameters(); }));
      }
    }
    else if (data->getTypeName() == "enum") {
      LumiverseEnum* eData = (LumiverseEnum*)data;
      _paramGUIs[p].add(new LumiverseEnumDefaultProperty("Default", eData));
      _paramGUIs[p].add(new LumiverseEnumTweakProperty("Tweak", eData));
      _paramGUIs[p].add(new LumiverseEnumModeProperty("Mode", eData));
      _paramGUIs[p].add(new LumiverseEnumInterpProperty("Interpolation Mode", eData));
      _paramGUIs[p].add(new LumiverseEnumRangeMaxProperty("Max Value", eData));
      _paramGUIs[p].add(new LumiverseEnumValuesProperty("Options", eData, [p, this]{ this->reloadAll(p); this->_params->resized(); }));
    }

    _paramGUIs[p].add(new DeleteParameterButton(_device, p, [&]{ reloadParameters(); }));

    _params->addSection(sectionName, _paramGUIs[p], true);
  }
}

void ProfileEditorComponent::addParam() {
  juce::AlertWindow w("Add Parameter",
    "Add a Parameter to the Profile.",
    juce::AlertWindow::QuestionIcon);

  StringArray types;
  types.add("Float");
  types.add("Enumeration");
  types.add("Color");
  types.add("Orientation");

  w.addTextEditor("name", "", "Name");
  w.addComboBox("types", types, "Type");

  w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    // this is the item they chose in the drop-down list..
    const int typeChosen = w.getComboBoxComponent("types")->getSelectedItemIndex();
    string name = w.getTextEditor("name")->getText().toStdString();

    if (_device->getParam(name) != nullptr) {
      juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
        "Unable to add Parameter",
        "A parameter with the name \"" + name + "\" already exists.",
        "OK");
      return;
    }

    if (typeChosen == 0) {
      // Float
      _device->setParam(name, (LumiverseType*)new LumiverseFloat());
    }
    else if (typeChosen == 1) {
      // Enum
      _device->setParam(name, (LumiverseType*)new LumiverseEnum());
    }
    else if (typeChosen == 2) {
      _device->setParam(name, (LumiverseType*)new LumiverseColor());
    }
    else if (typeChosen == 3) {
      // Orientation
      _device->setParam(name, (LumiverseType*)new LumiverseOrientation());
    }

    reloadParameters();
  }
}

void ProfileEditorComponent::reloadAll(string paramName) {
  auto components = _paramGUIs[paramName];

  for (auto c : components) {
    auto pc = (PropertyComponent*)c;
    pc->refresh();
  }
}

//==============================================================================
ProfileEditor::ProfileEditor(const String& name, Colour backgroundColour, int buttonsNeeded) : 
  DocumentWindow(name, backgroundColour, buttonsNeeded)
{
  _pec = new ProfileEditorComponent();
  setContentOwned(_pec, false);
}

ProfileEditor::~ProfileEditor()
{
  _pec = nullptr;
}

void ProfileEditor::closeButtonPressed()
{
  _pec->close();
  delete this;
}