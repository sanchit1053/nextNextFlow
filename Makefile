orchestrator: orchestrator.cpp container.cpp mux.cpp
	g++ orchestrator.cpp container.cpp mux.cpp -o orchestrator -lfmt

.PHONY: clean

clean:
	rm -rf orchestrator
	rm -rf scratch/