#include "console.h"

Console::Console(void) {
	Serial.begin(115200);
	Serial.println();
	this->max_tag_length = TAG_MAX_LENGTH;
	this->tag_written = false;
}

void Console::setTagLength(uint8_t length) {
	this->max_tag_length = length;
}

void Console::line(style_t style, uint8_t length, bool new_line) {
	String line_str;
	String chr;

	if(this->tag_written == true)
		newLine();

	switch(style)
	{
		case DOTTED:		chr = "."; break;
		case DASHED:		chr = "-"; break;
		case DOUBLE_DASHED:	chr = "="; break;
		case LINE:			chr = "_"; break;
		case WAVE:			chr = "~"; break;
		case HASH:			chr = "#"; break;
		case ASTERISK:		chr = "*"; break;
		default:			chr = "."; break;
	}

	for(uint8_t i=0 ; i<length ; i++)
		line_str += chr;

	if(new_line)
		Serial.println(line_str);
	else
		Serial.print(line_str);
}

void Console::header(String title, style_t style, uint8_t length) {
	uint8_t fill = (length - title.length() - 6);
	uint8_t side = fill / 2;

	if(this->tag_written == true)
		newLine();	

	line(style, length, true);
	line(style, side, false);
	Serial.print("   " + title + "   ");
	if(length%2)	// Odd length
		line(style, (title.length()%2 ? side : side+1), true);
	else			// Even length
		line(style, (title.length()%2 ? side+1 : side), true);
	line(style, length, true);
}

void Console::writeTag(String tag) {
	uint8_t num_spaces = this->max_tag_length - tag.length();
	String pad_str;

	if(this->tag_written == true)
		newLine();

	for(uint8_t i=0 ; i<num_spaces ; i++)
		pad_str += " ";
	Serial.print("[ " + tag + pad_str + " ] ");
}

void Console::write(String tag, String message) {
	if(this->tag_written == false) {
		writeTag(tag);
		this->tag_written = true;
	}
	Serial.print(message);
}

void Console::log(String tag, String message) {
	write(tag, message);
	newLine();
}

// Terminal text in green -------------------------------------------
void Console::success(String tag, String message) {
	Serial.print(F("\033[1;32m"));
	write(tag, message);
	Serial.print(F("\033[0m"));
	newLine();
}

// Terminal text in blue --------------------------------------------
void Console::info(String tag, String message) {
	Serial.print(F("\033[1;34m"));
	write(tag, message);
	Serial.print(F("\033[0m"));
	newLine();
}

// Terminal text in yellow ------------------------------------------
void Console::warning(String tag, String message) {
	Serial.print(F("\033[1;33m"));
	write(tag, message);
	Serial.print(F("\033[0m"));
	newLine();
}

// Terminal text in red ---------------------------------------------
void Console::error(String tag, String message) {
	Serial.print(F("\033[1;31m"));
	write(tag, message);
	Serial.print(F("\033[0m"));
	newLine();
}

// Terminal text in magenta -----------------------------------------
void Console::command(String tag, String message) {
	Serial.print(F("\033[1;35m"));
	write(tag, message);
	Serial.print(F("\033[0m"));
	newLine();
}

// Terminal text in cyan --------------------------------------------
void Console::data(String tag, String message) {
	Serial.print(F("\033[1;36m"));
	write(tag, message);
	Serial.print(F("\033[0m"));
	newLine();
}

void Console::newLine(void) {
	this->tag_written = false;
	Serial.println();
}

// Terminal text in red ---------------------------------------------
void Console::heap(String message) {
	String heap = " ( Free = " + String(ESP.getFreeHeap());
	heap += " - Total = " + String(ESP.getHeapSize());
	heap += " - Min. = " + String(ESP.getMinFreeHeap()) + " )";
	write(HEAP_T, message + heap);
	newLine();
}

Console console;