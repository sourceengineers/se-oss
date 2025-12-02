#!/usr/bin/env python3

import json
import hashlib
import re
import sys


# id_counter = 0
# def get_id(string):
#     global id_counter
#     id_counter += 1
#     return id_counter

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
        match = re.search("getStringId<(?:se_oss::)?TemplateText<(.*) >", symbol)
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
        lookup_functions.append(f"//! Get ID for string '{message}'")
        lookup_functions.append("template<>")
        lookup_functions.append(f"uint32_t getStringId<TemplateText<{template_parameters}, 0>>() {{ return 0x{string_id:08X}; }}\n")

    template_file = open(database_template)
    template_cpp = template_file.read()
    template_file.close()

    template_cpp = template_cpp.replace("{{lookup_functions}}", "\n".join(lookup_functions))

    lookup_file = open(database_output_cpp, "w")
    lookup_file.write(template_cpp)
    lookup_file.close()

    # create the string look-up database
    with open(database_output_json, "w") as file:
        json.dump(messages, file, indent=2)

    exit(0)