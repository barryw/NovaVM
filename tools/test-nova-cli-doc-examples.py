#!/usr/bin/env python3
"""Run the command examples documented in docs/books/nova-cli-guide.

The local examples exercise real NDI images. Remote examples run against a
small NovaHost-compatible HTTP server so command syntax, path handling, and
upload/download behavior are tested without requiring hardware.
"""

from __future__ import annotations

import argparse
import contextlib
import http.server
import json
import os
import pathlib
import shlex
import subprocess
import tempfile
import threading
import urllib.parse


REPO = pathlib.Path(__file__).resolve().parents[1]


def nova_command() -> list[str]:
    override = os.environ.get("NOVA_DOC_TEST_NOVA")
    if override:
        return shlex.split(override)

    return ["dotnet", "run", "--project", str(REPO / "e6502.Nova"), "--"]


class MockNovaHost:
    def __init__(self, root: pathlib.Path):
        self.root = root
        self.mounts = {
            "fd0": "",
            "fd1": "",
            "fd2": "",
            "fd3": "",
            "hd0": "",
            "hd1": "",
        }
        self._server: http.server.ThreadingHTTPServer | None = None
        self._thread: threading.Thread | None = None

    @property
    def host(self) -> str:
        assert self._server is not None
        host, port = self._server.server_address
        return f"{host}:{port}"

    def start(self) -> None:
        self._seed()
        owner = self

        class Handler(http.server.BaseHTTPRequestHandler):
            def log_message(self, fmt: str, *args: object) -> None:
                return

            def _json(self, code: int, obj: object) -> None:
                data = json.dumps(obj).encode("utf-8")
                self.send_response(code)
                self.send_header("Content-Type", "application/json")
                self.send_header("Content-Length", str(len(data)))
                self.end_headers()
                self.wfile.write(data)

            def _error(self, code: int, message: str) -> None:
                self._json(code, {"error": message})

            def _sd_path(self) -> pathlib.Path | None:
                parsed = urllib.parse.urlparse(self.path)
                if parsed.path == "/sd":
                    rel = ""
                elif parsed.path.startswith("/sd/"):
                    rel = parsed.path[4:]
                else:
                    return None
                rel_path = pathlib.PurePosixPath(urllib.parse.unquote(rel))
                if rel_path.is_absolute() or ".." in rel_path.parts:
                    return None
                return owner.root / rel_path.as_posix()

            def do_GET(self) -> None:
                parsed = urllib.parse.urlparse(self.path)
                if parsed.path == "/health":
                    self._json(
                        200,
                        {
                            "ok": True,
                            "bootPhase": "ready",
                            "sdMounted": True,
                            "fpgaBridgeAvailable": True,
                            "hostStatusFlags": 0,
                            "hostStatusHex": "0x00",
                            "capabilities": {"nicDispatcher": True},
                        },
                    )
                    return
                if parsed.path == "/sd-status":
                    self._json(
                        200,
                        {
                            "cardType": "SDHC",
                            "cardTypeId": 3,
                            "mounted": True,
                            "bootPhase": "ready",
                            "cardSize": 1024 * 1024 * 1024,
                            "totalBytes": 1024 * 1024 * 1024,
                            "usedBytes": 4096,
                            "bootDiag": "mock NovaHost",
                        },
                    )
                    return
                if parsed.path == "/audio-status":
                    self._json(200, {"playing": False, "kind": "none"})
                    return
                if parsed.path == "/wifi":
                    self._json(
                        200,
                        {
                            "configured": True,
                            "connected": True,
                            "wantConnected": True,
                            "ssid": "NovaLab",
                            "passwordSet": True,
                            "useStatic": False,
                            "staticIp": "",
                            "gateway": "",
                            "subnet": "",
                            "dns": "",
                            "localIp": "127.0.0.1",
                            "mac": "00:00:00:00:00:00",
                            "rssi": -40,
                            "wifiStatus": "WL_CONNECTED",
                            "lastError": "",
                        },
                    )
                    return
                if parsed.path == "/wifi/scan":
                    self._json(200, [{"ssid": "NovaLab", "rssi": -40, "channel": 6, "encrypted": True}])
                    return
                if parsed.path == "/drives":
                    self._json(
                        200,
                        [
                            {
                                "slot": slot,
                                "mounted": bool(path),
                                "currentPath": "/" if not path else path,
                                "configuredPath": path,
                            }
                            for slot, path in owner.mounts.items()
                        ],
                    )
                    return

                sd_path = self._sd_path()
                if sd_path is None:
                    self._error(404, "not found")
                    return
                if sd_path.is_dir():
                    entries = [
                        {"name": child.name, "size": child.stat().st_size, "dir": child.is_dir()}
                        for child in sorted(sd_path.iterdir(), key=lambda p: p.name.lower())
                    ]
                    self._json(200, entries)
                    return
                if sd_path.is_file():
                    data = sd_path.read_bytes()
                    self.send_response(200)
                    self.send_header("Content-Type", "application/octet-stream")
                    self.send_header("Content-Length", str(len(data)))
                    self.end_headers()
                    self.wfile.write(data)
                    return
                self._error(404, "not found")

            def do_PUT(self) -> None:
                parsed = urllib.parse.urlparse(self.path)
                if parsed.path == "/wifi":
                    self.rfile.read(int(self.headers.get("Content-Length", "0")))
                    self._json(200, {"ok": True})
                    return

                sd_path = self._sd_path()
                if sd_path is None:
                    self._error(404, "not found")
                    return
                sd_path.parent.mkdir(parents=True, exist_ok=True)
                length = int(self.headers.get("Content-Length", "0"))
                sd_path.write_bytes(self.rfile.read(length))
                self._json(200, {"ok": True})

            def do_DELETE(self) -> None:
                sd_path = self._sd_path()
                if sd_path is None:
                    self._error(404, "not found")
                    return
                if sd_path.is_dir():
                    sd_path.rmdir()
                elif sd_path.exists():
                    sd_path.unlink()
                else:
                    self._error(404, "not found")
                    return
                self._json(200, {"ok": True})

            def do_POST(self) -> None:
                parsed = urllib.parse.urlparse(self.path)
                if parsed.path in {"/audio-stop", "/reboot"}:
                    self._json(200, {"ok": True})
                    return
                if parsed.path in {
                    "/wifi/connect",
                    "/wifi/disconnect",
                    "/wifi/reconnect",
                    "/wifi/forget",
                }:
                    self._json(202, {"ok": True, "accepted": True})
                    return
                parts = parsed.path.strip("/").split("/")
                if len(parts) == 3 and parts[0] == "drives" and parts[1] in owner.mounts:
                    slot = parts[1]
                    action = parts[2]
                    if action == "unmount":
                        owner.mounts[slot] = ""
                        owner._write_boot_config()
                        self._json(200, {"ok": True, "slot": slot, "mounted": False})
                        return
                    if action == "mount":
                        length = int(self.headers.get("Content-Length", "0"))
                        body = json.loads(self.rfile.read(length) or b"{}")
                        sd_rel = str(body.get("path", "")).lstrip("/")
                        if not sd_rel:
                            self._error(400, "missing path")
                            return
                        if not sd_rel.lower().endswith(".ndi"):
                            self._error(400, "path must end in .ndi")
                            return
                        if not (owner.root / sd_rel).exists():
                            self._error(404, "not found")
                            return
                        owner.mounts[slot] = "/" + sd_rel
                        owner._write_boot_config()
                        self._json(
                            200,
                            {"ok": True, "slot": slot, "mounted": True, "path": "/" + sd_rel},
                        )
                        return
                self._error(404, "not found")

        self._server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Handler)
        self._thread = threading.Thread(target=self._server.serve_forever, daemon=True)
        self._thread.start()

    def stop(self) -> None:
        if self._server is not None:
            self._server.shutdown()
            self._server.server_close()
        if self._thread is not None:
            self._thread.join(timeout=5)

    def _seed(self) -> None:
        for rel in [
            "config",
            "disks/floppy",
            "disks/hard",
            "roms",
            "soundfonts",
            "music",
            "assets/boot",
            "assets/fonts",
            "assets/sid",
        ]:
            (self.root / rel).mkdir(parents=True, exist_ok=True)
        self._write_boot_config()

    def _write_boot_config(self) -> None:
        (self.root / "config").mkdir(parents=True, exist_ok=True)
        (self.root / "config" / "boot.json").write_text(
            json.dumps({"mounts": self.mounts}, indent=2) + "\n",
            encoding="utf-8",
        )


def run(cmd: list[str], cwd: pathlib.Path = REPO) -> str:
    label = " ".join(shlex.quote(part) for part in cmd)
    print(f"$ {label}")
    result = subprocess.run(
        cmd,
        cwd=cwd,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )
    print(result.stdout.rstrip())
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit code {result.returncode}: {label}")
    return result.stdout


def run_nova(args: list[str]) -> str:
    return run(nova_command() + args)


def write_basic_source(path: pathlib.Path) -> None:
    path.write_text(
        "\n".join(
            [
                "10 PRINT \"NOVA CLI DOC TEST\"",
                "20 FOR I=1 TO 3",
                "30 PRINT I",
                "40 NEXT I",
                "50 END",
                "",
            ]
        ),
        encoding="ascii",
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--keep-temp", action="store_true")
    args = parser.parse_args()

    temp_cm = tempfile.TemporaryDirectory(prefix="nova-cli-doc-")
    with contextlib.ExitStack() as stack:
        temp_name = temp_cm.name
        if args.keep_temp:
            temp_cm.cleanup = lambda: None  # type: ignore[method-assign]
        stack.enter_context(temp_cm)

        work = pathlib.Path(temp_name)
        sd_root = work / "sd"
        sd_root.mkdir()
        host = MockNovaHost(sd_root)
        host.start()
        stack.callback(host.stop)

        image = work / "doc-demo.ndi"
        hd_image = work / "hd0.ndi"
        scratch_image = work / "scratch.ndi"
        source = work / "AUTOBOOT.bas"
        tokenized = work / "AUTOBOOT.tokenized.bas"
        detok = work / "AUTOBOOT.roundtrip.txt"
        export_dir = work / "exported"
        export_dir.mkdir()
        soundfont = work / "DocBank.nsfb"
        old_file = work / "old.bin"
        soundfont.write_bytes(b"NSFB-DOC-TEST\n")
        old_file.write_bytes(b"old\n")
        write_basic_source(source)

        run_nova(["create", str(image), "--label", "DOCDEMO"])
        run_nova(["create", str(hd_image), "--hd", "--label", "HOME"])
        run_nova(["create", str(scratch_image), "--size", "4096", "--label", "SCRATCH"])
        run_nova(["info", str(image)])
        run_nova(["validate", str(image)])
        run_nova(["label", str(image), "DOCS"])
        run_nova(["mkdir", str(image), "/programs"])
        run_nova(["mkdir", str(image), "/empty"])
        run_nova(["rmdir", str(image), "/empty"])
        run_nova(["tokenize", str(source), str(tokenized), "--base", "$0301"])
        run_nova(["detokenize", str(tokenized), str(detok)])
        run_nova(["detokenize", str(tokenized)])
        run_nova(["import", str(image), str(source), "/programs", "--tokenize"])
        run_nova(["import", str(image), str(REPO / "ehbasic/basic.bin")])
        run_nova(["import", str(image), str(REPO / "e6502.ESP32/novahost/assets/boot/novavm_logo.nvg"), "/programs"])
        run_nova(["import", str(image), str(REPO / "docs/programs/midi/sousa-stars-stripes.mid"), "/programs"])
        run_nova(["dir", str(image)])
        run_nova(["dir", str(image), "/programs"])
        run_nova(["export", str(image), "/programs/AUTOBOOT.bas", str(export_dir), "--detokenize"])
        run_nova(["export", str(image), "/basic.bin", str(work / "basic-export.bin")])
        run_nova(["delete", str(image), "/programs/novavm_logo.nvg"])
        run_nova(["validate", str(image)])

        remote = host.host
        run_nova(["device", "status", "--remote", remote])
        run_nova(["ls", "--remote", remote])
        run_nova(["put", "--remote", remote, str(image), "disks/floppy/doc-demo.ndi"])
        run_nova(["put", "--remote", remote, str(REPO / "docs/programs/midi/sousa-stars-stripes.mid"), "music/RAW-STARS.NMS"])
        run_nova(["put", "--remote", remote, str(old_file), "tmp/old.bin"])
        run_nova(["get", "--remote", remote, "config/boot.json", str(work / "boot.downloaded.json")])
        run_nova(["rm", "--remote", remote, "tmp/old.bin"])
        run_nova(["ls", "--remote", remote, "disks/floppy"])
        run_nova(["disk", "list", "--remote", remote])
        run_nova(["disk", "list", "--remote", remote, "--floppy"])
        run_nova(["disk", "upload", str(image), "--remote", remote, "--floppy", "--name", "doc-demo.ndi"])
        run_nova(["disk", "download", "doc-demo.ndi", "--remote", remote, "--floppy", str(work / "doc-demo.downloaded.ndi")])
        run_nova(["rom", "upload", str(REPO / "ehbasic/basic.bin"), "--remote", remote, "--name", "novabasic.bin"])
        run_nova(["rom", "list", "--remote", remote])
        run_nova(["rom", "download", "novabasic.bin", "--remote", remote, str(work / "novabasic.downloaded.bin")])
        run_nova(["soundfont", "upload", str(soundfont), "--remote", remote])
        run_nova(["soundfont", "list", "--remote", remote])
        run_nova(["soundfont", "download", "DocBank.nsfb", "--remote", remote, str(work / "DocBank.downloaded.nsfb")])
        run_nova(["music", "upload", str(REPO / "docs/programs/midi/sousa-stars-stripes.mid"), "--remote", remote, "--name", "STARS.NMS"])
        run_nova(["music", "list", "--remote", remote])
        run_nova(["music", "download", "STARS.NMS", "--remote", remote, str(work / "STARS.downloaded.NMS")])
        run_nova(["asset", "upload", str(REPO / "e6502.ESP32/novahost/assets/boot/novavm_logo.nvg"), "--remote", remote, "--type", "boot"])
        run_nova(["asset", "list", "--remote", remote, "--type", "boot"])
        run_nova(["asset", "download", "novavm_logo.nvg", "--remote", remote, "--type", "boot", str(work / "novavm_logo.downloaded.nvg")])
        run_nova(["drive", "list", "--remote", remote])
        run_nova(["drive", "mount", "fd0", "/disks/floppy/doc-demo.ndi", "--remote", remote])
        run_nova(["drive", "list", "--remote", remote])
        run_nova(["drive", "unmount", "fd0", "--remote", remote])
        run_nova(["wifi", "status", "--remote", remote])
        run_nova(["wifi", "scan", "--remote", remote])
        run_nova(["wifi", "set", "--remote", remote, "--ssid", "NovaLab", "--dhcp"])
        run_nova([
            "wifi",
            "set",
            "--remote",
            remote,
            "--ssid",
            "NovaLab",
            "--password",
            "secret",
            "--static",
            "--static-ip",
            "192.168.1.65",
            "--gateway",
            "192.168.1.1",
            "--subnet",
            "255.255.255.0",
            "--dns",
            "192.168.1.1",
        ])
        run_nova(["wifi", "connect", "--remote", remote])
        run_nova(["wifi", "disconnect", "--remote", remote])
        run_nova(["wifi", "reconnect", "--remote", remote])
        run_nova(["wifi", "forget", "--remote", remote])
        run_nova(["audio", "status", "--remote", remote])
        run_nova(["audio", "stop", "--remote", remote])
        run_nova(["device", "reboot", "--remote", remote])

        base_url = f"http://{remote}"
        run(["curl", "-fsS", f"{base_url}/health"])
        run(["curl", "-fsS", f"{base_url}/sd-status"])
        run(["curl", "-fsS", f"{base_url}/sd/"])
        run(["curl", "-fsS", f"{base_url}/sd/config/boot.json"])
        run(["curl", "-fsS", "-X", "PUT", "--data-binary", f"@{old_file}", f"{base_url}/sd/tmp/curl.bin"])
        run(["curl", "-fsS", "-X", "DELETE", f"{base_url}/sd/tmp/curl.bin"])
        run(["curl", "-fsS", f"{base_url}/drives"])
        run([
            "curl",
            "-fsS",
            "-X",
            "POST",
            "-H",
            "Content-Type: application/json",
            "--data",
            '{"path":"/disks/floppy/doc-demo.ndi"}',
            f"{base_url}/drives/fd0/mount",
        ])
        run(["curl", "-fsS", "-X", "POST", f"{base_url}/drives/fd0/unmount"])
        run(["curl", "-fsS", f"{base_url}/wifi"])
        run(["curl", "-fsS", f"{base_url}/wifi/scan"])
        run([
            "curl",
            "-fsS",
            "-X",
            "PUT",
            "-H",
            "Content-Type: application/json",
            "--data",
            '{"ssid":"NovaLab","dhcp":true}',
            f"{base_url}/wifi",
        ])
        run([
            "curl",
            "-fsS",
            "-X",
            "PUT",
            "-H",
            "Content-Type: application/json",
            "--data",
            '{"ssid":"NovaLab","password":"secret","useStatic":true,"staticIp":"192.168.1.65","gateway":"192.168.1.1","subnet":"255.255.255.0","dns":"192.168.1.1"}',
            f"{base_url}/wifi",
        ])
        run(["curl", "-fsS", "-X", "POST", f"{base_url}/wifi/reconnect"])
        run(["curl", "-fsS", f"{base_url}/audio-status"])
        run(["curl", "-fsS", "-X", "POST", f"{base_url}/audio-stop"])
        run(["curl", "-fsS", "-X", "POST", f"{base_url}/reboot"])
        run_nova(["disk", "delete", "doc-demo.ndi", "--remote", remote, "--floppy"])

        print(f"\nAll documented Nova CLI examples passed. Temp: {work}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
