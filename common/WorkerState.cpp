enum WorkerState {
	initializing, ready, working
};

string workerStateToString(WorkerState state) {
	if (state == ready) {
		return "Ready";
	} else if (state == working) {
		return "Working";
	} else if (state == initializing) {
		return "Initializing";
	}
	return "";
}

WorkerState workerStateFromString(string& state) {
	if (state == "Ready") {
		return ready;
	} else if (state == "Working") {
		return working;
	} else if (state == "Initializing") {
		return initializing;
	}
	throw "Invalid worker state";
}