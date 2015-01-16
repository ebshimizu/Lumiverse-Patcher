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
    MainWindow::getRig()->getDevice(d->getId)->setParam(param, (float)newValue));
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
    MainWindow::getRig()->getDevice(d->getId)->setParam(param, (float)newValue);
  }
}

void LumiverseOrientationPropertySlider::sliderDragEnded(Slider* slider) {
}

LumiverseColorPropertySlider::LumiverseColorPropertySlider(string propertyName, string channelName,
  DeviceSet devices, double val, shared_ptr<Playback> pb, UpdateSource src, CueTable* ct)
  : SliderPropertyComponent(channelName, 0, 1, 0.001), m_pb(pb), m_src(src), m_ct(ct)
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
  if (m_src == CUE) {
    // Write into selected cue in the cue editor
    shared_ptr<CueList> list;
    Cue* cue;

    list = m_pb->getCueList(m_ct->getCurrentList());
    if (list == nullptr)
      return;

    cue = list->getCue(m_ct->getCueNum());
    if (cue == nullptr)
      return;

    for (auto d : m_devices.getDevices()) {
      if (cue->getCueData()[d->getId()].count(m_param) == 0)
        continue;

      auto deviceKeyframes = cue->getCueData()[d->getId()][m_param];
      for (const auto& k : deviceKeyframes) {
        if (k.t == m_ct->getCueTime()) {
          LumiverseColor* val = (LumiverseColor*)k.val.get();
          val->setColorChannel(m_channel, newVal);
          break;
        }
      }
    }

    m_ct->repaint();
  }
  m_pb->getProgrammer()->setParam(m_devices, m_param, m_channel, newVal);
}

void LumiverseColorPropertySlider::sliderDragEnded(Slider* slider) {
  MainWindow::getApplicationCommandManager().invokeDirectly(MainWindow::pushUndo, false);
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

  MainWindow::getApplicationCommandManager().invokeDirectly(MainWindow::pushUndo, false);
  MainWindow::getApplicationCommandManager().invokeDirectly(MainWindow::updateSmartGroups, true);
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

    MainWindow::getApplicationCommandManager().invokeDirectly(MainWindow::pushUndo, false);
		MainWindow::getApplicationCommandManager().invokeDirectly(MainWindow::updateSmartGroups, true);
	}
}
// ============================================================================
ControlsPanel::ControlsPanel(shared_ptr<Playback> pb) : m_pb(pb) {
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

void ControlsPanel::updateProperties(DeviceSet activeDevices, UpdateSource src, CueTable* ct) {
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
  auto& prog = m_pb->getProgrammer();

  // If we're manipulating a cue, we should gather the data that doesn't change here.
  shared_ptr<CueList> list;
  Cue* cue;

  if (src == CUE) {
    list = m_pb->getCueList(ct->getCurrentList());
    if (list == nullptr)
      return;

    cue = list->getCue(ct->getCueNum());
    if (cue == nullptr)
      return;
  }
  // if we're not in a cue, pull the right data into the programmer before running this.
  else if (src == TABLE) {
    prog->captureFromRig(activeDevices);
  }
  else if (src == PROG) {
    prog->captureDevices(activeDevices);
  }

  for (std::string param : params) {
    for (auto d : devices) {
      if (d->paramExists(param)) {
        // The update source matters mostly at this part, where we pick the data
        // we put into the programmer. If we're doing a cue, things get more complicated.
        LumiverseType* p = nullptr;

        if (src != CUE) {
          // Pull value from programmer
          p = prog->getDevice(d->getId())->getParam(param);
        }
        else {
          if (cue->getCueData()[d->getId()].count(param) == 0)
            continue;

          auto deviceKeyframes = cue->getCueData()[d->getId()][param];
          for (const auto& k : deviceKeyframes) {
            if (k.t == ct->getCueTime()) {
              p = k.val.get();
              break;
            }
          }

          if (p == nullptr) {
            continue;
          }
        }

        if (p->getTypeName() == "float") {
          LumiverseFloatPropertySlider* comp = new LumiverseFloatPropertySlider(param, activeDevices,
            (LumiverseFloat*) p, m_pb, src, ct);
          
          paramComponents.add(comp);
          break;
        }
        else if (p->getTypeName() == "enum") {
          LumiverseEnumPropertyComponent* comp = new LumiverseEnumPropertyComponent(param, activeDevices, (LumiverseEnum*) p, m_pb, src, ct);

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
            colorComponents.add(new LumiverseColorPropertySlider(param, kvp.first, activeDevices, kvp.second, m_pb, src, ct));
          }

          break;
        }
		if (p->getTypeName() == "orientation") {
			LumiverseOrientationPropertySlider* comp = new LumiverseOrientationPropertySlider(param, activeDevices, (LumiverseOrientation*) p, m_pb, src, ct);

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
  
  if (activeDevices.size() > 0)
	metadataComponents.add(new MetadataPropertyButton(activeDevices));

  if (paramComponents.size() > 0) properties.addSection("General", paramComponents);
  if (beamComponents.size() > 0) properties.addSection("Beam", beamComponents);
  if (colorComponents.size() > 0) properties.addSection("Color", colorComponents);
  if (controlComponents.size() > 0) properties.addSection("Control", controlComponents);
  if (metadataComponents.size() > 0) properties.addSection("Metadata", metadataComponents);
}