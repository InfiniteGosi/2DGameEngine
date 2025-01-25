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

			Mix_Chunk* music = assetStore->GetAudio(audio.assetId);

			if (!Mix_Playing(audio.channel)) {
				// Play the audio, using a free channel if needed
				if (Mix_PlayChannel(audio.channel, music, 1) == -1) {
					return;
				}
			}
		}
	}
};