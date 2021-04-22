#!/usr/bin/env python3

import binascii
import sys

def main():
    if len(sys.argv) < 2:
        print("Please specify the hex string to be converted")
        return
    output_file = None

    if len(sys.argv) == 3:
        output_file = sys.argv[2]

    if output_file is None:
        output_file = "hex.bin"

    binstr = binascii.unhexlify(sys.argv[1])
    with open(output_file, "wb") as f: f.write(binstr)

    print("Converted hex into {}".format(output_file))

if __name__ == "__main__":
    # execute only if run as a script
    main()