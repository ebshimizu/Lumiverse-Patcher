#include "ControlsPanel.h"
#include "../Main.h"

LumiverseFloatPropertySlider::LumiverseFloatPropertySlider(string propertyName,
  DeviceSet devices, LumiverseFloat* val)
  : SliderPropertyComponent(propertyName, val->getMin(), val->getMax(), 0.001)
{
  param = propertyName;
  // We don't want to immediately capture this device, so we don't calll the full
  // setVal here
	this->val = val->getVal();
	slider.setValue(this->val);
  this->devices = devices;
}

void LumiverseFloatPropertySlider::setValue(double newValue)
{
  val = newValue;
  slider.setValue(newValue);

  for (const auto& d : devices.getDevices()) {
    MainWindow::getRig()->getDevice(d->getId())->setParam(param, (float)newValue);
  }
}

void LumiverseFloatPropertySlider::sliderDragEnded(Slider* slider) {
}

LumiverseOrientationPropertySlider::LumiverseOrientationPropertySlider(string propertyName, 
	DeviceSet devices, LumiverseOrientation* val) 
  : SliderPropertyComponent(propertyName, val->getMin(), val->getMax(), 0.01)
{
	param = propertyName;
  this->val = val->getVal();
  slider.setValue(this->val);
	this->devices = devices;
}

void LumiverseOrientationPropertySlider::setValue(double newValue)
{
	val = newValue;
	slider.setValue(newValue);

  for (const auto& d : devices.getDevices()) {
    MainWindow::getRig()->getDevice(d->getId())->setParam(param, (float)newValue);
  }
}

void LumiverseOrientationPropertySlider::sliderDragEnded(Slider* slider) {
}

LumiverseColorPropertySlider::LumiverseColorPropertySlider(string propertyName, string channelName,
  DeviceSet devices, double val)
  : SliderPropertyComponent(channelName, 0, 1, 0.001)
{
  m_param = propertyName;
  m_channel = channelName;
  m_val = val;
  slider.setValue(m_val);
  m_devices = devices;
}

void LumiverseColorPropertySlider::setValue(double newVal) {
  m_val = newVal;

  slider.setValue(newVal);
  for (const auto& d : m_devices.getDevices()) {
    MainWindow::getRig()->getDevice(d->getId())->setParam(m_param, m_channel, newVal);
  }
}

void LumiverseColorPropertySlider::sliderDragEnded(Slider* slider) {
}

MetadataPropertyTextEditor::MetadataPropertyTextEditor(string propertyName, DeviceSet devices) 
	: m_devices(devices), m_metadata(propertyName), 
	TextPropertyComponent(propertyName, 128, false) {
	m_val = combineMetadataWithSameKey(devices.getAllMetadataForKey(m_metadata));
}

string MetadataPropertyTextEditor::combineMetadataWithSameKey(set<string> metadata) {
	if (metadata.size() == 1)
		return *(metadata.begin());
	else if (metadata.size() > 1)
		return "[Multiple Values]";
	return "[Error: non-exist]";
}

void MetadataPropertyTextEditor::setText(const String &newText) {
	if (newText.isEmpty()) {
		bool toDelete = false;
		{
			AlertWindow w("Delete Metadata",
				"Are you sure to delete this metadata field?",
				AlertWindow::QuestionIcon);

			w.addButton("Yes", 1, KeyPress(KeyPress::returnKey, 0, 0));
			w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

			toDelete = (w.runModalLoop() != 0);
		}

		if (toDelete) {
			for (Device *dev : m_devices.getDevices()) {
				dev->deleteMetadata(m_metadata);
			}
		}
	}
	else {
		for (Device *dev : m_devices.getDevices()) {
			dev->setMetadata(m_metadata, newText.toStdString());
		}
		m_val = newText.toStdString();
	}
}

void MetadataPropertyButton::buttonClicked() {
	bool hasInput = false;
	String name;
	String val;

	// The popped up window only exists in this block. So when
	// we go back to invoke update, it would be passed to the
	// right target (MainWindow).
	{
		AlertWindow w("New Metadata",
			"Enter a name and a value for the new metadata.",
			AlertWindow::QuestionIcon);

		w.addTextEditor("name", "Name", "List Name:");
		w.addTextEditor("val", "Value", "List Value:");

		w.addButton("Create", 1, KeyPress(KeyPress::returnKey, 0, 0));
		w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

		if (w.runModalLoop() != 0) // is they picked 'ok'
		{
			// this is the text they entered..
			name = w.getTextEditorContents("name");
			val = w.getTextEditorContents("val");
			hasInput = true;
		}
	}
	
	if (hasInput) {
		for (Device *dev : m_devices.getDevices()) {
			dev->setMetadata(name.toStdString(), val.toStdString());
		}
	}
}

//==============================================================================
DMXPatchAddrProperty::DMXPatchAddrProperty(string patchID, string deviceID) :
TextPropertyComponent(patchID + ": Address", 10, false), _patchID(patchID), _deviceID(deviceID)
{}

void DMXPatchAddrProperty::setText(const String& newText) {
  if (newText == "") {
    MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->deleteDevice(_deviceID);
  }
  else {
    auto info = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getDevicePatch(_deviceID);
    if (info != nullptr) {
      info->setBaseAddress(newText.getIntValue() - 1);
    }
    else {
      string dmxMap = MainWindow::getRig()->getDevice(_deviceID)->getType();
      MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->patchDevice(_deviceID, new DMXDevicePatch(dmxMap, newText.getIntValue() - 1, 0));
    }
  }
  MainWindow::getApplicationCommandManager().invokeDirectly(MainWindow::refresh, false);
}

String DMXPatchAddrProperty::getText() const {
  auto info = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getDevicePatch(_deviceID);
  if (info != nullptr) {
    return String(info->getBaseAddress() + 1);
  }
  else {
    return "Not Patched";
  }
}

//==============================================================================
DMXPatchUniverseProperty::DMXPatchUniverseProperty(string patchID, string deviceID) :
TextPropertyComponent(patchID + ": Universe", 10, false), _patchID(patchID), _deviceID(deviceID)
{}

void DMXPatchUniverseProperty::setText(const String& newText) {
  if (newText == "") {
    MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->deleteDevice(_deviceID);
  }
  else {
    auto info = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getDevicePatch(_deviceID);
    if (info != nullptr) {
      info->setUniverse(newText.getIntValue() - 1);
    }
    else {
      string dmxMap = MainWindow::getRig()->getDevice(_deviceID)->getType();
      MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->patchDevice(_deviceID, new DMXDevicePatch(dmxMap, 0, newText.getIntValue() - 1));
    }
  }
  MainWindow::getApplicationCommandManager().invokeDirectly(MainWindow::refresh, false);
}

String DMXPatchUniverseProperty::getText() const {
  auto info = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getDevicePatch(_deviceID);
  if (info != nullptr) {
    return String(info->getUniverse() + 1);
  }
  else {
    return "Not Patched";
  }
}

// ============================================================================
ControlsPanel::ControlsPanel() {
  properties.setName("Device Properties");
  properties.setMessageWhenEmpty("No Devices Selected");

  addAndMakeVisible(properties);
}

ControlsPanel::~ControlsPanel() {

}

void ControlsPanel::paint(Graphics& g) {
  g.fillAll(Colour(0xff1c1c1c));
}

void ControlsPanel::resized() {
  properties.setBounds(getLocalBounds());
}

void ControlsPanel::updateProperties(DeviceSet activeDevices) {
  // Process for this is to look at what components needed to be added, add and sort them
  // and then display them
  properties.clear();

  // Start with active parameters
  // Multiple selection items will pull initial values from the first element found for now
  // Later, may add options for relative value adjustments

  // This will almost certainly need to be broken up and organized at some point.
  // Should probably have some maps that translate param to cleaner name + category.
  Array<PropertyComponent*> paramComponents;
  Array<PropertyComponent*> colorComponents;
  Array<PropertyComponent*> beamComponents;
  Array<PropertyComponent*> controlComponents;
  Array<PropertyComponent*> metadataComponents;

  set<std::string> metadata = activeDevices.getAllMetadata();
  set<std::string> params = activeDevices.getAllParams();
  const set<Device*>& devices = activeDevices.getDevices();

  for (std::string param : params) {
    for (auto d : devices) {
      if (d->paramExists(param)) {
        // The update source matters mostly at this part, where we pick the data
        // we put into the programmer. If we're doing a cue, things get more complicated.
        LumiverseType* p = nullptr;

        // Pull value from rig
        p = MainWindow::getRig()->getDevice(d->getId())->getParam(param);

        if (p->getTypeName() == "float") {
          LumiverseFloatPropertySlider* comp = new LumiverseFloatPropertySlider(param, activeDevices,
            (LumiverseFloat*) p);
          
          paramComponents.add(comp);
          break;
        }
        else if (p->getTypeName() == "enum") {
          LumiverseEnumPropertyComponent* comp = new LumiverseEnumPropertyComponent(param, activeDevices, (LumiverseEnum*) p);

          if (param == "rainbow" || param == "colorWheel")
            colorComponents.add(comp);
          else if (param == "control" || param == "moveSpeed")
            controlComponents.add(comp);
          else if (param == "shutter" || param == "gobo" || param == "goboRot" || param == "prism")
            beamComponents.add(comp);
          else 
            paramComponents.add(comp);

          break;
        }
        else if (p->getTypeName() == "color") {
          LumiverseColor* c = (LumiverseColor*)p;
          
          for (const auto& kvp : c->getColorParams()) {
            colorComponents.add(new LumiverseColorPropertySlider(param, kvp.first, activeDevices, kvp.second));
          }

          break;
        }
        else if (p->getTypeName() == "orientation") {
          LumiverseOrientationPropertySlider* comp = new LumiverseOrientationPropertySlider(param, activeDevices, (LumiverseOrientation*) p);

          paramComponents.add(comp);
          break;
        }
        else {
          // Don't do anything for unknown types. Can't interact with what you don't understand.
        }
      }
    }
  }

  // Next add metadata. Values with multiples will be marked.
  for (std::string meta : metadata) {
	  MetadataPropertyTextEditor* comp = new MetadataPropertyTextEditor(meta, activeDevices);
	  metadataComponents.add(comp);
  }
  
  if (activeDevices.size() > 0) metadataComponents.add(new MetadataPropertyButton(activeDevices));

  if (paramComponents.size() > 0) properties.addSection("General", paramComponents);
  if (beamComponents.size() > 0) properties.addSection("Beam", beamComponents);
  if (colorComponents.size() > 0) properties.addSection("Color", colorComponents);
  if (controlComponents.size() > 0) properties.addSection("Control", controlComponents);
  if (metadataComponents.size() > 0) properties.addSection("Metadata", metadataComponents);

  // Finally patch data if there's a single device selected.
  if (devices.size() == 1) {
    string deviceID = "";
    for (auto d : devices) {
      deviceID = d->getId();
      break;
    }

    Array<PropertyComponent*> patchComponents;
    for (const auto& kvp : MainWindow::getRig()->getPatches()) {
      if (kvp.second->getType() == "DMXPatch") {
        // Add DMX properties
        patchComponents.add(new DMXPatchUniverseProperty(kvp.first, deviceID));
        patchComponents.add(new DMXPatchAddrProperty(kvp.first, deviceID));
      }
    }

    properties.addSection("Patch", patchComponents);
  }
}