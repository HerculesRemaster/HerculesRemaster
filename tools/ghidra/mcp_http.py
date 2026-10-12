"""Small stdlib client for the GhidraMCP GUI plugin HTTP API (127.0.0.1:8089).

Every call names its program (pass program=None only for project-wide endpoints
such as /list_open_programs or /import_file). Server errors raise GhidraError.
The endpoint list with methods and parameter sources is served at /mcp/schema.

CLI (writes the response to a file, prints a one-line summary):
    mcp_http.py get <endpoint> -p <program> [-q k=v ...] -o out.json [--all KEY]
    mcp_http.py post <endpoint> -p <program> [-b k=v ...] -o out.json
--all KEY follows offset/limit paging and merges the list under KEY.
"""
from __future__ import annotations

import argparse
import json
import os
import sys
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

BASE_URL = os.environ.get("GHIDRA_MCP_URL", "http://127.0.0.1:8089").rstrip("/")
PAGE = 500


class GhidraError(RuntimeError):
    pass


def _request(method: str, endpoint: str, program: str | None, query: dict | None,
             body: dict | None, timeout: float):
    query = {k: v for k, v in (query or {}).items() if v is not None}
    if program is not None:
        query["program"] = program
    url = f"{BASE_URL}/{endpoint.lstrip('/')}"
    if query:
        url += "?" + urllib.parse.urlencode(query)
    data = None
    headers = {}
    if method == "POST":
        data = json.dumps(body or {}).encode()
        headers["Content-Type"] = "application/json"
    req = urllib.request.Request(url, data=data, method=method, headers=headers)
    try:
        with urllib.request.urlopen(req, timeout=timeout) as r:
            text = r.read().decode("utf-8", "replace")
    except urllib.error.HTTPError as e:
        raise GhidraError(f"{method} {endpoint}: HTTP {e.code}: {e.read()[:300]!r}") from e
    except urllib.error.URLError as e:
        raise GhidraError(f"{method} {endpoint}: {e.reason} (run tools/ghidra/ensure_ghidra.ps1)") from e
    try:
        result = json.loads(text)
    except json.JSONDecodeError:
        return text
    if isinstance(result, dict) and "error" in result:
        raise GhidraError(f"{method} {endpoint}: {result['error']}")
    return result


def get(endpoint: str, program: str | None, timeout: float = 120, **query):
    return _request("GET", endpoint, program, query, None, timeout)


def post(endpoint: str, program: str | None, body: dict, timeout: float = 600, **query):
    return _request("POST", endpoint, program, query, body, timeout)


def get_all(endpoint: str, program: str | None, key: str, page: int = PAGE, **query) -> list:
    """Follow offset/limit paging until `total` is reached; return the merged list."""
    items: list = []
    offset = 0
    while True:
        r = get(endpoint, program, offset=offset, limit=page, **query)
        chunk = r.get(key, [])
        items.extend(chunk)
        offset += len(chunk)
        total = r.get("total")
        if not chunk or (total is not None and offset >= total) or (total is None and len(chunk) < page):
            return items


def write_json(path: Path, obj) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(obj, indent=1), encoding="utf-8")


def _kv(pairs: list[str]) -> dict:
    out = {}
    for p in pairs:
        k, _, v = p.partition("=")
        out[k] = v
    return out


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("method", choices=["get", "post"])
    ap.add_argument("endpoint")
    ap.add_argument("-p", "--program", required=True, help="program name, or '-' for project-wide endpoints")
    ap.add_argument("-q", "--query", nargs="*", default=[], help="query params k=v")
    ap.add_argument("-b", "--body", nargs="*", default=[], help="POST body fields k=v (strings)")
    ap.add_argument("-o", "--out", required=True, type=Path)
    ap.add_argument("--all", metavar="KEY", help="page through and merge the list under KEY")
    a = ap.parse_args(argv)
    program = None if a.program == "-" else a.program
    try:
        if a.method == "get" and a.all:
            result = get_all(a.endpoint, program, a.all, **_kv(a.query))
            count = len(result)
        elif a.method == "get":
            result = get(a.endpoint, program, **_kv(a.query))
            count = result.get("count", "-") if isinstance(result, dict) else "-"
        else:
            result = post(a.endpoint, program, _kv(a.body), **_kv(a.query))
            count = "-"
    except GhidraError as e:
        print(f"error: {e}", file=sys.stderr)
        return 1
    write_json(a.out, result)
    print(f"{a.method.upper()} /{a.endpoint.lstrip('/')} program={a.program} count={count} -> {a.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
