#pragma once
#include "Component.h"

struct Console : public Component {
	char inputBuf[256];
	std::vector<std::pair<std::string, olc::Pixel>> buffer; //text, color
	std::vector<std::string> history;
	int historyPos = -1;

	bool autoScroll = true;
	bool scrollToBottom = false;

	//Console() {
	//	memset(inputBuf, 0, sizeof(inputBuf));
	//	historyPos = -1;
	//	autoScroll = true;
	//	scrollToBottom = false;
	//}

};