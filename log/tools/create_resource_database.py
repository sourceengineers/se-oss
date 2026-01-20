#!/usr/bin/env python3

import json
import hashlib
import re
import subprocess
import sys

def git_revision_hash() -> str:
    try:
        return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip()
    except subprocess.CalledProcessError:
        return "unknown"

def git_repo_is_dirty() -> bool:
    try:
        return subprocess.check_output(['git', 'status', '--porcelain']).decode('ascii').strip() != ""
    except subprocess.CalledProcessError:
        return True

def get_id(string: str):
    hash_valued = hashlib.sha256(string.encode('utf-8')).digest()
    return int.from_bytes(hash_valued[:4], byteorder='big')


if __name__ == '__main__':
    database_template = sys.argv[1]
    database_output_cpp = sys.argv[2]
    database_output_json = sys.argv[3]

    # collect format strings
    messages = {}
    id_counter = 0
    for symbol in sys.stdin:
        match = re.search("getResourceId<(?:se_oss::)?ResourceIdentifier<(.*) >", symbol)
        if match is None:
            continue

        chars = re.findall(r"\(char\)(\d+)", match.group(1))
        if chars is None:
            continue

        message = "".join(chr(int(c)) for c in chars).rstrip('\x00')
        messages[get_id(message)] = message

    # create C++ output
    lookup_functions = []
    for string_id, message in messages.items():
        template_parameters = ", ".join(str(ord(c)) for c in list(message))
        cleaned_message = message.encode('unicode_escape').decode('utf-8')
        lookup_functions.append(f"//! Get ID for string '{cleaned_message}'")
        lookup_functions.append("template<>")
        lookup_functions.append(f"uint32_t getResourceId<ResourceIdentifier<{template_parameters}, 0>>() {{ return 0x{string_id:08X}; }}\n")

    template_file = open(database_template)
    template_cpp = template_file.read()
    template_file.close()

    template_cpp = template_cpp.replace("{{lookup_functions}}", "\n".join(lookup_functions))

    lookup_file = open(database_output_cpp, "w")
    lookup_file.write(template_cpp)
    lookup_file.close()

    # create the string look-up database
    with open(database_output_json, "w") as file:
        resource_db = {
            "revision": {
                "git-hash": git_revision_hash(),
                "dirty": git_repo_is_dirty(),
            },
            "resources": messages
        }
        json.dump(resource_db, file, indent=2)

    exit(0)