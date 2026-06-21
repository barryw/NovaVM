#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
BOOK_DIR="$SCRIPT_DIR"
HELP_DIR="$REPO_ROOT/docs/help"
OUTPUT_DIR="$BOOK_DIR"

# Optional docs build: if the PDF toolchain isn't installed, skip cleanly instead
# of failing the whole (ROM/test) build. CI/dev with pandoc+latexmk still builds it.
if ! command -v pandoc &>/dev/null || ! command -v latexmk &>/dev/null; then
    echo "build-pdf: pandoc/latexmk not found -- skipping PDF user guide (not required for ROM/tests)."
    exit 0
fi

# Check required tools
for tool in pandoc python3; do
    if ! command -v "$tool" &>/dev/null; then
        echo "ERROR: $tool is required but not installed." >&2
        exit 1
    fi
done

# Check Python yaml module
if ! python3 -c "import yaml" 2>/dev/null; then
    echo "ERROR: Python 'yaml' module required. Install with: pip3 install pyyaml" >&2
    exit 1
fi

# Check for LaTeX
if ! command -v latexmk &>/dev/null && ! command -v pdflatex &>/dev/null; then
    echo "ERROR: latexmk or pdflatex required. Install MacTeX or texlive." >&2
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

ASSEMBLED="$OUTPUT_DIR/assembled.md"

# Assemble markdown files
python3 "$HELP_DIR/assemble-book.py" \
    --manifest "$BOOK_DIR/book.yaml" \
    --help-dir "$HELP_DIR" \
    --output "$ASSEMBLED"

# Determine PDF engine and build engine-specific options
PDF_ENGINE="latexmk"
PANDOC_ARGS=()
if command -v latexmk &>/dev/null; then
    PANDOC_ARGS+=(--pdf-engine-opt=-pdf)
else
    PDF_ENGINE="pdflatex"
fi

# Check if template exists
if [ -f "$BOOK_DIR/latex-template.tex" ]; then
    PANDOC_ARGS+=(--template="$BOOK_DIR/latex-template.tex")
fi

# Include LaTeX header for tcolorbox environments and listings language
if [ -f "$BOOK_DIR/latex-header.tex" ]; then
    PANDOC_ARGS+=(-H "$BOOK_DIR/latex-header.tex")
fi

# Run Pandoc
pandoc "$ASSEMBLED" \
    --lua-filter="$BOOK_DIR/nova-filter.lua" \
    --pdf-engine="$PDF_ENGINE" \
    ${PANDOC_ARGS[@]+"${PANDOC_ARGS[@]}"} \
    -V geometry:margin=1in \
    -V fontsize=11pt \
    -o "$OUTPUT_DIR/novabasic_user_guide.pdf"

# Clean up intermediate
rm -f "$ASSEMBLED"

echo "PDF generated: $OUTPUT_DIR/novabasic_user_guide.pdf"
