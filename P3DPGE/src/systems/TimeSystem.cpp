#include "TimeSystem.h"

#include "../components/Time.h"
#include "../utils/Command.h"

template<class... T>
inline void AddTimeCommands(EntityAdmin* admin) {
	admin->commands["time_pause_engine"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->time->paused = !admin->time->paused;
		if (admin->time->paused) {
			admin->time->deltaTime = 0.f;
		}
		return ""; //TODO(, sushi) format these
		}, "time_pause_engine", "time_pause_engine");

	admin->commands["time_next_frame"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		if(admin->time->paused) {
			admin->time->frame = true;
		}
		return "";
	}, "time_next_frame", "time_next_frame");
}

void TimeSystem::Init() {
	AddTimeCommands(admin);
}

void TimeSystem::Update() {
	Time* time = admin->time;

	//sets date and time string
	time->end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	ctime_s(time->datentime, sizeof(time->datentime), &time->end_time);

	if(!time->paused) {
		time->deltaTime = admin->p->GetElapsedTime();
		time->totalTime += time->deltaTime;
		++time->updateCount;

		time->physicsAccumulator += time->deltaTime;
	} else if(time->frame) {
		time->deltaTime = admin->p->GetElapsedTime();
		time->totalTime += time->deltaTime;
		++time->updateCount;

		time->physicsAccumulator += time->deltaTime;

		time->frame = false;
	} else {
		time->deltaTime = 0.f;
	}
}