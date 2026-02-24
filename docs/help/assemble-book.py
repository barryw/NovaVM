#!/usr/bin/env python3
"""Assemble Markdown help files into a single document for Pandoc."""

import argparse
import os
import glob
import yaml


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest", required=True)
    parser.add_argument("--help-dir", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    with open(args.manifest) as f:
        book = yaml.safe_load(f)

    parts = []

    # Metadata block
    title = book.get("title", "NovaBASIC User Guide")
    subtitle = book.get("subtitle", "")
    parts.append(f"---\ntitle: \"{title}\"\nsubtitle: \"{subtitle}\"\n---\n")

    # Chapters
    for chapter in book.get("chapters", []):
        parts.append(f"\n# {chapter['title']}\n")
        for src in chapter.get("sources", []):
            path = os.path.join(args.help_dir, src)
            if os.path.exists(path):
                parts.append(read_body(path))
            else:
                parts.append(f"\n*Content pending: {src}*\n")

    # Appendices
    for appendix in book.get("appendices", []):
        parts.append(f"\n# {appendix['title']}\n")
        if "auto_generate" in appendix:
            kind = appendix["auto_generate"]
            pattern = os.path.join(args.help_dir, "reference", kind, "*.md")
            files = sorted(glob.glob(pattern))
            for fpath in files:
                body = read_body(fpath)
                # Extract title from frontmatter for section heading
                title = extract_title(fpath)
                parts.append(f"\n## {title}\n")
                parts.append(body)
                parts.append("\n")
        else:
            for src in appendix.get("sources", []):
                path = os.path.join(args.help_dir, src)
                if os.path.exists(path):
                    parts.append(read_body(path))
                else:
                    parts.append(f"\n*Content pending: {src}*\n")

    with open(args.output, "w") as f:
        f.write("\n".join(parts))


def read_body(path):
    """Read a Markdown file, stripping YAML frontmatter."""
    with open(path) as f:
        content = f.read()
    if content.startswith("---"):
        try:
            end = content.index("---", 3)
            return content[end + 3:].strip()
        except ValueError:
            return content
    return content.strip()


def extract_title(path):
    """Extract the title from YAML frontmatter."""
    with open(path) as f:
        content = f.read()
    if content.startswith("---"):
        try:
            end = content.index("---", 3)
            fm = content[3:end]
            data = yaml.safe_load(fm)
            if isinstance(data, dict):
                return data.get("title", os.path.splitext(os.path.basename(path))[0])
        except (ValueError, yaml.YAMLError):
            pass
    return os.path.splitext(os.path.basename(path))[0]


if __name__ == "__main__":
    main()
