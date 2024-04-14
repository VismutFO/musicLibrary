#include <ctime>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <fstream>

#ifdef WIN32
#include <windows.h>
#endif

#include "musicxml/elements.h"
#include "musicxml/factory.h"
#include "musicxml/xml.h"
#include "musicxml/xmlfile.h"

#include <random>

using namespace MusicXML2;
using namespace std;

#define kDivision	4

string getStep(int number) {
	string notes[] = {"C", "C", "D", "D", "E", "F", "F", "G", "G", "A", "A", "B"};
	return notes[number];
}

int getAlter(int number) {
	int notes[] = { 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };
	return notes[number];
}

//------------------------------------------------------------------------
Sxmlattribute newAttribute(const string& name, const string& value)
{
	Sxmlattribute attribute = xmlattribute::create();
	attribute->setName(name);
	attribute->setValue(value);
	return attribute;
}

//------------------------------------------------------------------------
Sxmlattribute newAttributeI(const string& name, int value)
{
	Sxmlattribute attribute = xmlattribute::create();
	attribute->setName(name);
	attribute->setValue(value);
	return attribute;
}

//------------------------------------------------------------------------
Sxmlelement newElement(int type, const string& value)
{
	Sxmlelement elt = factory::instance().create(type);
	// elt->setValue(value);
	elt->setValue(value);
	return elt;
}

//------------------------------------------------------------------------
Sxmlelement newElementI(int type, int value)
{
	Sxmlelement elt = factory::instance().create(type);
	elt->setValue(value);
	return elt;
}

//------------------------------------------------------------------------
Sxmlelement makeAttributes(size_t kBeats, size_t kBeatType) {
	Sxmlelement attributes = factory::instance().create(k_attributes);
	attributes->push(newElementI(k_divisions, kDivision));

	Sxmlelement time = factory::instance().create(k_time);
	time->push(newElement(k_beats, std::to_string(kBeats)));
	time->push(newElement(k_beat_type, std::to_string(kBeatType)));
	attributes->push(time);

	Sxmlelement clef = factory::instance().create(k_clef); // всегда скрипичный ключ
	clef->push(newElement(k_sign, "G"));
	clef->push(newElement(k_line, "2"));
	attributes->push(clef);

	return attributes;
}

Sxmlelement createPitch(int midi_num) {
	Sxmlelement pitch = factory::instance().create(k_pitch);
	std::string step = getStep(midi_num % 12);
	pitch->push(newElement(k_step, step));
	int alter = getAlter(midi_num % 12);
	if (alter) {
		pitch->push(newElementI(k_alter, alter));
	}
	pitch->push(newElementI(k_octave, (midi_num / 12) - 1));
	return pitch;
}

Sxmlelement createNote(Sxmlelement pitch, int duration) { // duration is denominator (for example, need half - duration = 2)
	Sxmlelement note = factory::instance().create(k_note);		// creates the note
	note->push(pitch);											// adds the pitch to the note
	note->push(newElementI(k_duration, duration));				// sets the note duration to a quarter note
	switch (duration) {
	case 1:
		note->push(newElement(k_type, "whole"));
		break;
	case 2:
		note->push(newElement(k_type, "half"));
		break;
	case 4:
		note->push(newElement(k_type, "quarter"));
		break;
	case 8:
		note->push(newElement(k_type, "eigth"));
		break;
	case 16:
		note->push(newElement(k_type, "16th"));
		break;
	}
	return note;
}

Sxmlelement createRest(int duration) {
	Sxmlelement rest = factory::instance().create(k_rest);
	return rest;
}


//------------------------------------------------------------------------
// creates a measure containing random notes
// the function takes the measure number as an argument
//------------------------------------------------------------------------
Sxmlelement makeMeasure(size_t num, size_t kBeats, size_t kBeatType, const std::vector<std::pair<int, int>>& notes) { // такт, передаётся номер
	Sxmlelement measure = factory::instance().create(k_measure);
	measure->add(newAttributeI("number", num));
	if (num == 1) {					//  creates specific elements of the first measure
		measure->push(makeAttributes(kBeats, kBeatType));		// division, time, clef...
	}
	for (size_t i = 0; i < notes.size(); i++) {
		if (notes[i].first == -1) {
			measure->push(createRest(notes[i].second));
		}
		else {
			measure->push(createNote(createPitch(notes[i].first), notes[i].second));		// and finally adds the note to the measure
		}
	}
	return measure;
}

#define kPartID	"P1"
//------------------------------------------------------------------------
// creates a part containing 'count' measures
//------------------------------------------------------------------------

Sxmlelement makePart(size_t kBeats, size_t kBeatType, const std::vector<std::vector<std::pair<int, int>>>& allMeasures) {
	Sxmlelement part = factory::instance().create(k_part);
	part->add(newAttribute("id", kPartID));
	for (size_t i = 0; i < allMeasures.size(); i++) {
		part->push(makeMeasure(i + 1, kBeats, kBeatType, allMeasures[i]));
	}
	return part;
}

//------------------------------------------------------------------------
// creates the part list element
//------------------------------------------------------------------------
Sxmlelement makePartList() {
	Sxmlelement partlist = factory::instance().create(k_part_list);
	Sxmlelement scorepart = factory::instance().create(k_score_part);
	scorepart->add(newAttribute("id", kPartID));
	scorepart->push(newElement(k_part_name, "Part name"));
	Sxmlelement scoreinstr = factory::instance().create(k_score_instrument);
	scoreinstr->add(newAttribute("id", "I1"));
	scoreinstr->push(newElement(k_instrument_name, "Any instr."));
	scorepart->push(scoreinstr);
	partlist->push(scorepart);
	return partlist;
}

//------------------------------------------------------------------------
// creates the identification element
//------------------------------------------------------------------------
Sxmlelement makeIdentification() {
	Sxmlelement id = factory::instance().create(k_identification);
	Sxmlelement encoding = factory::instance().create(k_encoding);

	Sxmlelement creator = newElement(k_creator, "Artemiy Stepanov");
	creator->add(newAttribute("type", "Composer"));
	id->push(creator);

	encoding->push(newElement(k_software, "MusicXML Library v2"));
	id->push(encoding);
	return id;
}

//------------------------------------------------------------------------
// the function that creates and writes the score
//------------------------------------------------------------------------
Sxmlelement getMusicByMeasures(size_t kBeats, size_t kBeatType, const std::vector<std::vector<std::pair<int, int>>>& allMeasures) {
	Sxmlelement score = factory::instance().create(k_score_partwise);
	score->push(newElement(k_movement_title, "Random Music"));
	score->push(makeIdentification());
	score->push(makePartList());
	score->push(makePart(kBeats, kBeatType, allMeasures));
	return score;
}

uint8_t* getBinaryContentByMusic(Sxmlelement music) {
	return nullptr;
}

void printMusic(MusicXML2::Sxmlelement score) {
	SXMLFile f = TXMLFile::create();
	f->set(new TXMLDecl("1.0", "", TXMLDecl::kNo));
	f->set(new TDocType("score-partwise"));
	f->set(score);
	f->print(cout);
	cout << endl;
}

void saveMusicToFile(MusicXML2::Sxmlelement score, const char* resultName) {
	SXMLFile f = TXMLFile::create();
	f->set(new TXMLDecl("1.0", "", TXMLDecl::kNo));
	f->set(new TDocType("score-partwise"));
	f->set(score);
	ofstream output(resultName);
	f->print(output);
}
