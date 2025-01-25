#pragma once
#include <string>

struct AudioComponent {
	std::string assetId;
	int channel;

	AudioComponent(std::string assetId = "", int channel = -1) {
		this->assetId = assetId;
		this->channel = channel;
	}
};