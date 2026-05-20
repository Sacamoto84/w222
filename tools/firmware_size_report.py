#!/usr/bin/env python3
"""Build a self-contained HTML firmware size report from an ESP-IDF map file."""

from __future__ import annotations

import argparse
import json
import os
import pathlib
import re
import subprocess
import sys
from html import escape


def find_idf_size(explicit: str | None) -> pathlib.Path:
    if explicit:
        return pathlib.Path(explicit)

    idf_path = os.environ.get("IDF_PATH")
    if idf_path:
        candidate = pathlib.Path(idf_path) / "tools" / "idf_size.py"
        if candidate.exists():
            return candidate

    common_candidates = (
        pathlib.Path("C:/esp/v5.5.4/esp-idf/tools/idf_size.py"),
        pathlib.Path("C:/Espressif/frameworks/esp-idf-v5.5.4/tools/idf_size.py"),
        pathlib.Path("D:/Espressif5/frameworks/esp-idf-v5.4.4/tools/idf_size.py"),
    )
    for candidate in common_candidates:
        if candidate.exists():
            return candidate

    raise FileNotFoundError("Cannot find idf_size.py. Pass --idf-size or run from an ESP-IDF exported terminal.")


def find_idf_python(idf_size: pathlib.Path) -> str:
    """Return the Python interpreter that should be used to run idf_size.py.

    On Windows the ESP-IDF tools installer places a versioned venv under
    C:/Espressif/tools/python/vX.Y.Z/venv/Scripts/python.exe.  We try to
    derive it from the path to idf_size.py so the user does not have to
    launch the report script from an exported ESP-IDF terminal.
    """
    idf_python = os.environ.get("IDF_PYTHON")
    if idf_python:
        return idf_python

    idf_size_str = str(idf_size)
    m = re.search(r"v(\d+\.\d+\.\d+)", idf_size_str)
    if m:
        version = m.group(0)  # e.g. v5.5.4
        candidates = [
            pathlib.Path(f"C:/Espressif/tools/python/{version}/venv/Scripts/python.exe"),
            pathlib.Path(f"D:/Espressif5/tools/python/{version}/venv/Scripts/python.exe"),
        ]
        for candidate in candidates:
            if candidate.exists():
                return str(candidate)

    return sys.executable


def run_idf_size(idf_size: pathlib.Path, map_file: pathlib.Path, mode: str) -> dict:
    cmd = [
        find_idf_python(idf_size),
        str(idf_size),
        str(map_file),
        mode,
        "--format",
        "json2",
        "--no-color",
    ]
    try:
        result = subprocess.run(cmd, check=True, text=True, encoding="utf-8", capture_output=True)
    except subprocess.CalledProcessError as exc:
        print(f"[ERROR] idf_size.py failed (exit {exc.returncode}):", file=sys.stderr)
        if exc.stdout:
            print(exc.stdout, file=sys.stderr)
        if exc.stderr:
            print(exc.stderr, file=sys.stderr)
        raise
    return json.loads(result.stdout)


def flatten_items(raw: dict, kind: str) -> list[dict]:
    items: list[dict] = []
    for full_name, info in raw.items():
        size = int(info.get("size", 0))
        if size <= 0:
            continue

        memory_types = info.get("memory_types", {})
        memory = {name: int(value.get("size", 0)) for name, value in memory_types.items()}
        sections: dict[str, int] = {}
        for mem_info in memory_types.values():
            for section_name, section_info in mem_info.get("sections", {}).items():
                short_name = section_info.get("abbrev_name", section_name)
                sections[short_name] = sections.get(short_name, 0) + int(section_info.get("size", 0))

        archive = ""
        object_file = ""
        if kind == "files" and ":" in full_name:
            archive, object_file = full_name.split(":", 1)

        items.append(
            {
                "name": info.get("abbrev_name") or pathlib.PurePosixPath(full_name.replace("\\", "/")).name,
                "full": full_name,
                "archive": archive,
                "object": object_file,
                "kind": kind,
                "size": size,
                "memory": memory,
                "sections": sections,
            }
        )

    items.sort(key=lambda item: item["size"], reverse=True)
    return items


HTML_TEMPLATE = r"""<!doctype html>
<html lang="ru">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP-IDF Firmware Size</title>
<style>
:root {
  color-scheme: dark;
  --bg: #0f1218;
  --panel: #181d27;
  --panel2: #202634;
  --text: #eef3fb;
  --muted: #96a2b3;
  --line: #323a4b;
}
* { box-sizing: border-box; }
body {
  margin: 0;
  font-family: Segoe UI, Roboto, Arial, sans-serif;
  background: var(--bg);
  color: var(--text);
}
header {
  padding: 18px 22px 12px;
  border-bottom: 1px solid var(--line);
  background: #121720;
}
h1 {
  margin: 0 0 8px;
  font-size: 22px;
  font-weight: 650;
}
.meta {
  color: var(--muted);
  font-size: 13px;
}
.toolbar {
  display: grid;
  grid-template-columns: repeat(4, minmax(130px, 1fr));
  gap: 10px;
  padding: 14px 22px;
  background: #11161f;
  border-bottom: 1px solid var(--line);
}
label {
  display: grid;
  gap: 5px;
  color: var(--muted);
  font-size: 12px;
}
select, input {
  width: 100%;
  height: 36px;
  border: 1px solid var(--line);
  border-radius: 7px;
  background: var(--panel);
  color: var(--text);
  padding: 0 10px;
  font: inherit;
}
main {
  display: grid;
  grid-template-columns: minmax(0, 1fr) 430px;
  gap: 14px;
  padding: 14px 22px 22px;
}
.panel {
  min-width: 0;
  background: var(--panel);
  border: 1px solid var(--line);
  border-radius: 8px;
  overflow: hidden;
}
.panel h2 {
  margin: 0;
  padding: 12px 14px;
  font-size: 14px;
  font-weight: 650;
  color: #dce6f5;
  background: var(--panel2);
  border-bottom: 1px solid var(--line);
}
#treemap {
  position: relative;
  height: calc(100vh - 205px);
  min-height: 560px;
  overflow: hidden;
}
.cell {
  position: absolute;
  border: 1px solid rgba(255,255,255,.12);
  overflow: hidden;
  padding: 7px;
  line-height: 1.15;
  cursor: default;
}
.cell:hover {
  outline: 2px solid rgba(255,255,255,.75);
  z-index: 2;
}
.cell-name {
  font-size: 12px;
  font-weight: 650;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}
.cell-size {
  margin-top: 3px;
  font-size: 11px;
  color: rgba(255,255,255,.82);
}
table {
  width: 100%;
  border-collapse: collapse;
  font-size: 12px;
}
th, td {
  padding: 8px 10px;
  border-bottom: 1px solid var(--line);
  text-align: left;
}
th {
  color: var(--muted);
  background: #151a23;
  position: sticky;
  top: 0;
}
td.size {
  white-space: nowrap;
  text-align: right;
  font-variant-numeric: tabular-nums;
}
.bar {
  height: 5px;
  margin-top: 5px;
  border-radius: 999px;
  background: #324052;
}
.bar > i {
  display: block;
  height: 100%;
  border-radius: inherit;
  background: #65d6ad;
}
#details {
  padding: 12px 14px;
  color: var(--muted);
  font-size: 13px;
  border-bottom: 1px solid var(--line);
  min-height: 66px;
}
#tableWrap {
  max-height: calc(100vh - 318px);
  overflow: auto;
}
@media (max-width: 1050px) {
  main { grid-template-columns: 1fr; }
  .toolbar { grid-template-columns: repeat(2, minmax(130px, 1fr)); }
  #treemap { height: 620px; }
  #tableWrap { max-height: none; }
}
</style>
</head>
<body>
<header>
  <h1>ESP-IDF Firmware Size</h1>
  <div class="meta">Map: <code>__MAP_FILE__</code> · Generated: __GENERATED__</div>
</header>
<section class="toolbar">
  <label>Вид
    <select id="kind">
      <option value="archives">Компоненты / библиотеки</option>
      <option value="files">Файлы .obj</option>
    </select>
  </label>
  <label>Память
    <select id="metric">
      <option value="size">Total Size</option>
      <option value="External RAM">Flash / External RAM</option>
      <option value="DIRAM">DIRAM</option>
      <option value=".text">.text</option>
      <option value=".rodata">.rodata</option>
      <option value=".bss">.bss</option>
      <option value=".data">.data</option>
    </select>
  </label>
  <label>Фильтр
    <input id="filter" placeholder="lvgl, font, hosted...">
  </label>
  <label>Показывать
    <select id="limit">
      <option value="40">Top 40</option>
      <option value="80">Top 80</option>
      <option value="200">Top 200</option>
      <option value="99999">Все</option>
    </select>
  </label>
</section>
<main>
  <section class="panel">
    <h2 id="mapTitle">Treemap</h2>
    <div id="treemap"></div>
  </section>
  <aside class="panel">
    <h2>Top</h2>
    <div id="details">Наведи на прямоугольник, чтобы увидеть детали.</div>
    <div id="tableWrap">
      <table>
        <thead><tr><th>Имя</th><th class="size">Размер</th></tr></thead>
        <tbody id="rows"></tbody>
      </table>
    </div>
  </aside>
</main>
<script>
const DATA = __DATA__;

const els = {
  kind: document.getElementById('kind'),
  metric: document.getElementById('metric'),
  filter: document.getElementById('filter'),
  limit: document.getElementById('limit'),
  treemap: document.getElementById('treemap'),
  rows: document.getElementById('rows'),
  details: document.getElementById('details'),
  mapTitle: document.getElementById('mapTitle'),
};

function bytes(n) {
  if (!Number.isFinite(n)) return '0 B';
  if (n >= 1024 * 1024) return (n / 1024 / 1024).toFixed(2) + ' MB';
  if (n >= 1024) return (n / 1024).toFixed(1) + ' KB';
  return n + ' B';
}

function valueOf(item, metric) {
  if (metric === 'size') return item.size || 0;
  if (item.memory && Object.prototype.hasOwnProperty.call(item.memory, metric)) return item.memory[metric] || 0;
  if (item.sections && Object.prototype.hasOwnProperty.call(item.sections, metric)) return item.sections[metric] || 0;
  return 0;
}

function colorFor(name, value, maxValue) {
  let hash = 0;
  for (let i = 0; i < name.length; i++) hash = ((hash << 5) - hash + name.charCodeAt(i)) | 0;
  const hue = Math.abs(hash) % 360;
  const light = 34 + Math.min(22, Math.sqrt(value / Math.max(maxValue, 1)) * 22);
  return `hsl(${hue} 58% ${light}%)`;
}

function layout(items, x, y, w, h, out) {
  if (!items.length || w <= 1 || h <= 1) return;
  if (items.length === 1) {
    out.push({ item: items[0], x, y, w, h });
    return;
  }
  const total = items.reduce((sum, item) => sum + item.value, 0);
  let acc = 0;
  let split = 1;
  for (; split < items.length; split++) {
    if (acc + items[split - 1].value >= total / 2) break;
    acc += items[split - 1].value;
  }
  split = Math.max(1, Math.min(items.length - 1, split));
  const left = items.slice(0, split);
  const right = items.slice(split);
  const leftTotal = left.reduce((sum, item) => sum + item.value, 0);
  const ratio = leftTotal / total;
  if (w >= h) {
    const w1 = Math.round(w * ratio);
    layout(left, x, y, w1, h, out);
    layout(right, x + w1, y, w - w1, h, out);
  } else {
    const h1 = Math.round(h * ratio);
    layout(left, x, y, w, h1, out);
    layout(right, x, y + h1, w, h - h1, out);
  }
}

function visibleItems() {
  const kind = els.kind.value;
  const metric = els.metric.value;
  const filter = els.filter.value.trim().toLowerCase();
  const limit = Number(els.limit.value);
  return DATA[kind]
    .map(item => ({ ...item, value: valueOf(item, metric) }))
    .filter(item => item.value > 0)
    .filter(item => !filter || (item.name + ' ' + item.full).toLowerCase().includes(filter))
    .sort((a, b) => b.value - a.value)
    .slice(0, limit);
}

function render() {
  const items = visibleItems();
  const total = items.reduce((sum, item) => sum + item.value, 0);
  const maxValue = items[0]?.value || 1;
  els.mapTitle.textContent = `Treemap · ${items.length} элементов · ${bytes(total)}`;
  els.treemap.textContent = '';

  const rect = els.treemap.getBoundingClientRect();
  const placed = [];
  layout(items, 0, 0, Math.max(1, rect.width), Math.max(1, rect.height), placed);
  for (const node of placed) {
    const item = node.item;
    const cell = document.createElement('div');
    cell.className = 'cell';
    cell.style.left = node.x + 'px';
    cell.style.top = node.y + 'px';
    cell.style.width = Math.max(0, node.w) + 'px';
    cell.style.height = Math.max(0, node.h) + 'px';
    cell.style.background = colorFor(item.full, item.value, maxValue);
    cell.title = `${item.name}\n${bytes(item.value)}\n${item.full}`;
    if (node.w > 70 && node.h > 36) {
      cell.innerHTML = `<div class="cell-name">${escapeHtml(item.name)}</div><div class="cell-size">${bytes(item.value)}</div>`;
    }
    cell.addEventListener('mouseenter', () => showDetails(item));
    els.treemap.appendChild(cell);
  }

  els.rows.textContent = '';
  for (const item of items.slice(0, 60)) {
    const tr = document.createElement('tr');
    const pct = total ? Math.max(1, item.value / total * 100) : 0;
    tr.innerHTML = `<td>${escapeHtml(item.name)}<div class="bar"><i style="width:${pct}%"></i></div></td><td class="size">${bytes(item.value)}</td>`;
    tr.addEventListener('mouseenter', () => showDetails(item));
    els.rows.appendChild(tr);
  }
}

function showDetails(item) {
  const mem = Object.entries(item.memory || {}).filter(([, v]) => v > 0).map(([k, v]) => `${k}: ${bytes(v)}`).join('<br>');
  const sec = Object.entries(item.sections || {}).filter(([, v]) => v > 0).map(([k, v]) => `${k}: ${bytes(v)}`).join('<br>');
  els.details.innerHTML = `<b>${escapeHtml(item.name)}</b><br>${bytes(item.value)}<br><span title="${escapeHtml(item.full)}">${escapeHtml(item.full)}</span><br><br>${mem}<br>${sec ? '<br>' + sec : ''}`;
}

function escapeHtml(text) {
  return String(text).replace(/[&<>"']/g, ch => ({'&':'&amp;', '<':'&lt;', '>':'&gt;', '"':'&quot;', "'":'&#039;'}[ch]));
}

for (const el of [els.kind, els.metric, els.filter, els.limit]) el.addEventListener('input', render);
window.addEventListener('resize', render);
render();
</script>
</body>
</html>
"""


def build_html(map_file: pathlib.Path, archives: list[dict], files: list[dict]) -> str:
    import datetime as _dt

    data = {
        "archives": archives,
        "files": files,
    }
    html = HTML_TEMPLATE.replace("__DATA__", json.dumps(data, ensure_ascii=False, separators=(",", ":")))
    html = html.replace("__MAP_FILE__", escape(str(map_file)))
    html = html.replace("__GENERATED__", _dt.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
    return html


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate visual ESP-IDF firmware size HTML report.")
    parser.add_argument("map_file", nargs="?", default="build/w222.map", help="Path to ESP-IDF .map file")
    parser.add_argument("-o", "--output", default="build/firmware_size.html", help="Output HTML file")
    parser.add_argument("--idf-size", help="Path to ESP-IDF tools/idf_size.py")
    args = parser.parse_args()

    map_file = pathlib.Path(args.map_file).resolve()
    output = pathlib.Path(args.output).resolve()
    idf_size = find_idf_size(args.idf_size)

    archives = flatten_items(run_idf_size(idf_size, map_file, "--archives"), "archives")
    files = flatten_items(run_idf_size(idf_size, map_file, "--files"), "files")

    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(build_html(map_file, archives, files), encoding="utf-8")
    print(f"Wrote {output}")
    print(f"Archives: {len(archives)}, files: {len(files)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
