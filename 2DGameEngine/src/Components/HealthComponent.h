#pragma once

struct HealthComponent {
	int healPercentage;
	HealthComponent(int healthComponent = 0) {
		this->healPercentage = healthComponent;
	}
};