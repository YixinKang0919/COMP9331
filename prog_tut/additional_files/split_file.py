#! /usr/bin/env python3

"""
Script that takes as arguments a text file and a number, and splits the file into
the specified number of smaller files.  The lines of the original file are
distributed evenly among the smaller files.

Usage: python3 split_file.py <file_to_split> <num_splits>

Example: python3 split_file.py rockyou100k.txt 5

This will split the file rockyou100k.txt into 5 smaller files, each containing
approximately 20,000 lines, named rockyou100k-1.txt, ..., rockyou100k-5.txt.

Written by Tim Arney <t.arney@unsw.edu.au>
For COMP3331/9331 Computer Networks and Applications
"""

from pathlib import Path
import sys


def main():
    if len(sys.argv) != 3:
        sys.exit(f'Usage: {sys.argv[0]} <file_to_split> <num_splits>')

    file_to_split = Path(sys.argv[1])

    if not file_to_split.exists():
        sys.exit(f"Error: file not found: {file_to_split}")

    try:
        num_splits = int(sys.argv[2])
    except ValueError:
        sys.exit("Error: invalid num_splits '{num_splits}', must be an integer.")

    split_file(file_to_split, num_splits)


def split_file(file_to_split, num_splits):
    split_files = []
    
    for i in range(1, num_splits + 1):
        split_file = file_to_split.with_name(f'{file_to_split.stem}-{i}{file_to_split.suffix}')
        f = open(split_file, 'w')
        split_files.append(f)

    with open(file_to_split, 'r') as f:
        for i, line in enumerate(f):
            j = i % num_splits
            split_files[j].write(line)

    for f in split_files:
        f.close()


if __name__ == '__main__':
    main()
