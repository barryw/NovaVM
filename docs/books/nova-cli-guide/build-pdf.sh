#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

for tool in pandoc latexmk; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "ERROR: $tool is required but not installed." >&2
        exit 1
    fi
done

pandoc \
    "$SCRIPT_DIR/metadata.yaml" \
    "$SCRIPT_DIR/chapters/nova-cli.md" \
    "$SCRIPT_DIR/chapters/ndi-format.md" \
    --from=gfm+smart \
    --pdf-engine=latexmk \
    --pdf-engine-opt=-pdf \
    --toc \
    --number-sections \
    -V documentclass=book \
    -V classoption=oneside \
    -V geometry:margin=0.85in \
    -V fontsize=10pt \
    -H "$SCRIPT_DIR/latex-header.tex" \
    -o "$SCRIPT_DIR/nova-cli-guide.pdf"

echo "PDF generated: $SCRIPT_DIR/nova-cli-guide.pdf"
