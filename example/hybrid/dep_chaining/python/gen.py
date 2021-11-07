import argparse

def cli_arguments():
    parser = argparse.ArgumentParser(description="Generates a .c / .cpp source file at provided destination location")
    parser.add_argument('--source_type', required=True, choices=["c", "cpp"])
    parser.add_argument('--destination', required=True)
    args = parser.parse_args()
    print("Source Type", args.source_type)
    print("Destination", args.destination)

    return (args.source_type, args.destination)

# Run
source_type, destination = cli_arguments()
file = open(destination, "w")

source = ""
if source_type == "c":
    source = """
    #include <stdio.h>

    int main() {
        printf("Hello World from C");
        return 0;
    }
    """
elif source_type == "cpp":
    source = """
    #include <iostream>

    int main() {
        std::cout << "Hello World from CPP" << std::endl;
        return 0;
    }
    """

file.write(source)
file.close()
