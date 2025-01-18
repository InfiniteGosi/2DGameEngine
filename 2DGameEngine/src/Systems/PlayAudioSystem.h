#pragma once
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/AudioComponent.h"

class PlayAudioSystem : public System {
public:
	PlayAudioSystem() {
		RequireComponent<AudioComponent>();
	}

	void Update(const std::unique_ptr<AssetStore>& assetStore) {
		for (auto& entity : GetSystemEntities()) {
			auto audio = entity.GetComponent<AudioComponent>();

			Mix_Music* music = assetStore->GetAudio(audio.assetId);

			if (Mix_PlayingMusic() == 0)
			{
				if (Mix_PlayMusic(music, -1) == -1)
				{
					return;
				}
			}
		}
	}
};