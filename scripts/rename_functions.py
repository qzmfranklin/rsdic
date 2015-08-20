#!/usr/bin/python3

import argparse
import os
import shlex
import sys
import subprocess

if __name__ == '__main__':
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter,
            description = 'Rename a list a function names')

    parser.add_argument('pair_list',
            metavar = 'PAIR',
            type = argparse.FileType('r', encoding = 'utf8'),
            help = 'A list of separated pairs, where first => second')
    parser.add_argument('-d', '--delimiter',
            type = str,
            default = ':',
            help = 'Delimiter')

    args = parser.parse_args()

    for line in args.pair_list:
        if line[0] == '#':
            continue
        pair = line.rstrip('\n').split(args.delimiter)
        old = pair[0].strip()
        new = pair[1].strip()
        cmd = [
                'git', 'grep', '-l',
                shlex.quote(old),
                'lib/',
                '|',
                'xargs', 'sed', '-i', "''",
                "'{}'".format(
                    "s/[[:<:]]{}[[:>:]]/{}/g".format(shlex.quote(old), shlex.quote(new))
                    )
                ]
        print(subprocess.list2cmdline(cmd))
        #subprocess.call(cmd, shell = True)

