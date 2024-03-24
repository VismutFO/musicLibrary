#include <ctime>
#include <iostream>
#include <string>
#include <stdlib.h>

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

//------------------------------------------------------------------------
// a function that return random numbers in the given range
//------------------------------------------------------------------------
int getrandom(int range) {
	float f = (float)rand() / RAND_MAX;
	return (int)(f * range);
}

string getNote(int number) {
	int n = getrandom(7);
	string note;
	note += (char('A' + n));
	return note;
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
Sxmlelement makeAttributes() {
	Sxmlelement attributes = factory::instance().create(k_attributes);
	attributes->push(newElementI(k_divisions, kDivision));

	Sxmlelement time = factory::instance().create(k_time);
	time->push(newElement(k_beats, "4"));
	time->push(newElement(k_beat_type, "4"));
	attributes->push(time);

	Sxmlelement clef = factory::instance().create(k_clef);
	clef->push(newElement(k_sign, "G"));
	clef->push(newElement(k_line, "2"));
	attributes->push(clef);

	return attributes;
}

//------------------------------------------------------------------------
// creates a measure containing random notes
// the function takes the measure number as an argument
//------------------------------------------------------------------------
Sxmlelement makeMeasure(unsigned long num) {
	Sxmlelement measure = factory::instance().create(k_measure);
	measure->add(newAttributeI("number", num));
	if (num == 1) {					//  creates specific elements of the first measure
		measure->push(makeAttributes());		// division, time, clef...
	}
	for (int i = 0; i < 4; i++) {		// next adds 4 quarter notes
		Sxmlelement note = factory::instance().create(k_note);		// creates the note
		Sxmlelement pitch = factory::instance().create(k_pitch);	// creates a pitch
		pitch->push(newElement(k_step, getNote(1)));				// sets the pitch to a random value
		pitch->push(newElementI(k_octave, 4 + getrandom(2)));		// sets the octave to a random value
		note->push(pitch);											// adds the pitch to the note
		note->push(newElementI(k_duration, kDivision));				// sets the note duration to a quarter note
		note->push(newElement(k_type, "quarter"));					// creates the graphic elements of the note
		measure->push(note);		// and finally adds the note to the measure
	}
	return measure;
}

#define kPartID	"P1"
//------------------------------------------------------------------------
// creates a part containing 'count' measures
//------------------------------------------------------------------------
Sxmlelement makePart(int count) {
	Sxmlelement part = factory::instance().create(k_part);
	part->add(newAttribute("id", kPartID));
	for (int i = 1; i <= count; i++)			// and 'count' times
		part->push(makeMeasure(i));			// adds a new measure to the part
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

	Sxmlelement creator = newElement(k_creator, "Georg Chance");
	creator->add(newAttribute("type", "Composer"));
	id->push(creator);

	encoding->push(newElement(k_software, "MusicXML Library v2"));
	id->push(encoding);
	return id;
}

//------------------------------------------------------------------------
// the function that creates and writes the score
//------------------------------------------------------------------------
Sxmlelement randomMusic(int measuresCount) {
	Sxmlelement score = factory::instance().create(k_score_partwise);
	score->push(newElement(k_movement_title, "Random Music"));
	score->push(makeIdentification());
	score->push(makePartList());
	score->push(makePart(measuresCount));			// adds a part to the score
	return score;
}