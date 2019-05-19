build_dir:
	mkdir -p build

compile: build Make/build.sh
	Make/build.sh

test: build compile Make/test.sh
	Make/test.sh

random_game: compile build build/random_game
	build/random_game