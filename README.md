# Archangel

Archangel is a UCI-compatible chess engine written in C++20.

The project includes:
- A playable engine binary that communicates through the UCI protocol
- A perft test binary for move generation validation
- Bitboard-based board representation and move generation
- Alpha-beta search with iterative deepening and a handcrafted evaluation function

## Project Structure

- `main.cpp`: UCI loop and search entrypoint
- `perft.cpp`: perft driver
- `movegen/`: board state management and legal move generation
- `engine/`: search and evaluation
- `containers/`: custom move list container
- `types/`: core data types (`Move`, `BoardState`, etc.)
- `scripts/`: helper scripts for compilation and perft runs

## Requirements

- `clang++` with C++20 support
- OpenMP (`libomp`) available to the compiler/linker
- macOS/Homebrew paths in scripts currently target:
  - `/opt/homebrew/opt/libomp/include`
  - `/opt/homebrew/opt/libomp/lib`

If your system differs, update the compile flags in `scripts/compile.sh` and `scripts/perft.sh`.

## Build

### Engine (UCI)

From the repo root:

```bash
cd scripts
./compile.sh
```

`compile.sh` outputs a binary named `RENAME-ME.bin` in `scripts/`.
Rename it to your preferred engine name, for example:

```bash
mv RENAME-ME.bin archangel.bin
```

### Perft Tester

From `scripts/`:

```bash
./perft.sh
```

This builds `perft-tester.bin` on first run.

## Run

### UCI mode

```bash
cd scripts
./archangel.bin
```

Example UCI session:

```text
uci
isready
position startpos moves e2e4 e7e5
go depth 5
quit
```

Supported `go` inputs in the current implementation:
- `go depth <n>`
- `go movetime <ms>`
- `go wtime <ms>` / `go btime <ms>` (engine uses ~1/20 of side-to-move time)

### Perft

From `scripts/`:

```bash
./perft.sh <depth> "<fen>" "<moves>"
```

Examples:

```bash
./perft.sh 4
./perft.sh 3 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
./perft.sh 3 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" "e2e4 e7e5"
```

## Notes

- Search currently uses alpha-beta with iterative deepening and time cutoff.
- Evaluation is based on material, pawn structure/castling factors, piece-square tables, and king position heuristics.
- Build scripts are tuned for Apple Silicon (`-mcpu=apple-m4`), so you may want to adjust flags for other CPUs.
