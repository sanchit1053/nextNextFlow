orchestrator: orchestrator.cpp container.hpp container.cpp mux.cpp channel.hpp parser.hpp 
	g++ orchestrator.cpp container.cpp mux.cpp -o orchestrator -lfmt

.PHONY: clean

clean:
	rm -rf orchestrator
	rm -rf scratch/
	rm -rf storage/
