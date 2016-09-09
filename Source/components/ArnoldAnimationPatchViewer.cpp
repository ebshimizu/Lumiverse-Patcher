/*
  ==============================================================================

    ArnoldAnimationPatchViewer.cpp
    Created: 9 Sep 2016 3:37:20pm
    Author:  falindrith

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "ArnoldAnimationPatchViewer.h"
#include "../Main.h"

AAPTextProperty::AAPTextProperty(string prop, string patchID) :
  TextPropertyComponent(prop, 1000, false), _property(prop), _patchID(patchID)
{
}

void AAPTextProperty::setText(const String & newText)
{
  ArnoldAnimationPatch* p = dynamic_cast<ArnoldAnimationPatch*>(MainWindow::getRig()->getPatch(_patchID));

  if (_property == "sceneFile") {
    p->setAssFile(newText.toStdString());
  }
  else if (_property == "pluginDir") {
    p->getArnoldInterface()->setPluginDirectory(newText.toStdString());
  }
  else if (_property == "gamma") {
    p->getArnoldInterface()->setGamma(newText.getFloatValue());
  }
  else if (_property == "width") {
    int height = dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->getHeight();
    dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->setDims(newText.getIntValue(), height);
  }
  else if (_property == "height") {
    int width = dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->getWidth();
    dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->setDims(width, newText.getIntValue());
  }
  else if (_property == "samples") {
    dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->setSamples(newText.getIntValue());
  }
  else if (_property == "path") {
    dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->setPath(newText.toStdString());
  }
}

String AAPTextProperty::getText() const
{
  ArnoldAnimationPatch* p = dynamic_cast<ArnoldAnimationPatch*>(MainWindow::getRig()->getPatch(_patchID));

  if (_property == "sceneFile") {
    return p->getArnoldInterface()->getAssFile();
  }
  else if (_property == "pluginDir") {
    return p->getArnoldInterface()->getPluginDirectory();
  }
  else if (_property == "gamma") {
    return String(p->getArnoldInterface()->getGamma());
  }
  else if (_property == "width") {
    return String(dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->getWidth());
  }
  else if (_property == "height") {
    return String(dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->getHeight());
  }
  else if (_property == "samples") {
    return String(dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->getSamples());
  }
  else if (_property == "path") {
    return dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface())->getPath();
  }
}

//==============================================================================
ArnoldAnimationPatchViewer::ArnoldAnimationPatchViewer(string patchID)
{
  Array<PropertyComponent*> mainProps;
  mainProps.add(new AAPTextProperty("sceneFile", patchID));
  mainProps.add(new AAPTextProperty("pluginDir", patchID));
  mainProps.add(new AAPTextProperty("gamma", patchID));

  ArnoldAnimationPatch* p = dynamic_cast<ArnoldAnimationPatch*>(MainWindow::getRig()->getPatch(patchID));
  if (dynamic_cast<CachingArnoldInterface*>(p->getArnoldInterface()) != nullptr) {
    Array<PropertyComponent*> cacheProps;
    cacheProps.add(new AAPTextProperty("width", patchID));
    cacheProps.add(new AAPTextProperty("height", patchID));
    cacheProps.add(new AAPTextProperty("samples", patchID));
    cacheProps.add(new AAPTextProperty("path", patchID));
    
    _properties.addSection("Cache Properties", cacheProps);
  }

  _properties.addSection("Arnold Animation Patch Settings", mainProps);
  addAndMakeVisible(_properties);
}

ArnoldAnimationPatchViewer::~ArnoldAnimationPatchViewer()
{
}

void ArnoldAnimationPatchViewer::paint (Graphics& g)
{
  g.setColour (Colours::grey);
  g.fillAll();
}

void ArnoldAnimationPatchViewer::resized()
{
  _properties.setBounds(getLocalBounds());
}