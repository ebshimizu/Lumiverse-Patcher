#ifndef ARNOLDANIMATIONPATCHVIEWER_H
#define ARNOLDANIMATIONPATCHVIEWER_H

#pragma once

#include "LumiverseCore.h"

using namespace Lumiverse;

class AAPTextProperty : public TextPropertyComponent
{
public:
  AAPTextProperty(string prop, string patchID);

  void setText(const String& newText);
  String getText() const;

private:
  string _property;
  string _patchID;
};

class ArnoldAnimationPatchViewer : public Component
{
public:
  ArnoldAnimationPatchViewer(string patchID);
  ~ArnoldAnimationPatchViewer();
  
  void paint(Graphics& g) override;
  void resized() override;

private:
  PropertyPanel _properties;
};

#endif