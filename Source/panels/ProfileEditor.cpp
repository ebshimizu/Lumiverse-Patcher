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
#include "../Main.h"

//==============================================================================
ProfileEditorComponent::ProfileEditorComponent()
{
  _device = new Device("template", 0, "[New Device Type]");

  // When this initializes we don't have a file assigned.
  addAndMakeVisible(_browse = new TextButton("Open...", "Find a Profile to Open"));
  _browse->setName("Browse");
  _browse->addListener(this);

  addAndMakeVisible(_addParam = new TextButton("Add Parameter", "Add a new Parameter"));
  _addParam->setName("AddParam");
  _addParam->addListener(this);

  addAndMakeVisible(_addMetadata = new TextButton("Add Metadata", "Add a new Metadata Field"));
  _addMetadata->setName("AddMetadata");
  _addMetadata->addListener(this);

  addAndMakeVisible(_saveAsButton = new TextButton("Save As...", "Save this profile as a different file."));
  _saveAsButton->setName("SaveAs");
  _saveAsButton->addListener(this);

  addAndMakeVisible(_saveButton = new TextButton("Save", "Save this profile."));
  _saveButton->setName("Save");
  _saveButton->addListener(this);

  addAndMakeVisible(_newButton = new TextButton("New", "New Profile"));
  _newButton->setName("New");
  _newButton->addListener(this);

  addAndMakeVisible(_dmxMap = new PropertyPanel("DMX Settings"));
  addAndMakeVisible(_params = new PropertyPanel("Parameters"));
  reloadParameters();

  addAndMakeVisible(_metadata = new PropertyPanel("Metadata"));
  reloadMetadata();
}

ProfileEditorComponent::~ProfileEditorComponent()
{
  delete _device;
  _browse = nullptr;
  _addMetadata = nullptr;
  _addParam = nullptr;
  _saveAsButton = nullptr;
  _saveButton = nullptr;
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
    text = "Profile: " + _profile.getFileName();
  }
  auto topRec = area.removeFromTop(30);
  topRec.removeFromRight(300);
  g.drawFittedText(text, topRec, Justification::left, 1);

  g.setFont(16);
  g.drawFittedText("Parameters", area.removeFromTop(30), Justification::centredLeft, 1);

  area.removeFromBottom(200);
  auto bottom = area.removeFromBottom(30);
  g.drawFittedText("DMX Settings", bottom.removeFromRight(275), Justification::centredLeft, 1);
  g.drawFittedText("Metadata", bottom, Justification::centredLeft, 1);
}

void ProfileEditorComponent::resized()
{
  auto area = getLocalBounds();

  // Filename and browse button on top.
  area.reduce(3, 3);
  auto top = area.removeFromTop(30);
  _browse->setBounds(top.removeFromRight(75).reduced(3));
  _saveAsButton->setBounds(top.removeFromRight(75).reduced(3));
  _saveButton->setBounds(top.removeFromRight(75).reduced(3));
  _newButton->setBounds(top.removeFromRight(75).reduced(3));

  _addParam->setBounds(area.removeFromTop(30).removeFromBottom(20).removeFromRight(100));

  auto bottom = area.removeFromBottom(200);
  _dmxMap->setBounds(bottom.removeFromRight(275));
  _metadata->setBounds(bottom);

  area.removeFromBottom(5);
  auto metadataRegion = area.removeFromBottom(20);
  metadataRegion.removeFromRight(280);
  _addMetadata->setBounds(metadataRegion.removeFromRight(100));

  _params->setBounds(area);
}

void ProfileEditorComponent::buttonClicked(Button* b) {
  if (b->getName() == "AddParam") {
    addParam();
  }
  if (b->getName() == "AddMetadata") {
    addMetadata();
  }
  if (b->getName() == "SaveAs") {
    saveAs();
  }
  if (b->getName() == "Save") {
    save();
  }
  if (b->getName() == "Browse") {
    open();
  }
  if (b->getName() == "New") {
    delete _device;
    _device = new Device("template", 1, "[New Device Type]");
    _dmxMapData.clear();
    _profile = "";
    reloadParameters();
    reloadMetadata();
    repaint();
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

  reloadDMXMap();
}

void ProfileEditorComponent::reloadMetadata() {
  _metadata->clear();

  _metadataProperties.clear();

  for (auto m : _device->getMetadataKeyNames()) {
    _metadataProperties.add(new LumiverseMetadataEditProperty(m, _device, [this]{ this->reloadMetadata(); }));
  }

  _metadata->addProperties(_metadataProperties);
}

void ProfileEditorComponent::reloadDMXMap() {
  _dmxMap->clear();

  for (auto p : _device->getParamNames())
  {
    Array<PropertyComponent*> props;

    if (_dmxMapData.count(p) == 0)
    {
      // Start a new entry
      patchData d(0, FLOAT_TO_SINGLE);

      string type = _device->getParam(p)->getTypeName();
      if (type == "enum") {
        d.type = ENUM;
      }
      else if (type == "color") {
        d.type = COLOR_RGB;
      }
      else if (type == "orientation") {
        d.type = ORI_TO_FINE;
      }

      _dmxMapData[p] = d;
    }

    props.add(new LumiverseDMXMapTypeProperty(&_dmxMapData, p, _device));
    props.add(new LumiverseDMXMapOffsetProperty(&_dmxMapData, p));

    _dmxMap->addSection(p, props, true);
  }

  // Check if there are things in the DMX map that should be deleted
  vector<string> toDelete;
  for (const auto& kvp : _dmxMapData)
  {
    if (_device->getParam(kvp.first) == nullptr) {
      toDelete.push_back(kvp.first);
    }
  }

  for (const auto& k : toDelete) {
    _dmxMapData.erase(k);
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

void ProfileEditorComponent::addMetadata() {
  juce::AlertWindow w("Add Metadata Field",
    "Add a Metadata Field to the Profile.",
    juce::AlertWindow::QuestionIcon);

  w.addTextEditor("name", "", "Name");

  w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    string name = w.getTextEditor("name")->getText().toStdString();

    if (_device->metadataExists(name)) {
      juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
        "Unable to add Metadata Field",
        "A field with the name \"" + name + "\" already exists.",
        "OK");
      return;
    }

    _device->setMetadata(name, "");
    reloadMetadata();
  }
}

void ProfileEditorComponent::reloadAll(string paramName) {
  auto components = _paramGUIs[paramName];

  for (auto c : components) {
    auto pc = (PropertyComponent*)c;
    pc->refresh();
  }
}

void ProfileEditorComponent::saveAs() {
  FileChooser fc("Save as...",
    File::getCurrentWorkingDirectory(),
    "*.profile.json",
    true);

  if (fc.browseForFileToSave(true))
  {
    _profile = fc.getResult();
    writeProfile();
  }
}

void ProfileEditorComponent::save() {
  if (!_profile.exists()) {
    saveAs();
    return;
  }

  writeProfile();
}

void ProfileEditorComponent::writeProfile() {
  // Update the device name based on profile file name
  String fileName = _profile.getFileName();
  fileName = fileName.upToFirstOccurrenceOf(".", false, false);

  _device->setType(fileName.toStdString());

  // Create JSON Node consisting of device and DMX data
  JSONNode profile;

  profile.push_back(JSONNode("info", "Lumiverse Device Profile"));
  profile.push_back(JSONNode("version", 1));
  profile.push_back(_device->toJSON());

  JSONNode dmx;
  dmx.set_name("dmxMap");

  for (auto d : _dmxMapData) {
    JSONNode mapping;
    mapping.set_name(d.first);
    mapping.push_back(JSONNode("start", d.second.startAddress));
#ifdef USE_C11_MAPS
    mapping.push_back(JSONNode("ctype", convTypeToString[d.second.type]));
#else
    mapping.push_back(JSONNode("ctype", convTypeToString(d.second.type)));
#endif
    dmx.push_back(mapping.as_array());
  }

  profile.push_back(dmx);

  ofstream prof;
  prof.open(_profile.getFullPathName().toStdString(), ios::out | ios::trunc);
  prof << profile.write_formatted();
  prof.close();

  repaint();
}

void ProfileEditorComponent::open() {
  // TODO: CHANGE HARDCODED PATH TO USER SPECIFIED
  FileChooser fc("Load Lumiverse Profile",
    File(MainWindow::getPropertiesFile()->getValue("profilePath")),
    "*.profile.json",
    true);

  if (fc.browseForFileToOpen())
  {
    String chosen;
    chosen << fc.getResults().getReference(0).getFullPathName();

    _profile = fc.getResult();

    // Check to see if we can load the file.
    ifstream data;
    data.open(_profile.getFullPathName().toStdString(), ios::in | ios::binary | ios::ate);

    if (data.is_open()) {
      // "+ 1" for the ending
      streamoff size = data.tellg();
      char* memblock = new char[(unsigned int)size + 1];

      data.seekg(0, ios::beg);

      data.read(memblock, size);
      data.close();

      // It's not guaranteed that the following memory after memblock is blank.
      // C-style string needs an end.
      memblock[size] = '\0';

      JSONNode n = libjson::parse(memblock);

      // Load
      // Profiles have two fields: template and dmxMap. Template is the device, dmxMap is the DMX map
      auto d = n.find("template");
      if (d == n.end()) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
          "Unable to load profile",
          "The profile does not have the required template device defined.",
          "OK");
        delete memblock;
        return;
      }

      // Make new device
      delete _device;
      _device = new Device("template", *d);

      // load DMX map
      auto i = n.find("dmxMap");
      if (i == n.end()) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
          "Unable to load profile",
          "The profile does not have the required DMX Map defined.",
          "OK");
        delete memblock;
        return;
      }

      _dmxMapData.clear();
      auto j = i->begin();
      while (j != i->end()) {
        string paramName = j->name();

        // This assumes the next piece of data is arranged in a [ int, string ] format
        unsigned int addr = (*j)[0].as_int();
        string conversion = (*j)[1].as_string();

        _dmxMapData[paramName] = patchData(addr, conversion);

        ++j;
      }

      delete memblock;
    }
  }

  reloadParameters();
  reloadMetadata();
  repaint();
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