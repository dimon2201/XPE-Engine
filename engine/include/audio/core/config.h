#pragma once

const float MusicVolume = 1.0f;
const float SoundVolume = 1.0f;
const float VoiceVolume = 1.0f;

//const unsigned int NUM_BUFFERS = 4;
//const unsigned int BUFFER_SAMPLES = 8192;

//audio recorder
const unsigned int SAMPLE_RATE = 22050;
const unsigned int CHANNELS = 1;
const float DURATION = 0.02;

const unsigned int DATA_SIZE = static_cast<unsigned int>(DURATION * SAMPLE_RATE * CHANNELS * 2);