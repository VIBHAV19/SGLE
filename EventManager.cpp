#include "EventManager.h"
#include "stdafx.h"

void EventManager::fireEvent(Event e, EventArgs &eventArgs) {
	std::map<Event, Callback>::iterator it;

	it = events.find(e);

	if (it != events.end())
		it->second(eventArgs);
}

void EventManager::addEventListener(Event event, Callback callback) {
	events.insert_or_assign(event, callback);
}

void EventManager::removeEventListener(Event event) {
	events.erase(event);
}

void EventManager::removeAllEvents() {
	events.clear();
}
