#pragma once
#include <string>

struct AudioComponent {
	std::string assetId;

	AudioComponent(std::string assetId = "") {
		this->assetId = assetId;
	}
};