"""Generate embedded resources using only Python's standard library."""

import io
from pathlib import Path
import sys
import zipfile


def array(name, data):
    # Individual character literals avoid MSVC's limit on concatenated strings.
    values = [f"'\\x{byte:02x}'" for byte in data] + ["'\\0'"]
    lines = [", ".join(values[i:i + 16]) for i in range(0, len(values), 16)]
    return f"static const char {name}[] = {{\n    " + ",\n    ".join(lines) + "\n};\n"


def embed(name, data):
    return "#include <cstddef>\n" + array(name, data) + f"static const size_t {name}_len = {len(data)};\n"


def generate(mode, output, inputs):
    if mode == "mascot":
        result = '#include "DefaultMascot.hpp"\n\n'
        entries = []
        for index, path in enumerate(inputs):
            data = path.read_bytes()
            symbol = f"mascot_{index}"
            result += array(symbol, data)
            entries.append(f'    {{ "{path.name}", {{ {symbol}, {len(data)} }} }}')
        result += "\nconst std::map<std::string, std::pair<const char *, size_t>> defaultMascot = {\n"
        return result + ",\n".join(entries) + "\n};\n"
    if mode == "licenses":
        data = b"Licenses for the software components used in Shijima-Qt are listed below.\n"
        for path in inputs:
            data += f"\n\n~~~~~~~~~~ {path.name} ~~~~~~~~~~\n\n".encode("utf-8") + path.read_bytes()
        return array("shijima_licenses", data)
    if mode == "gnome":
        buffer = io.BytesIO()
        with zipfile.ZipFile(buffer, "w", compression=zipfile.ZIP_DEFLATED) as archive:
            for path in inputs:
                # Fixed timestamps and permissions make the embedded ZIP reproducible.
                info = zipfile.ZipInfo(path.name, date_time=(1980, 1, 1, 0, 0, 0))
                info.compress_type = zipfile.ZIP_DEFLATED
                info.external_attr = 0o100644 << 16
                archive.writestr(info, path.read_bytes())
        return embed("gnome_script", buffer.getvalue())
    if mode == "embed" and len(inputs) == 1:
        return embed(output.stem, inputs[0].read_bytes())
    raise ValueError(f"Invalid resource mode or input count: {mode}")


def main():
    if len(sys.argv) < 4:
        raise SystemExit("Usage: generate_resources.py MODE OUTPUT INPUT...")
    mode, output_name, *input_names = sys.argv[1:]
    output = Path(output_name)
    result = generate(mode, output, [Path(name) for name in input_names])
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(result.encode("utf-8"))


if __name__ == "__main__":
    main()
