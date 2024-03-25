#pragma once
#include <string>
#include "musicxml/elements.h"
#include "musicxml/factory.h"
#include "musicxml/xml.h"
#include "musicxml/xmlfile.h"

std::string getStep(int number);

int getAlter(int number);

//------------------------------------------------------------------------
MusicXML2::Sxmlattribute newAttribute(const std::string& name, const std::string& value);

//------------------------------------------------------------------------
MusicXML2::Sxmlattribute newAttributeI(const std::string& name, int value);

//------------------------------------------------------------------------
MusicXML2::Sxmlelement newElement(int type, const std::string& value);

//------------------------------------------------------------------------
MusicXML2::Sxmlelement newElementI(int type, int value);

//------------------------------------------------------------------------
MusicXML2::Sxmlelement makeAttributes(size_t kBeats, size_t kBeatType);

//------------------------------------------------------------------------
// creates a measure containing random notes
// the function takes the measure number as an argument
//------------------------------------------------------------------------
MusicXML2::Sxmlelement makeMeasure(size_t num, size_t kBeats, size_t kBeatType, const std::vector<std::pair<int, int>>& notes);

//------------------------------------------------------------------------
// creates a part containing 'count' measures
//------------------------------------------------------------------------
MusicXML2::Sxmlelement makePart(size_t kBeats, size_t kBeatType, const std::vector<std::vector<std::pair<int, int>>>& allMeasures);

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
MusicXML2::Sxmlelement getMusicByMeasures(size_t kBeats, size_t kBeatType, const std::vector<std::vector<std::pair<int, int>>>& allMeasures);

uint8_t* getBinaryContentByMusic(MusicXML2::Sxmlelement music);

void printMusic(MusicXML2::Sxmlelement score);
