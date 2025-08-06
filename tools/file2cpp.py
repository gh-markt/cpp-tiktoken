import sys
import os

def file_to_cpp_array(input_file, output_file, array_name="data"):
    # Read the binary content of the input file
    with open(input_file, "rb") as f:
        byte_data = f.read()

    # Format bytes as hex values
    hex_bytes = [f"0x{b:02x}" for b in byte_data]
    
    # Prepare C++ array string with 12 bytes per line
    lines = []
    bytes_per_line = 12
    for i in range(0, len(hex_bytes), bytes_per_line):
        line = ", ".join(hex_bytes[i:i+bytes_per_line])
        lines.append("    " + line)
    
    array_length = len(byte_data)

    # Write to cpp file
    with open(output_file, "w") as cpp_file:
        cpp_file.write(f"// Generated from {os.path.basename(input_file)}\n")
        cpp_file.write("#include <utility>\n")
        cpp_file.write(f"static const unsigned char {array_name}[] = {{\n")
        cpp_file.write(",\n".join(lines))
        cpp_file.write(f"\n}};\n\n")
        cpp_file.write(f"static const unsigned int {array_name}_len = {array_length};\n")
        cpp_file.write(f"std::pair<const unsigned char*, size_t> get_{array_name}() {{ return {{ {array_name}, {array_name}_len }}; }}")

    #print(f"Written {array_length} bytes to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python file_to_cpp_array.py <input_file> <output_file> [array_name]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    array_name = sys.argv[3] if len(sys.argv) > 3 else "data"

    file_to_cpp_array(input_file, output_file, array_name)