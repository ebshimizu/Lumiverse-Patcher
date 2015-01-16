#include "LumiverseEnumPropertyComponent.h"
#include "../Main.h"

LumiverseEnumPropertyComponent::LumiverseEnumPropertyComponent(string name, DeviceSet devices,
  LumiverseEnum* data)
  : PropertyComponent(name), m_tweak(data->getTweak()), m_val(data->getValIndex() + 1),
  m_mode(data->getMode()), m_interpMode(data->getInterpMode())
{
  m_devices = devices;

  addAndMakeVisible(m_valBox);
  addAndMakeVisible(m_slider);
  addAndMakeVisible(m_modeBox);
  addAndMakeVisible(m_interpModeBox);

  // Tweak bar settings
  m_slider.setName("tweak");
  m_slider.setRange(0, 1, 0.001);
  m_slider.setSliderStyle(Slider::LinearBar);
  m_slider.addListener(this);

  // Value box settings
  m_valBox.setName("val");
  m_valBox.setEditableText(false);
  m_vals = data->getStartToVals();
  for (const auto& kvp : m_vals) {
    m_valBox.addItem(kvp.second, kvp.first + 1);
  }

  // Mode box settings
  m_modeBox.setName("mode");
  m_modeBox.setEditableText(false);
  m_modeBox.addItem("First", Lumiverse::LumiverseEnum::FIRST + 1);
  m_modeBox.addItem("Center", Lumiverse::LumiverseEnum::CENTER + 1);
  m_modeBox.addItem("Last", Lumiverse::LumiverseEnum::LAST + 1);

  // InterpMode box settings
  m_interpModeBox.setName("interpMode");
  m_interpModeBox.setEditableText(false);
  m_interpModeBox.addItem("Smooth", Lumiverse::LumiverseEnum::InterpolationMode::SMOOTH + 1);
  m_interpModeBox.addItem("Smooth Within Option", Lumiverse::LumiverseEnum::InterpolationMode::SMOOTH_WITHIN_OPTION + 1);
  m_interpModeBox.addItem("Snap", Lumiverse::LumiverseEnum::InterpolationMode::SNAP + 1);

  m_valBox.addListener(this);
  m_modeBox.addListener(this);
  m_interpModeBox.addListener(this);

  preferredHeight = 85;
}

LumiverseEnumPropertyComponent::~LumiverseEnumPropertyComponent()
{
}

void LumiverseEnumPropertyComponent::refresh() {
  m_slider.setValue(m_tweak, dontSendNotification);
  m_valBox.setSelectedId(m_val, dontSendNotification);
  m_modeBox.setSelectedId(m_mode + 1, dontSendNotification);
  m_interpModeBox.setSelectedId(m_interpMode + 1, dontSendNotification);
}

void LumiverseEnumPropertyComponent::sliderValueChanged(Slider* slider) {
  m_tweak = slider->getValue();
  updateData();
}

void LumiverseEnumPropertyComponent::sliderDragEnded(Slider* slider) {
}

void LumiverseEnumPropertyComponent::comboBoxChanged(ComboBox* box) {
  if (box->getName() == "val") {
    m_val = box->getSelectedId();
  }
  else if (box->getName() == "mode") {
    m_mode = (Lumiverse::LumiverseEnum::Mode)(box->getSelectedId() - 1);
  }
  else if (box->getName() == "interpMode") {
    m_interpMode = (Lumiverse::LumiverseEnum::InterpolationMode)(box->getSelectedId() - 1);
  }

  updateData();
}

void LumiverseEnumPropertyComponent::resized() {
  int width = getWidth();
  int height = getHeight();
  const int textW = jmin(200, getWidth() / 3);
  
  for (int i = 0; i < getNumChildComponents(); i++) {
    Component* const c = getChildComponent(i);
    if (i == 0) {
      c->setBounds(juce::Rectangle<int>(textW, 2 + (20 * i), width - textW - 1, 20));
    }
    else {
      c->setBounds(juce::Rectangle<int>(textW + 100, 2 + (20 * i), width - textW - 100, 20));
    }
  }
}

void LumiverseEnumPropertyComponent::paint(Graphics& g) {
  LookAndFeel& lf = getLookAndFeel();

  lf.drawPropertyComponentBackground(g, getWidth(), getHeight(), *this);
  lf.drawPropertyComponentLabel(g, getWidth(), getHeight(), *this);

  juce::Rectangle<int> r(lf.getPropertyComponentContentPosition(*this).removeFromLeft(100));

  g.drawFittedText("Tweak", r.getX(), r.getY() + 22, r.getWidth(), 20, Justification::centredLeft, 2);
  g.drawFittedText("Mode", r.getX(), r.getY() + 42, r.getWidth(), 20, Justification::centredLeft, 2);
  g.drawFittedText("Interp. Mode", r.getX(), r.getY() + 62, r.getWidth(), 20, Justification::centredLeft, 2);
}

void LumiverseEnumPropertyComponent::updateData() {
  for (const auto& d : m_devices.getDevices()) {
    MainWindow::getRig()->getDevice(d->getId())->setParam(getName().toStdString(),
      m_vals[m_val - 1], m_tweak, m_mode, m_interpMode);
  }
}