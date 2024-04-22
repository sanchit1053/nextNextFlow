orchestrator: orchestrator.cpp container.cpp
	g++ orchestrator.cpp container.cpp -o orchestrator -lfmt

.PHONY: clean

clean:
	rm -rf orchestrator
	rm -rf scratch/