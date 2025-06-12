import sys
import subprocess
import csv
import re

ENGINE_PATH = "./Engine"
STOCKFISH_PATH = "./stockfish"

def runMyEngine(FEN, depth, moveList=""):
    args = [ENGINE_PATH, str(depth), FEN]
    if moveList:
        args.append(moveList)

    try:
        result = subprocess.run(args, capture_output=True, text=True)
        output = result.stdout
        return output
    except Exception as e:
        print(f"[Engine Error] {e}")
        return None

def runStockfish(FEN, depth, moveList=""):
    try:
        proc = subprocess.Popen([STOCKFISH_PATH], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        def send(cmd):
            proc.stdin.write(cmd + '\n')
            proc.stdin.flush()

        #send("uci")
        #send("isready")
        send(f"position fen {FEN}" + (f" moves {moveList}" if moveList else ""))
        send(f"go perft {depth}")
        send("quit")

        output, _ = proc.communicate()
        return output
    except Exception as e:
        print(f"[Stockfish Error] {e}")
        return None

def parse_perft_output(output: str) -> dict:
    move_dict = {}
    lines = output.strip().splitlines()

    for line in lines:
        if ':' not in line:
            continue

        match = re.match(r'^([a-h][1-8][a-h][1-8][nbrqNBRQ]?):\s*(\d+)$', line.strip())
        if match:
            move = match.group(1)
            count = int(match.group(2))
            move_dict[move] = count

    return move_dict

def extract_total_nodes(output: str) -> int:
    match = re.search(r'Nodes searched:\s*(\d+)', output)
    if match:
        return int(match.group(1))
    lines = output.strip().splitlines()
    for line in reversed(lines):
        if line.strip().isdigit():
            return int(line.strip())
    return -1

def compare_perft_outputs(sf_output: str, engine_output: str):
    sf_moves = parse_perft_output(sf_output)
    engine_moves = parse_perft_output(engine_output)

    all_moves = sorted(set(sf_moves) | set(engine_moves))
    differences_found = False

    print("Move       Stockfish    Your Engine    Difference")
    print("----------------------------------------------------")

    for move in all_moves:
        sf_count = sf_moves.get(move, 0)
        eng_count = engine_moves.get(move, 0)
        diff = sf_count - eng_count

        if diff != 0:
            differences_found = True
            print(f"{move:<10} {sf_count:<12} {eng_count:<13} {diff:+}")
    
    sf_total = extract_total_nodes(sf_output)
    engine_total = extract_total_nodes(engine_output)
    total_diff = sf_total - engine_total

    print("\nTotal Nodes Searched:")
    print(f"  Stockfish:   {sf_total}")
    print(f"  Your Engine: {engine_total}")
    print(f"  Difference:  {total_diff:+}")

    if not differences_found and total_diff == 0:
        print("\n✅ No differences found. Your engine matches Stockfish.")
    elif not differences_found:
        print("\n⚠️ Move counts match, but total node count differs.")
    elif total_diff == 0:
        print("\n⚠️ Per-move differences found, but total node count matches.")
    else:
        print("\n❌ Differences found in per-move and total node counts.")


def main():
    if len(sys.argv) < 2:
        print("Provide file with FEN positions and depths!") 
        sys.exit(1)

    with open(sys.argv[1], 'r') as posInput:
        reader = csv.reader(posInput)

        for args in reader:
            if len(args) == 3:
                myOut = runMyEngine(args[0], args[1], args[2])
                stockOut = runStockfish(args[0], args[1], args[2])
            else:
                myOut = runMyEngine(args[0], args[1])
                stockOut = runStockfish(args[0], args[1])

            #compare the outputs
            compare_perft_outputs(stockOut, myOut)

    return 0

if __name__ == "__main__":
    main()