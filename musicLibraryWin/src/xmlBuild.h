#pragma once
#include <string>
#include "musicxml/elements.h"
#include "musicxml/factory.h"
#include "musicxml/xml.h"
#include "musicxml/xmlfile.h"


int getrandom(int range);

std::string getNote(int number);

//------------------------------------------------------------------------
MusicXML2::Sxmlattribute newAttribute(const std::string& name, const std::string& value);

//------------------------------------------------------------------------
MusicXML2::Sxmlattribute newAttributeI(const std::string& name, int value);

//------------------------------------------------------------------------
MusicXML2::Sxmlelement newElement(int type, const std::string& value);

//------------------------------------------------------------------------
MusicXML2::Sxmlelement newElementI(int type, int value);

//------------------------------------------------------------------------
MusicXML2::Sxmlelement makeAttributes();

//------------------------------------------------------------------------
// creates a measure containing random notes
// the function takes the measure number as an argument
//------------------------------------------------------------------------
MusicXML2::Sxmlelement makeMeasure(unsigned long num);

//------------------------------------------------------------------------
// creates a part containing 'count' measures
//------------------------------------------------------------------------
MusicXML2::Sxmlelement makePart(int count);

//------------------------------------------------------------------------
// creates the part list element
//------------------------------------------------------------------------
MusicXML2::Sxmlelement makePartList();
//------------------------------------------------------------------------
// creates the identification element
//------------------------------------------------------------------------
MusicXML2::Sxmlelement makeIdentification();

//------------------------------------------------------------------------
// the function that creates and writes the score
//------------------------------------------------------------------------
MusicXML2::Sxmlelement randomMusic(int measuresCount);